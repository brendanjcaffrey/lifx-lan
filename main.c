#include <stdio.h>
#include "sender.h"
#include "receiver.h"
#include "color.h"

// checks that argv[1] starts with test
#define ARG_IS(test) !memcmp(argv[1], test, strlen(test))

void receive(void);
void receive_callback(uint16_t type, void* buf, size_t size);

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [command]\n", argv[0]);
        exit(2);
    }

    struct lifx_lan_sender s;
    lifx_lan_sender_init(&s);
    struct lifx_lan_light_color color;

    // device messages
    if (ARG_IS("get-service")) { lifx_lan_sender_device_get_service(&s); receive(); }
    else if (ARG_IS("get-host-info")) { lifx_lan_sender_device_get_host_info(&s); receive(); }
    else if (ARG_IS("get-host-firmware")) { lifx_lan_sender_device_get_host_firmware(&s); receive(); }
    else if (ARG_IS("get-wifi-info")) { lifx_lan_sender_device_get_wifi_info(&s); receive(); }
    else if (ARG_IS("get-wifi-firmware")) { lifx_lan_sender_device_get_wifi_firmware(&s); receive(); }
    else if (ARG_IS("get-power")) { lifx_lan_sender_device_get_power(&s); receive(); }
    else if (ARG_IS("set-power") && argc == 3) { lifx_lan_sender_device_set_power(&s, argv[2][0] == '1'); }
    else if (ARG_IS("get-label")) { lifx_lan_sender_device_get_label(&s); receive(); }
    else if (ARG_IS("set-label") && argc == 3) { lifx_lan_sender_device_set_label(&s, argv[2]); }
    else if (ARG_IS("get-version")) { lifx_lan_sender_device_get_version(&s); receive(); }
    else if (ARG_IS("get-info")) { lifx_lan_sender_device_get_info(&s); receive(); }
    else if (ARG_IS("get-location")) { lifx_lan_sender_device_get_location(&s); receive(); }
    else if (ARG_IS("get-group")) { lifx_lan_sender_device_get_group(&s); receive(); }
    else if (ARG_IS("echo-request"))
    {
        uint8_t buf[64];
        memset(buf, 0, sizeof(buf));
        memcpy(buf, "test echo request", 17);
        lifx_lan_sender_device_echo_request(&s, buf);
        receive();
    }

    // light messages
    else if (ARG_IS("get")) { lifx_lan_sender_light_get(&s); receive(); }
    else if (ARG_IS("set-color"))
    {
        assert(argc == 7); // bin/lifx-lan set-color h s b k duration
        lifx_lan_color_hsbk(&color, atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
        lifx_lan_sender_light_set_color(&s, &color, atoi(argv[6]));
    }
    else if (ARG_IS("get-power-light")) { lifx_lan_sender_light_get_power(&s); receive(); }
    else if (ARG_IS("set-power") && argc == 4) { lifx_lan_sender_light_set_power(&s, argv[2][0] == '1', atoi(argv[3])); }

    else { fprintf(stderr, "Unrecognized command\n"); return 2; }

    lifx_lan_sender_uninit(&s);
    return 0;
}

void receive(void)
{
    struct lifx_lan_receiver r;
    lifx_lan_receiver_init(&r);
    lifx_lan_receiver_receive(&r, &receive_callback);
}

void receive_callback(uint16_t type, void* buf, size_t size)
{
    switch (type)
    {
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_SERVICE:;
        struct lifx_lan_device_state_service service;
        lifx_lan_messages_decode_device_state_service(buf, size, &service);
        printf("STATE-SERVICE: service(%u) port(%u)\n", service.service, service.port);
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_HOST_INFO:;
        struct lifx_lan_device_state_host_info host_info;
        lifx_lan_messages_decode_device_state_host_info(buf, size, &host_info);
        printf("STATE-HOST-INFO: signal(%g) tx(%u) rx(%u)\n", host_info.info.signal,
            host_info.info.tx, host_info.info.rx);
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_HOST_FIRMWARE:;
        struct lifx_lan_device_state_host_firmware host_firmware;
        lifx_lan_messages_decode_device_state_host_firmware(buf, size, &host_firmware);
        printf("STATE-HOST-FIRMWARE: build(%llu) version(%u)\n",
            host_firmware.firmware.build, host_firmware.firmware.version);
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_WIFI_INFO:;
        struct lifx_lan_device_state_wifi_info wifi_info;
        lifx_lan_messages_decode_device_state_wifi_info(buf, size, &wifi_info);
        printf("STATE-WIFI-INFO: signal(%g) tx(%u) rx(%u)\n", wifi_info.info.signal,
            wifi_info.info.tx, wifi_info.info.rx);
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_WIFI_FIRMWARE:;
        struct lifx_lan_device_state_wifi_firmware wifi_firmware;
        lifx_lan_messages_decode_device_state_wifi_firmware(buf, size, &wifi_firmware);
        printf("STATE-WIFI-FIRMWARE: build(%llu) version(%u)\n",
            wifi_firmware.firmware.build, wifi_firmware.firmware.version);
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_POWER:;
        struct lifx_lan_device_state_power power;
        lifx_lan_messages_decode_device_state_power(buf, size, &power);
        printf("STATE-POWER: level(%u)\n", power.level);
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_LABEL:;
        struct lifx_lan_device_state_label label;
        lifx_lan_messages_decode_device_state_label(buf, size, &label);
        printf("STATE-LABEL: label(%.*s)\n", (int) sizeof(label.label), label.label);
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_VERSION:;
        struct lifx_lan_device_state_version version;
        lifx_lan_messages_decode_device_state_version(buf, size, &version);
        printf("STATE-VERSION: vendor(%u) product(%u) version(%u)\n",
            version.vendor, version.product, version.version);
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_INFO:;
        struct lifx_lan_device_state_info info;
        lifx_lan_messages_decode_device_state_info(buf, size, &info);
        printf("STATE-INFO: time(%llu) uptime(%llu) downtime(%llu)\n",
            info.time, info.uptime, info.downtime);
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_ACKNOWLEDGEMENT:
        printf("ACKNOWLEDGEMENT\n");
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_LOCATION:;
        struct lifx_lan_device_state_location location;
        lifx_lan_messages_decode_device_state_location(buf, size, &location);
        printf("STATE-LOCATION: location(");
        for (int i = 0; i < sizeof(location.location.id); ++i) { printf("%02X", location.location.id[i]); }
        printf(") label(%.*s) updated_at(%llu)\n", (int) sizeof(location.location.label),
            location.location.label, location.location.updated_at);
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_GROUP:;
        struct lifx_lan_device_state_group group;
        lifx_lan_messages_decode_device_state_group(buf, size, &group);
        printf("STATE-GROUP: group(");
        for (int i = 0; i < sizeof(group.group.id); ++i) { printf("%02X", group.group.id[i]); }
        printf(") label(%.*s) updated_at(%llu)\n", (int) sizeof(group.group.label),
            group.group.label, group.group.updated_at);
        break;
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_ECHO_RESPONSE:;
        struct lifx_lan_device_echo_response echo;
        lifx_lan_messages_decode_device_echo_response(buf, size, &echo);
        printf("ECHO-RESPONSE: payload(%.*s)\n", (int) sizeof(echo.payload), echo.payload);
        break;
    case LIFX_LAN_MESSAGE_TYPE_LIGHT_STATE:;
        struct lifx_lan_light_state state;
        lifx_lan_messages_decode_light_state(buf, size, &state);
        printf("STATE: hue(%u) saturation(%u) brightness(%u) kelvin(%u) power(%u) label(%.*s)\n", state.color.hue,
            state.color.saturation, state.color.brightness, state.color.kelvin, state.power, (int) sizeof(state.label), state.label);
        break;
    case LIFX_LAN_MESSAGE_TYPE_LIGHT_STATE_POWER:;
        struct lifx_lan_light_state state_power;
        lifx_lan_messages_decode_light_state(buf, size, &state_power);
        printf("STATE-POWER-LIGHT: level(%u)\n", state.power);
        break;

    // ignore all outgoing messages
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_SERVICE:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_HOST_INFO:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_HOST_FIRMWARE:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_WIFI_INFO:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_WIFI_FIRMWARE:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_POWER:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_SET_POWER:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_LABEL:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_SET_LABEL:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_VERSION:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_INFO:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_LOCATION:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_GET_GROUP:
    case LIFX_LAN_MESSAGE_TYPE_DEVICE_ECHO_REQUEST:
    case LIFX_LAN_MESSAGE_TYPE_LIGHT_GET:
    case LIFX_LAN_MESSAGE_TYPE_LIGHT_SET_COLOR:
    case LIFX_LAN_MESSAGE_TYPE_LIGHT_GET_POWER:
    case LIFX_LAN_MESSAGE_TYPE_LIGHT_SET_POWER: break;

    default: printf("Dropping unknown message type(%u)\n", type); break;
    }
}
