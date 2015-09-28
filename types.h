#ifndef H_LIFX_LAN_TYPES_H
#define H_LIFX_LAN_TYPES_H

#include <arpa/inet.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LIFX_LAN_SOURCE_ID         0xBCBCBCBC
#define LIFX_LAN_PROTOCOL          1024
#define LIFX_LAN_SEND_PORT         56700
#define LIFX_LAN_RECV_PORT         56750
#define LIFX_LAN_SERVICE_UDP       1
#define LIFX_LAN_LEVEL_POWERED_ON  65535
#define LIFX_LAN_LEVEL_POWERED_OFF 0

#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_SERVICE       2
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_HOST_INFO     12
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_HOST_FIRMWARE 14
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_WIFI_INFO     16
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_WIFI_FIRMWARE 18
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_POWER         20
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_SET_POWER         21
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_LABEL         23
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_SET_LABEL         24
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_VERSION       32
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_INFO          34
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_LOCATION      48
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_GROUP         51
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_ECHO_REQUEST      58

#define LIFX_LAN_MESSAGE_TYPE_LIGHT_GET       101
#define LIFX_LAN_MESSAGE_TYPE_LIGHT_SET_COLOR 102
#define LIFX_LAN_MESSAGE_TYPE_LIGHT_GET_POWER 101
#define LIFX_LAN_MESSAGE_TYPE_LIGHT_SET_POWER 117

#pragma pack(push, 1)
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

struct lifx_lan_device_set_power
{
    struct lifx_lan_header head;
    uint16_t level;
};

struct lifx_lan_device_set_label
{
    struct lifx_lan_header head;
    char   label[32];
};

struct lifx_lan_device_echo_request
{
    struct lifx_lan_header head;
    uint8_t payload[64];
};

struct lifx_lan_color
{
    uint16_t hue;
    uint16_t saturation;
    uint16_t brightness;
    uint16_t kelvin;
};

struct lifx_lan_light_set_color
{
    struct lifx_lan_header head;
    uint8_t reserved;
    struct lifx_lan_color color;
    uint32_t duration_millis;
};

struct lifx_lan_light_set_power
{
    struct lifx_lan_header head;
    uint16_t level; // 0 or 65535
    uint32_t duration_millis;
};
#pragma pack(pop)

#define LIFX_LAN_HEADER_SIZE sizeof(struct lifx_lan_header)

#endif
