#include <stdio.h>
#include <pthread.h>
#include <sys/un.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include "../sender.h"

#define MAX_TRACKED_LIGHTS 100
#define LIFX_RECV_TIMEOUT_SEC 0
#define LIFX_RECV_TIMEOUT_USEC 500
#define RESEND_SECONDS 1.0
#define GIVE_UP_SECONDS 300.0
#define UDS_SERVER_BACKLOG 5
#define UDS_RECV_TIMEOUT_SEC 1
#define UDS_RECV_TIMEOUT_USEC 0

struct thread_data
{
    pthread_mutex_t mutex;
    struct tracked_light* lights;
    size_t num_lights;
};

struct tracked_light
{
    uint64_t light_id;
    bool want_on;
    struct lifx_lan_light_color want_color;
    bool power_change_unconfirmed, color_change_unconfirmed;
    struct timespec last_sent, last_cmd_rcvd;

    struct tracked_light* next;
};

struct parsed_cmd
{
    uint64_t light_id;
    struct lifx_lan_light_color color;
    bool want_on, want_color_change;
};

void* run_lifx_thread(void* thread_data);
void process_lifx_msg(struct thread_data* data, struct lifx_lan_recv_result* result);
void process_lifx_power_msg(struct thread_data* data, struct lifx_lan_recv_result* result);
void process_lifx_light_msg(struct thread_data* data, struct lifx_lan_recv_result* result);
void send_lifx_msgs(struct thread_data* data, struct lifx_lan_sender* sender);
void send_lifx_light_msgs(struct lifx_lan_sender* sender, struct tracked_light* light, struct timespec* now);
void* run_uds_thread(struct thread_data* data, char const* path);
bool read_client_msg(int client_fd, struct parsed_cmd* cmd);
void handle_client_cmd(struct thread_data* data, struct parsed_cmd* cmd);
void get_now(struct timespec* ts);
struct tracked_light* find_light(struct thread_data* data, uint64_t light_id);

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s /path/to/sock\n", argv[0]);
        exit(2);
    }

    struct thread_data data;
    bzero(&data, sizeof(data));
    assert(pthread_mutex_init(&data.mutex, NULL) == 0);

    pthread_t lifx_thread_id;
    assert(pthread_create(&lifx_thread_id, NULL, run_lifx_thread, &data) == 0);
    run_uds_thread(&data, argv[1]);

    assert(pthread_join(lifx_thread_id, NULL) == 0);
    assert(pthread_mutex_destroy(&data.mutex) == 0);
    return 0;
}

void* run_lifx_thread(void* arg0)
{
    struct thread_data* data = arg0;

    struct lifx_lan_socket socket;
    lifx_lan_socket_init(&socket);
    lifx_lan_socket_set_receive_timeout(&socket, LIFX_RECV_TIMEOUT_SEC, LIFX_RECV_TIMEOUT_USEC);

    struct lifx_lan_sender sender;
    lifx_lan_sender_init(&sender, &socket);

    while (true)
    {
        struct lifx_lan_recv_result result;
        if (lifx_lan_socket_receive_once(&socket, &result))
        {
            process_lifx_msg(data, &result);
        }

        send_lifx_msgs(data, &sender);
    }

    return NULL;
}

void process_lifx_msg(struct thread_data* data, struct lifx_lan_recv_result* result)
{
    switch (result->type)
    {
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_POWER: process_lifx_power_msg(data, result); break;
    case LIFX_LAN_MESSAGE_TYPE_LIGHT_STATE: process_lifx_light_msg(data, result); break;
    default: break;
    }
}

void process_lifx_power_msg(struct thread_data* data, struct lifx_lan_recv_result* result)
{
    assert(pthread_mutex_lock(&data->mutex) == 0);

    struct lifx_lan_device_state_power power;
    lifx_lan_messages_decode_device_state_power(result->buf, result->len, &power);
    printf("STATE-POWER: target(%llu) level(%u)\n", power.header.target, power.level);

    struct tracked_light* light = find_light(data, power.header.target);
    if (light && light->power_change_unconfirmed)
    {
        bool light_is_on = power.level != 0;
        if (light->want_on == light_is_on)
        {
            printf("light %llu confirmed %s\n", power.header.target, light_is_on ? "on" : "off");
            light->power_change_unconfirmed = false;
        }
        else
        {
            printf("light %llu want %s but is %s\n", power.header.target, light->want_on ? "on" : "off", light_is_on ? "on" : "off");
        }
    }

    assert(pthread_mutex_unlock(&data->mutex) == 0);
}

void process_lifx_light_msg(struct thread_data* data, struct lifx_lan_recv_result* result)
{
    assert(pthread_mutex_lock(&data->mutex) == 0);

    struct lifx_lan_light_state state;
    lifx_lan_messages_decode_light_state(result->buf, result->len, &state);
    printf("LIGHT-STATE: target(%llu) hue(%u) saturation(%u) brightness(%u) kelvin(%u) power(%u) label(%.*s)\n",
        state.header.target, state.color.hue, state.color.saturation, state.color.brightness,
        state.color.kelvin, state.power, (int) sizeof(state.label), state.label);

    struct tracked_light* light = find_light(data, state.header.target);
    if (light && light->color_change_unconfirmed)
    {
        if (memcmp(&light->want_color, &state.color, sizeof(state.color)) == 0) // these are packed structs
        {
            printf("light %llu color confirmed\n", state.header.target);
            light->color_change_unconfirmed = false;
        }
        else
        {
            printf("light %llu want %u/%u/%u/%u but is %u/%u/%u/%u\n", state.header.target,
                light->want_color.hue, light->want_color.saturation, light->want_color.brightness, light->want_color.kelvin,
                state.color.hue, state.color.saturation, state.color.brightness, state.color.kelvin);
        }
    }

    assert(pthread_mutex_unlock(&data->mutex) == 0);
}

void send_lifx_msgs(struct thread_data* data, struct lifx_lan_sender* sender)
{
    assert(pthread_mutex_lock(&data->mutex) == 0);

    struct timespec now; get_now(&now);
    struct tracked_light* light = data->lights;
    while (light != NULL)
    {
        if (light->color_change_unconfirmed || light->power_change_unconfirmed)
        {
            send_lifx_light_msgs(sender, light, &now);
        }
        light = light->next;
    }

    assert(pthread_mutex_unlock(&data->mutex) == 0);
}

void send_lifx_light_msgs(struct lifx_lan_sender* sender, struct tracked_light* light, struct timespec* now)
{
    double since_sent = (now->tv_sec - light->last_sent.tv_sec) + (double) (now->tv_nsec - light->last_sent.tv_nsec) / 1E9;
    double since_cmd =  (now->tv_sec - light->last_cmd_rcvd.tv_sec) + (double) (now->tv_nsec - light->last_cmd_rcvd.tv_nsec) / 1E9;
    if (since_sent > RESEND_SECONDS && since_cmd < GIVE_UP_SECONDS)
    {
        light->last_sent = *now;
        if (light->color_change_unconfirmed)
        {
            struct lifx_lan_light_color* c = &light->want_color;
            printf("light %llu want %u/%u/%u/%u, sending\n", light->light_id, c->hue, c->saturation, c->brightness, c->kelvin);
            lifx_lan_sender_light_set_color(sender, light->light_id, &light->want_color, 0);
        }
        if (light->power_change_unconfirmed)
        {
            printf("light %llu want %s, sending\n", light->light_id, light->want_on ? "on" : "off");
            lifx_lan_sender_device_set_power(sender, light->light_id, light->want_on);
        }
    }
}

void* run_uds_thread(struct thread_data* data, char const* path)
{
    int server_fd;
    assert( (server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) > -1);

    struct sockaddr_un servaddr;
    bzero(&servaddr, sizeof(servaddr));
    assert(strlen(path) < sizeof(servaddr.sun_path));
    servaddr.sun_family = AF_UNIX;
    memcpy(&servaddr.sun_path, path, strlen(path)); // bzero takes care of the null termination

    int res = remove(path);
    assert(res == 0 || errno == ENOENT); // will return an error if the file doesn't exist, safe to ignore

    assert(bind(server_fd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == 0);
    assert(listen(server_fd, UDS_SERVER_BACKLOG) == 0);

    while (true)
    {
        int client_fd = accept(server_fd, NULL, NULL);
        assert(client_fd >= 0);

        struct timeval tv = { .tv_sec = UDS_RECV_TIMEOUT_SEC, .tv_usec = UDS_RECV_TIMEOUT_USEC };
        setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        struct parsed_cmd cmd;
        bool have_cmd = read_client_msg(client_fd, &cmd);
        close(client_fd);

        if (have_cmd) { handle_client_cmd(data, &cmd); }
    }

    return NULL;
}

void handle_client_cmd(struct thread_data* data, struct parsed_cmd* cmd)
{
    assert(pthread_mutex_lock(&data->mutex) == 0);

    struct tracked_light* light = find_light(data, cmd->light_id);
    if (light == NULL)
    {
        light = malloc(sizeof(struct tracked_light));
        bzero(light, sizeof(*light));
        light->light_id = cmd->light_id;

        light->next = data->lights;
        data->lights = light;

        ++data->num_lights;
        assert(data->num_lights <= MAX_TRACKED_LIGHTS);
    }

    light->power_change_unconfirmed = true;
    light->want_on = cmd->want_on;
    get_now(&light->last_cmd_rcvd);
    bzero(&light->last_sent, sizeof(light->last_sent));

    if (cmd->want_color_change)
    {
        light->color_change_unconfirmed = true;
        light->want_color = cmd->color;
    }

    assert(pthread_mutex_unlock(&data->mutex) == 0);
}

bool read_client_msg(int client_fd, struct parsed_cmd* cmd)
{
#define POWER_MSG_TOKENS 2
#define COLOR_MSG_TOKENS 6

    char buf[51]; bzero(buf, sizeof(buf));
    ssize_t len = read(client_fd, buf, sizeof(buf)-1); // leave the last character empty for null termination
    if (len < 3) { return false; }

    char orig[51]; memcpy(orig, buf, sizeof(buf));

    size_t token_count = 0;
    bzero(cmd, sizeof(*cmd));

    char* strtok_context = NULL;
    char* token = strtok_r(buf, ",", &strtok_context);
    while (token != NULL)
    {
        if (token_count >= COLOR_MSG_TOKENS)
        {
            printf("unable to parse '%.*s', too many tokens\n", (int) len, orig);
            return false;
        }

        if (strlen(token) == 0)
        {
            printf("unable to parse '%.*s', token %lu empty\n", (int) len, orig, token_count);
            return false;
        }
        for (int i = 0; i < strlen(token); ++i)
        {
            if (!isdigit(token[i]))
            {
                printf("unable to parse '%.*s', token %lu '%s' has non digits\n", (int) len, orig, token_count, token);
                return false;
            }
        }

        uint16_t* color_out = NULL;
        switch (++token_count)
        {
        case 1:
            cmd->light_id = strtoull(buf, NULL, 10);
            if (cmd->light_id == 0 || cmd->light_id == ULLONG_MAX)
            {
                printf("unable to parse '%.*s', error converting light_id '%s'\n", (int) len, orig, token);
                return false;
            }
            break;
        case 2:
            if (strlen(token) != 1 || (*token != '0' && *token != '1'))
            {
                printf("unable to parse '%.*s', invalid on/off value '%s'\n", (int) len, orig, token);
                return false;
            }
            cmd->want_on = *token == '1';
            break;
        case 3: color_out = &cmd->color.hue; break;
        case 4: color_out = &cmd->color.saturation; break;
        case 5: color_out = &cmd->color.brightness; break;
        case 6: color_out = &cmd->color.kelvin; break;
        default: abort();
        }

        if (color_out != NULL) { *color_out = strtol(token, NULL, 10); }
        token = strtok_r(NULL, ",", &strtok_context);
    }

    if (token_count == POWER_MSG_TOKENS)
    {
        printf("parsed '%.*s' to light_id=%llu want_on=%d\n", (int) len, orig, cmd->light_id, cmd->want_on);
    }
    else if (token_count == COLOR_MSG_TOKENS)
    {
        cmd->want_color_change = true;
        struct lifx_lan_light_color* c = &cmd->color;
        printf("parsed '%.*s' to light_id=%llu want_on=%d color=%d/%d/%d/%d\n", (int) len, orig, cmd->light_id, cmd->want_on, c->hue, c->saturation, c->brightness, c->kelvin);
    }
    else
    {
        printf("unable to parse '%.*s', invalid number of tokens %lu\n", (int) len, orig, token_count);
        return false;
    }

    return true;
}

void get_now(struct timespec* ts)
{
    assert(clock_gettime(CLOCK_MONOTONIC, ts) == 0);
}

struct tracked_light* find_light(struct thread_data* data, uint64_t light_id)
{
    struct tracked_light* light = data->lights;
    while (light != NULL)
    {
        if (light->light_id == light_id) { return light; }
        light = light->next;
    }

    return NULL;
}
