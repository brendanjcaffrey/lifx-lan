#ifndef H_LIFX_LAN_SENDER_H
#define H_LIFX_LAN_SENDER_H

#include "messages.h"
#include "socket.h"

struct lifx_lan_sender
{
    struct lifx_lan_socket* socket;
    struct lifx_lan_messages messages;
    struct sockaddr_in broadcast_addr;
};

void lifx_lan_sender_init(struct lifx_lan_sender* s, struct lifx_lan_socket* socket);

void lifx_lan_sender_device_get_service(struct lifx_lan_sender* s);
void lifx_lan_sender_device_get_host_info(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_device_get_host_firmware(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_device_get_wifi_info(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_device_get_wifi_firmware(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_device_get_power(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_device_set_power(struct lifx_lan_sender* s, uint64_t target, bool powered_on);
void lifx_lan_sender_device_get_label(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_device_set_label(struct lifx_lan_sender* s, uint64_t target, char* label);
void lifx_lan_sender_device_get_version(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_device_get_info(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_device_get_location(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_device_get_group(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_device_echo_request(struct lifx_lan_sender* s, uint64_t target, void* buf);

void lifx_lan_sender_light_get(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_light_set_color(struct lifx_lan_sender* s, uint64_t target,
    struct lifx_lan_light_color* color, uint32_t duration_millis);
void lifx_lan_sender_light_get_power(struct lifx_lan_sender* s, uint64_t target);
void lifx_lan_sender_light_set_power(struct lifx_lan_sender* s, uint64_t target,
    bool powered_on, uint32_t duration_millis);

void lifx_lan_sender_send_(struct lifx_lan_sender* s, void* msg, size_t msg_size);

void lifx_lan_sender_init(struct lifx_lan_sender* s, struct lifx_lan_socket* socket)
{
    s->socket = socket;

    memset(&s->broadcast_addr, 0, sizeof(s->broadcast_addr));
    s->broadcast_addr.sin_family = AF_INET;
    s->broadcast_addr.sin_port = htons(LIFX_LAN_PORT);
    s->broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
}

void lifx_lan_sender_device_get_service(struct lifx_lan_sender* s)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_device_get_service(&s->messages, &msg);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_get_host_info(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_device_get_host_info(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_get_host_firmware(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_device_get_host_firmware(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_get_wifi_info(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_device_get_wifi_info(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_get_wifi_firmware(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_device_get_wifi_firmware(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_get_power(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_device_get_power(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_set_power(struct lifx_lan_sender* s, uint64_t target, bool powered_on)
{
    struct lifx_lan_device_set_power msg;
    lifx_lan_messages_encode_device_set_power(&s->messages, &msg, target, powered_on);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_get_label(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_device_get_label(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_set_label(struct lifx_lan_sender* s, uint64_t target, char* label)
{
    struct lifx_lan_device_set_label msg;
    lifx_lan_messages_encode_device_set_label(&s->messages, &msg, target, label);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_get_version(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_device_get_version(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_get_info(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_device_get_info(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_get_location(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_device_get_location(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_get_group(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_device_get_group(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_device_echo_request(struct lifx_lan_sender* s, uint64_t target, void* buf)
{
    struct lifx_lan_device_echo_request msg;
    lifx_lan_messages_encode_device_echo_request(&s->messages, &msg, target, buf);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}


void lifx_lan_sender_light_get(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_light_get(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_light_set_color(struct lifx_lan_sender* s, uint64_t target,
    struct lifx_lan_light_color* color, uint32_t duration_millis)
{
    struct lifx_lan_light_set_color msg;
    lifx_lan_messages_encode_light_set_color(&s->messages, &msg, target, color, duration_millis);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_light_get_power(struct lifx_lan_sender* s, uint64_t target)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_light_get_power(&s->messages, &msg, target);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_light_set_power(struct lifx_lan_sender* s, uint64_t target,
    bool powered_on, uint32_t duration_millis)
{
    struct lifx_lan_light_set_power msg;
    lifx_lan_messages_encode_light_set_power(&s->messages, &msg, target, powered_on, duration_millis);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_send_(struct lifx_lan_sender* s, void* msg, size_t msg_size)
{
    assert(sendto(s->socket->fd, msg, msg_size, 0, (struct sockaddr*) &s->broadcast_addr, sizeof(s->broadcast_addr)) == msg_size);
}

#endif
