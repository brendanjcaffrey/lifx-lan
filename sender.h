#ifndef H_LIFX_LAN_SENDER_H
#define H_LIFX_LAN_SENDER_H

#include <sys/socket.h>
#include "types.h"
#include "messages.h"

struct lifx_lan_sender
{
    struct lifx_lan_messages messages;

    int fd;
    struct sockaddr_in addr;
};

void lifx_lan_sender_init(struct lifx_lan_sender* s);
void lifx_lan_sender_uninit(struct lifx_lan_sender* s);
void lifx_lan_sender_get_service(struct lifx_lan_sender* s);
void lifx_lan_sender_get(struct lifx_lan_sender* s);
void lifx_lan_sender_set_power(struct lifx_lan_sender* s, bool powered_on, uint32_t duration_millis);
void lifx_lan_sender_set_color(struct lifx_lan_sender* s, struct lifx_lan_color* color, uint32_t duration_millis);

void lifx_lan_sender_send_(struct lifx_lan_sender* s, void* msg, size_t msg_size);

void lifx_lan_sender_init(struct lifx_lan_sender* s)
{
    assert((s->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != -1);

    int bcast_enable = 1;
    assert(setsockopt(s->fd, SOL_SOCKET, SO_BROADCAST, &bcast_enable, sizeof(bcast_enable)) == 0);

    memset(&s->addr, 0, sizeof(s->addr));
    s->addr.sin_family = AF_INET;
    s->addr.sin_port = htons(LIFX_LAN_SEND_PORT);
    s->addr.sin_addr.s_addr = htonl(-1);
}

void lifx_lan_sender_uninit(struct lifx_lan_sender* s)
{
    close(s->fd);
}

void lifx_lan_sender_get_service(struct lifx_lan_sender* s)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_get_service(&s->messages, &msg);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_get(struct lifx_lan_sender* s)
{
    struct lifx_lan_header msg;
    lifx_lan_messages_encode_get(&s->messages, &msg);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_set_power(struct lifx_lan_sender* s, bool powered_on, uint32_t duration_millis)
{
    struct lifx_lan_set_power msg;
    lifx_lan_messages_encode_set_power(&s->messages, &msg, powered_on, duration_millis);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_set_color(struct lifx_lan_sender* s, struct lifx_lan_color* color, uint32_t duration_millis)
{
    struct lifx_lan_set_color msg;
    lifx_lan_messages_encode_set_color(&s->messages, &msg, color, duration_millis);
    lifx_lan_sender_send_(s, &msg, sizeof(msg));
}

void lifx_lan_sender_send_(struct lifx_lan_sender* s, void* msg, size_t msg_size)
{
    assert(sendto(s->fd, msg, msg_size, 0, (struct sockaddr*) &s->addr,
           sizeof(struct sockaddr_in)) == msg_size);
}

#endif
