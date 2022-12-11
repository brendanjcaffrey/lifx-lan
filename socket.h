#ifndef H_LIFX_LAN_SOCKET_H
#define H_LIFX_LAN_SOCKET_H

#include <sys/time.h>
#include <strings.h>
#include "types.h"

struct lifx_lan_socket
{
    int fd;
};

struct lifx_lan_recv_result
{
    bool msg_read;
    uint16_t type;
    size_t len;
    uint8_t buf[256];
};

void lifx_lan_socket_init(struct lifx_lan_socket* s)
{
    memset(s, 0, sizeof(*s));
    s->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); assert(s->fd != -1);

    struct sockaddr_in addr; memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    assert(bind(s->fd, (struct sockaddr*) &addr, sizeof(addr)) == 0);

    int yes = 1;
    assert(setsockopt(s->fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)) == 0);
}

void lifx_lan_socket_uninit(struct lifx_lan_socket* s)
{
    close(s->fd);
}

void lifx_lan_socket_set_receive_timeout(struct lifx_lan_socket* s, int sec, int usec)
{
    struct timeval tv = { .tv_sec = sec, .tv_usec = usec };
    setsockopt(s->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

void lifx_lan_socket_receive(struct lifx_lan_socket* s, void (*callback)(uint16_t type, void* buf, size_t len))
{
    uint8_t buf[256];
    uint16_t type;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    size_t type_offset = __builtin_offsetof(struct lifx_lan_header, type);
    ssize_t header_len = sizeof(struct lifx_lan_header);

    while (true)
    {
        ssize_t len = recvfrom(s->fd, buf, sizeof(buf), 0, (struct sockaddr *) &addr, &addr_len);
        if (len < header_len) { continue; }

        memcpy(&type, buf + type_offset, sizeof(type));
        callback(type, buf, len);
    }
}

bool lifx_lan_socket_receive_once(struct lifx_lan_socket* s, struct lifx_lan_recv_result* result)
{
    bzero(result, sizeof(*result));

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    size_t type_offset = __builtin_offsetof(struct lifx_lan_header, type);
    ssize_t header_len = sizeof(struct lifx_lan_header);

    ssize_t len = recvfrom(s->fd, result->buf, sizeof(result->buf), 0, (struct sockaddr *) &addr, &addr_len);
    if (len < header_len) { return false; }

    result->len = len;
    memcpy(&result->type, result->buf + type_offset, sizeof(result->type));
    return true;
}

#endif
