#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include "../sender.h"

void parse_light_ids(int argc, char** argv, int required_args, int* num_lights_out, uint64_t** light_ids_out);
void receive_callback(uint16_t type, void* buf, size_t size);
bool already_seen(uint64_t light_id);
void add_to_seen_list(uint64_t light_id);
void add_to_expecting(uint64_t light_id);
void remove_from_expecting(uint64_t light_id);
void alarm_callback(int sig);

struct light_list
{
    uint64_t light_id;
    struct light_list* next;
};
// yeah the globals are gross, but there's no other way to pass state to the signal handler
struct light_list* seen = NULL;
struct light_list* expecting = NULL;

#define NUM_REQUIRED_ARGS 1
#define INITIAL_SEQNUM 320
int main(int argc, char** argv)
{
    int num_lights = 0;
    uint64_t* light_ids = NULL;
    parse_light_ids(argc, argv, NUM_REQUIRED_ARGS, &num_lights, &light_ids);

    struct lifx_lan_socket socket;
    lifx_lan_socket_init(&socket);

    struct lifx_lan_sender sender;
    lifx_lan_sender_init(&sender, &socket);

    for (int i = 0; i < num_lights; ++i) {
        sender.messages.seqnum = INITIAL_SEQNUM + i;
        // add LIFX_LAN_TARGET_ALL to the list here so we don't exit early
        add_to_expecting(light_ids[i]);
        lifx_lan_sender_light_get(&sender, light_ids[i]);
        if (light_ids[i] != LIFX_LAN_TARGET_ALL) { lifx_lan_sender_light_get(&sender, light_ids[i]); }
    }

    signal(SIGALRM, &alarm_callback);
    alarm(3);

    lifx_lan_socket_receive(&socket, &receive_callback);
    lifx_lan_socket_uninit(&socket);
}

void parse_light_ids(int argc, char** argv, int required_args, int* num_lights_out, uint64_t** light_ids_out)
{
    int num_lights = 0;
    uint64_t* light_ids = NULL;
    if (argc == required_args) {
        num_lights = 1;
        light_ids = malloc(sizeof(uint64_t));
        *light_ids = LIFX_LAN_TARGET_ALL;
    } else {
        num_lights = argc - required_args;
        light_ids = malloc(sizeof(uint64_t) * num_lights);
        char** light_strs = argv + required_args;

        for (int i = 0; i < num_lights; ++i) {
            light_ids[i] = strtoll(light_strs[i], NULL, 10);
        }
    }

    *num_lights_out = num_lights;
    *light_ids_out = light_ids;
}

void receive_callback(uint16_t type, void* buf, size_t size)
{
    if (type != LIFX_LAN_MESSAGE_TYPE_LIGHT_STATE) return;
    struct lifx_lan_light_state state;
    lifx_lan_messages_decode_light_state(buf, size, &state);

    if (already_seen(state.header.target)) { return; }

    add_to_seen_list(state.header.target);
    remove_from_expecting(state.header.target);

    printf("STATE: target(%llu) hue(%u) saturation(%u) brightness(%u) kelvin(%u) power(%u) label(%.*s)\n",
        state.header.target, state.color.hue, state.color.saturation, state.color.brightness,
        state.color.kelvin, state.power, (int) sizeof(state.label), state.label);

    if (expecting == NULL) { exit(1); }
}

bool already_seen(uint64_t light_id)
{
    struct light_list* entry = seen;
    while (entry != NULL)
    {
        if (entry->light_id == light_id) { return true; }
        entry = entry->next;
    }
    return false;
}

void add_to_seen_list(uint64_t light_id)
{
    struct light_list* entry = malloc(sizeof(struct light_list));
    entry->light_id = light_id;
    entry->next = seen;
    seen = entry;
}

void add_to_expecting(uint64_t light_id)
{
    struct light_list* entry = malloc(sizeof(struct light_list));
    entry->light_id = light_id;
    entry->next = expecting;
    expecting = entry;
}

void remove_from_expecting(uint64_t light_id)
{
    struct light_list** prev_next = &expecting;
    struct light_list* entry = expecting;

    while (entry != NULL)
    {
        if (entry->light_id == light_id)
        {
            *prev_next = entry->next;
            free(entry);
            return;
        }
        else
        {
            prev_next = &entry->next;
            entry = entry->next;
        }
    }
}

void alarm_callback(int sig)
{
    struct light_list* entry = expecting;
    while (entry != NULL)
    {
        // we may have added this to prevent exiting early on an empty list
        if (entry->light_id != LIFX_LAN_TARGET_ALL)
        {
            printf("expected but not seen: %llu\n", entry->light_id);
        }
        entry = entry->next;
    }

    exit(1); (void) sig;
}
