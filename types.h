#ifndef H_LIFX_LAN_TYPES_H
#define H_LIFX_LAN_TYPES_H

#include <arpa/inet.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LIFX_LAN_SOURCE_ID 0xBCBCBCBC
#define LIFX_LAN_PROTOCOL 1024
#define LIFX_LAN_SEND_PORT 56700
#define LIFX_LAN_RECV_PORT 56750
#define LIFX_LAN_SERVICE_UDP 1
#define LIFX_LAN_LEVEL_POWERED_ON 65535
#define LIFX_LAN_LEVEL_POWERED_OFF 0

#define LIFX_LAN_MESSAGE_TYPE_GET_SERVICE 2
#define LIFX_LAN_MESSAGE_TYPE_GET         101
#define LIFX_LAN_MESSAGE_TYPE_SET_POWER   117
#define LIFX_LAN_MESSAGE_TYPE_SET_COLOR   102

#pragma pack(push ,1)
struct lifx_lan_header
{
    uint16_t size; // size of entire message, including this field
    uint16_t protocol:12; // must be 1024
    uint8_t  addressable:1; // must be 1
    uint8_t  tagged:1; // determines usage of target field
    uint8_t  origin:2; // must be 0
    uint32_t source; // set by client, used by responses
    uint64_t target; // 6 byte MAC to target specific bulb (left padded w/ 0's) or 0 for all
    uint8_t  reserved[6];
    uint8_t  res_required:1;
    uint8_t  ack_required:1;
    uint8_t  :6; // reserved
    uint8_t  sequence; // wrap around sequence number
    uint64_t :64; // reserved
    uint16_t type; // message type
    uint16_t :16; // reserved
};

struct lifx_lan_set_power
{
    struct lifx_lan_header head;
    uint16_t level; // 0 or 65535
    uint32_t duration; // milliseconds
};

struct lifx_lan_color
{
    uint16_t hue;
    uint16_t saturation;
    uint16_t brightness;
    uint16_t kelvin;
};

struct lifx_lan_set_color
{
    struct lifx_lan_header head;
    uint8_t  reserved;
    struct lifx_lan_color color;
    uint32_t duration;
};
#pragma pack(pop)

#define LIFX_LAN_HEADER_SIZE sizeof(struct lifx_lan_header)

#endif
