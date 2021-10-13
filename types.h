#ifndef H_LIFX_LAN_TYPES_H
#define H_LIFX_LAN_TYPES_H

#include <arpa/inet.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LIFX_LAN_PROTOCOL          1024
#define LIFX_LAN_PORT              56700
#define LIFX_LAN_SERVICE_UDP       1
#define LIFX_LAN_LEVEL_POWERED_ON  65535
#define LIFX_LAN_LEVEL_POWERED_OFF 0
#define LIFX_LAN_TARGET_ALL        0

#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_SERVICE         2
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_SERVICE       3
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_HOST_INFO       12
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_HOST_INFO     13
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_HOST_FIRMWARE   14
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_HOST_FIRMWARE 15
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_WIFI_INFO       16
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_WIFI_INFO     17
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_WIFI_FIRMWARE   18
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_WIFI_FIRMWARE 19
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_POWER           20
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_SET_POWER           21
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_POWER         22
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_LABEL           23
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_SET_LABEL           24
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_LABEL         25
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_VERSION         32
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_VERSION       33
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_INFO            34
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_INFO          35
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_ACKNOWLEDGEMENT     45
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_LOCATION        48
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_LOCATION      50
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_GROUP           51
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_GROUP         53
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_ECHO_REQUEST        58
#define LIFX_LAN_MESSAGE_TYPE_DEVICE_ECHO_RESPONSE       59

#define LIFX_LAN_MESSAGE_TYPE_LIGHT_GET         101
#define LIFX_LAN_MESSAGE_TYPE_LIGHT_SET_COLOR   102
#define LIFX_LAN_MESSAGE_TYPE_LIGHT_STATE       107
#define LIFX_LAN_MESSAGE_TYPE_LIGHT_GET_POWER   116
#define LIFX_LAN_MESSAGE_TYPE_LIGHT_SET_POWER   117
#define LIFX_LAN_MESSAGE_TYPE_LIGHT_STATE_POWER 118

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

struct lifx_lan_device_state_service
{
    struct lifx_lan_header header;
    uint8_t service;
    uint32_t port;
};

struct lifx_lan_device_info
{
    float signal;
    uint32_t tx;
    uint32_t rx;
    int16_t reserved;
};

struct lifx_lan_device_firmware
{
    uint64_t build;
    uint64_t reserved;
    uint32_t version;
};

struct lifx_lan_device_state_host_info
{
    struct lifx_lan_header header;
    struct lifx_lan_device_info info;
};

struct lifx_lan_device_state_host_firmware
{
    struct lifx_lan_header header;
    struct lifx_lan_device_firmware firmware;
};

struct lifx_lan_device_state_wifi_info
{
    struct lifx_lan_header header;
    struct lifx_lan_device_info info;
};

struct lifx_lan_device_state_wifi_firmware
{
    struct lifx_lan_header header;
    struct lifx_lan_device_firmware firmware;
};

struct lifx_lan_device_set_power
{
    struct lifx_lan_header header;
    uint16_t level;
};

struct lifx_lan_device_state_power
{
    struct lifx_lan_header header;
    uint16_t level;
};

struct lifx_lan_device_set_label
{
    struct lifx_lan_header header;
    char   label[32];
};

struct lifx_lan_device_state_label
{
    struct lifx_lan_header header;
    char   label[32];
};

struct lifx_lan_device_state_version
{
    struct lifx_lan_header header;
    uint32_t vendor;
    uint32_t product;
    uint32_t version;
};

struct lifx_lan_device_state_info
{
    struct lifx_lan_header header;
    uint64_t time;
    uint64_t uptime;
    uint64_t downtime;
};

struct lifx_lan_device_location_group
{
    uint8_t id[16];
    char label[32];
    uint64_t updated_at;
};

struct lifx_lan_device_state_location
{
    struct lifx_lan_header header;
    struct lifx_lan_device_location_group location;
};

struct lifx_lan_device_state_group
{
    struct lifx_lan_header header;
    struct lifx_lan_device_location_group group;
};

struct lifx_lan_device_echo_request
{
    struct lifx_lan_header header;
    uint8_t payload[64];
};

struct lifx_lan_device_echo_response
{
    struct lifx_lan_header header;
    uint8_t payload[64];
};


struct lifx_lan_light_color
{
    uint16_t hue;
    uint16_t saturation;
    uint16_t brightness;
    uint16_t kelvin;
};

struct lifx_lan_light_set_color
{
    struct lifx_lan_header header;
    uint8_t reserved;
    struct lifx_lan_light_color color;
    uint32_t duration_millis;
};

struct lifx_lan_light_state
{
    struct lifx_lan_header header;
    struct lifx_lan_light_color color;
    int16_t reserved;
    uint16_t power;
    char label[32];
    uint64_t reserved2;
};

struct lifx_lan_light_set_power
{
    struct lifx_lan_header header;
    uint16_t level; // 0 or 65535
    uint32_t duration_millis;
};

struct lifx_lan_light_state_power
{
    struct lifx_lan_header header;
    uint16_t level; // 0 or 65535
};
#pragma pack(pop)

#define LIFX_LAN_HEADER_SIZE sizeof(struct lifx_lan_header)

#endif
