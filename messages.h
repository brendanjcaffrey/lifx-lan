#ifndef H_LIFX_LAN_MESSAGES_H
#define H_LIFX_LAN_MESSAGES_H

#include "types.h"

struct lifx_lan_messages
{
    uint8_t seqnum;
};

void lifx_lan_messages_init(struct lifx_lan_messages* m);
void lifx_lan_messages_encode_header(struct lifx_lan_messages* m, struct lifx_lan_header* head, uint16_t message_type, size_t payload_size);

void lifx_lan_messages_encode_device_get_service(struct lifx_lan_messages* m, struct lifx_lan_header* head);
void lifx_lan_messages_encode_device_get_host_info(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_device_get_host_firmware(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_device_get_wifi_info(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_device_get_wifi_firmware(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_device_get_power(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_device_set_power(struct lifx_lan_messages* m,
    struct lifx_lan_device_set_power* msg, bool powered_on);
void lifx_lan_messages_encode_device_get_label(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_device_set_label(struct lifx_lan_messages* m,
    struct lifx_lan_device_set_label* msg, char* label);
void lifx_lan_messages_encode_device_get_version(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_device_get_info(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_device_get_location(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_device_get_group(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_device_echo_request(struct lifx_lan_messages* m,
    struct lifx_lan_device_echo_request* msg, void* payload);

void lifx_lan_messages_encode_light_get(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_light_set_color(struct lifx_lan_messages* m, struct lifx_lan_light_set_color* msg,
    struct lifx_lan_light_color* color, uint32_t duration_millis);
void lifx_lan_messages_encode_light_get_power(struct lifx_lan_messages* m, struct lifx_lan_header* msg);
void lifx_lan_messages_encode_light_set_power(struct lifx_lan_messages* m, struct lifx_lan_light_set_power* msg,
    bool powered_on, uint32_t duration_millis);

void lifx_lan_messages_decode_device_state_service(void* buf, size_t size, struct lifx_lan_device_state_service* out);
void lifx_lan_messages_decode_device_state_host_info(void* buf, size_t size, struct lifx_lan_device_state_host_info* out);
void lifx_lan_messages_decode_device_state_host_firmware(void* buf, size_t size, struct lifx_lan_device_state_host_firmware* out);
void lifx_lan_messages_decode_device_state_wifi_info(void* buf, size_t size, struct lifx_lan_device_state_wifi_info* out);
void lifx_lan_messages_decode_device_state_wifi_firmware(void* buf, size_t size, struct lifx_lan_device_state_wifi_firmware* out);
void lifx_lan_messages_decode_device_state_power(void* buf, size_t size, struct lifx_lan_device_state_power* out);
void lifx_lan_messages_decode_device_state_label(void* buf, size_t size, struct lifx_lan_device_state_label* out);
void lifx_lan_messages_decode_device_state_version(void* buf, size_t size, struct lifx_lan_device_state_version* out);
void lifx_lan_messages_decode_device_state_info(void* buf, size_t size, struct lifx_lan_device_state_info* out);
void lifx_lan_messages_decode_device_state_location(void* buf, size_t size, struct lifx_lan_device_state_location* out);
void lifx_lan_messages_decode_device_state_group(void* buf, size_t size, struct lifx_lan_device_state_group* out);
void lifx_lan_messages_decode_device_echo_response(void* buf, size_t size, struct lifx_lan_device_echo_response* out);

void lifx_lan_messages_decode_light_state(void* buf, size_t size, struct lifx_lan_light_state* out);
void lifx_lan_messages_decode_light_state_power(void* buf, size_t size, struct lifx_lan_light_state_power* out);


void lifx_lan_messages_init(struct lifx_lan_messages* m)
{
    m->seqnum = 0;
}

void lifx_lan_messages_encode_header(struct lifx_lan_messages* m, struct lifx_lan_header* head, uint16_t message_type, size_t msg_size)
{
    memset(head, 0, LIFX_LAN_HEADER_SIZE);
    assert(msg_size < UINT16_MAX);

    head->size = msg_size;
    head->protocol = LIFX_LAN_PROTOCOL;
    head->addressable = 1;
    head->source = LIFX_LAN_SOURCE_ID;
    head->sequence = m->seqnum++;
    head->tagged = 1;
    head->type = message_type;
    head->res_required = 1;
}

void lifx_lan_messages_encode_device_get_service(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_SERVICE, sizeof(*msg));
}

void lifx_lan_messages_encode_device_get_host_info(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_HOST_INFO, sizeof(*msg));
}

void lifx_lan_messages_encode_device_get_host_firmware(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_HOST_FIRMWARE, sizeof(*msg));
}

void lifx_lan_messages_encode_device_get_wifi_info(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_WIFI_INFO, sizeof(*msg));
}

void lifx_lan_messages_encode_device_get_wifi_firmware(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_WIFI_FIRMWARE, sizeof(*msg));
}

void lifx_lan_messages_encode_device_get_power(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_POWER, sizeof(*msg));
}

void lifx_lan_messages_encode_device_set_power(struct lifx_lan_messages* m,
    struct lifx_lan_device_set_power* msg, bool powered_on)
{
    lifx_lan_messages_encode_header(m, &msg->header, LIFX_LAN_MESSAGE_TYPE_DEVICE_SET_POWER, sizeof(*msg));
    msg->level = powered_on ? LIFX_LAN_LEVEL_POWERED_ON : LIFX_LAN_LEVEL_POWERED_OFF;
}

void lifx_lan_messages_encode_device_get_label(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_LABEL, sizeof(*msg));
}

void lifx_lan_messages_encode_device_set_label(struct lifx_lan_messages* m,
    struct lifx_lan_device_set_label* msg, char* label)
{
    lifx_lan_messages_encode_header(m, &msg->header, LIFX_LAN_MESSAGE_TYPE_DEVICE_SET_LABEL, sizeof(*msg));

    assert(strlen(label) <= sizeof(msg->label));
    memset(msg->label, 0, sizeof(msg->label));
    memcpy(msg->label, label, strlen(label));
}

void lifx_lan_messages_encode_device_get_version(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_VERSION, sizeof(*msg));
}

void lifx_lan_messages_encode_device_get_info(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_INFO, sizeof(*msg));
}

void lifx_lan_messages_encode_device_get_location(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_LOCATION, sizeof(*msg));
}

void lifx_lan_messages_encode_device_get_group(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_GROUP, sizeof(*msg));
}

void lifx_lan_messages_encode_device_echo_request(struct lifx_lan_messages* m,
    struct lifx_lan_device_echo_request* msg, void* payload)
{
    lifx_lan_messages_encode_header(m, &msg->header, LIFX_LAN_MESSAGE_TYPE_DEVICE_ECHO_REQUEST, sizeof(*msg));
    memcpy(msg->payload, payload, sizeof(msg->payload));
}


void lifx_lan_messages_encode_light_get(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_LIGHT_GET, sizeof(*msg));
}

void lifx_lan_messages_encode_light_set_color(struct lifx_lan_messages* m, struct lifx_lan_light_set_color* msg,
    struct lifx_lan_light_color* color, uint32_t duration_millis)
{
    lifx_lan_messages_encode_header(m, &msg->header, LIFX_LAN_MESSAGE_TYPE_LIGHT_SET_COLOR, sizeof(*msg));
    memcpy(&msg->color, color, sizeof(struct lifx_lan_light_color));
    msg->duration_millis = duration_millis;
}

void lifx_lan_messages_encode_light_get_power(struct lifx_lan_messages* m, struct lifx_lan_header* msg)
{
    lifx_lan_messages_encode_header(m, msg, LIFX_LAN_MESSAGE_TYPE_LIGHT_GET_POWER, sizeof(*msg));
}

void lifx_lan_messages_encode_light_set_power(struct lifx_lan_messages* m, struct lifx_lan_light_set_power* msg,
    bool powered_on, uint32_t duration_millis)
{
    lifx_lan_messages_encode_header(m, &msg->head, LIFX_LAN_MESSAGE_TYPE_LIGHT_SET_POWER, sizeof(*msg));
    msg->level = powered_on ? LIFX_LAN_LEVEL_POWERED_ON : LIFX_LAN_LEVEL_POWERED_OFF;
    msg->duration_millis = duration_millis;
}


void lifx_lan_messages_decode_device_state_service(void* buf, size_t size, struct lifx_lan_device_state_service* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_device_state_host_info(void* buf, size_t size, struct lifx_lan_device_state_host_info* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_device_state_host_firmware(void* buf, size_t size, struct lifx_lan_device_state_host_firmware* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_device_state_wifi_info(void* buf, size_t size, struct lifx_lan_device_state_wifi_info* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_device_state_wifi_firmware(void* buf, size_t size, struct lifx_lan_device_state_wifi_firmware* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_device_state_power(void* buf, size_t size, struct lifx_lan_device_state_power* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_device_state_label(void* buf, size_t size, struct lifx_lan_device_state_label* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_device_state_version(void* buf, size_t size, struct lifx_lan_device_state_version* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_device_state_info(void* buf, size_t size, struct lifx_lan_device_state_info* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_device_state_location(void* buf, size_t size, struct lifx_lan_device_state_location* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_device_state_group(void* buf, size_t size, struct lifx_lan_device_state_group* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_device_echo_response(void* buf, size_t size, struct lifx_lan_device_echo_response* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_light_state(void* buf, size_t size, struct lifx_lan_light_state* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

void lifx_lan_messages_decode_light_state_power(void* buf, size_t size, struct lifx_lan_light_state_power* out)
{
    assert(size == sizeof(*out)); memcpy(out, buf, size);
}

#endif
