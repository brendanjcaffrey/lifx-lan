#include <stdio.h>
#include "sender.h"
#include "color.h"

// checks that argv[1] starts with test
#define ARG_IS(test) !memcmp(argv[1], test, strlen(test))

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [command]\n", argv[0]);
        exit(2);
    }

    struct lifx_lan_sender s;
    lifx_lan_sender_init(&s);
    struct lifx_lan_color color;

    // device messages
    if (ARG_IS("get-service")) { lifx_lan_sender_device_get_service(&s); }
    else if (ARG_IS("get-host-info")) { lifx_lan_sender_device_get_host_info(&s); }
    else if (ARG_IS("get-host-firmware")) { lifx_lan_sender_device_get_host_firmware(&s); }
    else if (ARG_IS("get-wifi-info")) { lifx_lan_sender_device_get_wifi_info(&s); }
    else if (ARG_IS("get-wifi-firmware")) { lifx_lan_sender_device_get_wifi_firmware(&s); }
    else if (ARG_IS("get-power")) { lifx_lan_sender_device_get_power(&s); }
    else if (ARG_IS("set-power") && argc == 3) { lifx_lan_sender_device_set_power(&s, argv[2][0] == '1'); }
    else if (ARG_IS("get-label")) { lifx_lan_sender_device_get_label(&s); }
    else if (ARG_IS("set-label") && argc == 3) { lifx_lan_sender_device_set_label(&s, argv[2]); }
    else if (ARG_IS("get-version")) { lifx_lan_sender_device_get_version(&s); }
    else if (ARG_IS("get-info")) { lifx_lan_sender_device_get_info(&s); }
    else if (ARG_IS("get-location")) { lifx_lan_sender_device_get_location(&s); }
    else if (ARG_IS("get-group")) { lifx_lan_sender_device_get_group(&s); }
    else if (ARG_IS("echo-request"))
    {
        uint8_t buf[64];
        memset(buf, 0, sizeof(buf));
        memcpy(buf, "echo request", 12);
        lifx_lan_sender_device_echo_request(&s, buf);
    }

    // light messages
    else if (ARG_IS("get")) { lifx_lan_sender_light_get(&s); }
    else if (ARG_IS("set-color"))
    {
        assert(argc == 7); // bin/lifx-lan set-color h s b k duration
        lifx_lan_color_hsbk(&color, atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
        lifx_lan_sender_light_set_color(&s, &color, atoi(argv[6]));
    }
    else if (ARG_IS("get-power")) { lifx_lan_sender_light_get_power(&s); }
    else if (ARG_IS("set-power") && argc == 4) { lifx_lan_sender_light_set_power(&s, argv[2][0] == '1', atoi(argv[3])); }

    else { fprintf(stderr, "Unrecognized command\n"); return 2; }

    lifx_lan_sender_uninit(&s);
    return 0;
}
