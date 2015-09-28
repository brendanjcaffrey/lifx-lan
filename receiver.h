#ifndef H_LIFX_LAN_RECEIVER_H
#define H_LIFX_LAN_RECEIVER_H

#include "types.h"

struct lifx_lan_receiver
{
    int fd;
};

void lifx_lan_receiver_init(struct lifx_lan_receiver* r);
void lifx_lan_receiver_receive(struct lifx_lan_receiver* r, void (*callback)(uint16_t type, void* buf, size_t len));

void lifx_lan_receiver_init(struct lifx_lan_receiver* r)
{
    r->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); assert(r->fd != -1);

    int yes = 1;
    assert(setsockopt(r->fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)) == 0);
    assert(setsockopt(r->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == 0);
    assert(setsockopt(r->fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) == 0);

    struct sockaddr_in addr; memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LIFX_LAN_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    assert(bind(r->fd, (struct sockaddr*) &addr, sizeof(addr)) == 0);
}

void lifx_lan_receiver_receive(struct lifx_lan_receiver* r, void (*callback)(uint16_t type, void* buf, size_t len))
{
    uint8_t buf[256];
    uint16_t type;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    size_t type_offset = __builtin_offsetof(struct lifx_lan_header, type);

    while (true)
    {
        size_t len = recvfrom(r->fd, buf, 256, 0, (struct sockaddr *) &addr, &addr_len);
        if (len < sizeof(struct lifx_lan_header)) { continue; }

        memcpy(&type, buf + type_offset, sizeof(type));
        callback(type, buf, len);
    }
}

#endif
