#ifndef H_LIFX_LAN_SENDER_H
#define H_LIFX_LAN_SENDER_H

#include <sys/socket.h>
#include "types.h"
#include "messages.h"

struct lifx_lan_sender
{
    int fd;
    struct sockaddr_in addr;
    struct lifx_lan_messages messages;
};

void lifx_lan_sender_init(struct lifx_lan_sender* sender);
void lifx_lan_sender_uninit(struct lifx_lan_sender* sender);

void lifx_lan_sender_get_service(struct lifx_lan_sender* sender);
void lifx_lan_sender_get(struct lifx_lan_sender* sender);
void lifx_lan_sender_set_power(struct lifx_lan_sender* s, bool powered_on, uint32_t duration_millis);

void lifx_lan_sender_send_(struct lifx_lan_sender* sender, struct lifx_lan_header* head);
void lifx_lan_sender_send_with_payload_(struct lifx_lan_sender* sender, struct lifx_lan_header* head,
    void* payload, size_t payload_size);

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
    struct lifx_lan_header head;
    lifx_lan_messages_encode_get_service(&s->messages, &head);
    lifx_lan_sender_send_(s, &head);
}

void lifx_lan_sender_get(struct lifx_lan_sender* s)
{
    struct lifx_lan_header head;
    lifx_lan_messages_encode_get(&s->messages, &head);
    lifx_lan_sender_send_(s, &head);
}

void lifx_lan_sender_set_power(struct lifx_lan_sender* s, bool powered_on, uint32_t duration_millis)
{
    struct lifx_lan_header head; struct lifx_lan_set_power_payload payload;
    lifx_lan_messages_encode_set_power(&s->messages, &head, &payload, powered_on, duration_millis);
    lifx_lan_sender_send_with_payload_(s, &head, &payload, sizeof(payload));
}


void lifx_lan_sender_send_(struct lifx_lan_sender* s, struct lifx_lan_header* head)
{
    lifx_lan_sender_send_with_payload_(s, head, NULL, 0);
}

void lifx_lan_sender_send_with_payload_(struct lifx_lan_sender* s, struct lifx_lan_header* head,
    void* payload, size_t payload_size)
{
    size_t total_size = LIFX_LAN_HEADER_SIZE + payload_size;
    uint8_t buf[total_size];

    memcpy(buf, head, LIFX_LAN_HEADER_SIZE);
    if (payload_size != 0) { memcpy(buf+LIFX_LAN_HEADER_SIZE, payload, payload_size); }

    assert(sendto(s->fd, buf, total_size, 0, (struct sockaddr*) &s->addr,
           sizeof(struct sockaddr_in)) == total_size);
}

#endif
