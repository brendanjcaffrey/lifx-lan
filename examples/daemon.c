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
    uint64_t id;
    bool want_on;
    bool change_unconfirmed;
    struct timespec last_sent;

    struct tracked_light* next;
};

struct parsed_cmd
{
    uint64_t id;
    bool want_on;
};

void* run_lifx_thread(void* thread_data);
void process_lifx_msg(struct thread_data* data, struct lifx_lan_recv_result* result);
void send_lifx_msgs(struct thread_data* data, struct lifx_lan_sender* sender);
void* run_uds_thread(struct thread_data* data, char const* path);
bool read_client_msg(int client_fd, struct parsed_cmd* cmd);
void handle_client_cmd(struct thread_data* data, struct parsed_cmd* cmd);
void get_now(struct timespec* ts);
struct tracked_light* find_light(struct thread_data* data, uint64_t id);

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
    if (result->type != LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_POWER) { return; } // all we care about

    assert(pthread_mutex_lock(&data->mutex) == 0);

    struct lifx_lan_device_state_power power;
    lifx_lan_messages_decode_device_state_power(result->buf, result->len, &power);
    printf("STATE-POWER: target(%llu) level(%u)\n", power.header.target, power.level);

    struct tracked_light* light = find_light(data, power.header.target);
    if (light->change_unconfirmed)
    {
        bool light_is_on = power.level != 0;
        if (light->want_on == light_is_on)
        {
            printf("light %llu confirmed %s\n", power.header.target, light_is_on ? "on" : "off");
            light->change_unconfirmed = false;
        }
        else
        {
            printf("light %llu want %s but is %s\n", power.header.target, light->want_on ? "on" : "off", light_is_on ? "on" : "off");
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
        if (light->change_unconfirmed)
        {
            double since_sent = (now.tv_sec - light->last_sent.tv_sec) + (double) (now.tv_nsec - light->last_sent.tv_nsec) / 1E9;
            if (since_sent > RESEND_SECONDS)
            {
                printf("light %llu want %s, sending\n", light->id, light->want_on ? "on" : "off");
                lifx_lan_sender_device_set_power(sender, light->id, light->want_on);
                light->last_sent = now;
            }
        }

        light = light->next;
    }

    assert(pthread_mutex_unlock(&data->mutex) == 0);
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

    struct tracked_light* light = find_light(data, cmd->id);
    if (light == NULL)
    {
        light = malloc(sizeof(struct tracked_light));
        bzero(light, sizeof(*light));
        light->id = cmd->id;

        light->next = data->lights;
        data->lights = light;

        ++data->num_lights;
        assert(data->num_lights <= MAX_TRACKED_LIGHTS);
    }

    light->change_unconfirmed = true;
    light->want_on = cmd->want_on;
    bzero(&light->last_sent, sizeof(light->last_sent));

    assert(pthread_mutex_unlock(&data->mutex) == 0);
}

bool read_client_msg(int client_fd, struct parsed_cmd* cmd)
{
    char buf[22]; bzero(buf, sizeof(buf)); // expecting a message like [id],1 which is max 21 chars
    ssize_t len = read(client_fd, buf, sizeof(buf));
    if (len < 3) { return false; }

    char* comma = NULL;
    for (int i = 0; i < len; ++i)
    {
        if (!isdigit(buf[i]))
        {
            // only allow 1 comma, not at the beginning or end
            if (buf[i] == ',' && comma == NULL && i != 0 && i != len-1)
            {
                comma = buf + i;
            }
            else
            {
                printf("unable to parse '%.*s', invalid characters\n", (int) len, buf);
                return false;
            }
        }
    }

    if (comma == NULL || (comma - buf) != len-2)
    {
        printf("unable to parse '%.*s', missing or invalid comma\n", (int) len, buf);
        return false;
    }

    char on_off = *(comma+1);
    if (on_off != '0' && on_off != '1')
    {
        printf("unable to parse '%.*s', invalid on/off value\n", (int) len, buf);
        return false;
    }
    bool want_on = on_off == '1';

    uint64_t id = strtoull(buf, NULL, 10);
    if (id == 0 || id == ULLONG_MAX)
    {
        printf("unable to parse '%.*s', error converting id\n", (int) len, buf);
        return false;
    }

    printf("parsed '%.*s' to id=%llu want_on=%d\n", (int) len, buf, id, want_on);
    cmd->id = id;
    cmd->want_on = want_on;
    return true;
}

void get_now(struct timespec* ts)
{
    assert(clock_gettime(CLOCK_MONOTONIC, ts) == 0);
}

struct tracked_light* find_light(struct thread_data* data, uint64_t id)
{
    struct tracked_light* light = data->lights;
    while (light != NULL)
    {
        if (light->id == id) { return light; }
        light = light->next;
    }

    return NULL;
}
