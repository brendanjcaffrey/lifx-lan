#include <stdio.h>
#include "sender.h"
#include "color.h"

// checks that argv[1] starts with test
#define ARG_IS(test) !memcmp(argv[1], test, strlen(test))
// checks if argv[1] ends with -slowly
#define SLOWLY (strlen(argv[1]) > 7 && !memcmp(argv[1]+strlen(argv[1])-7, "-slowly", 7))
#define DURATION (SLOWLY ? 5000 : 0)

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [command][-slowly]\n", argv[0]);
        exit(2);
    }

    struct lifx_lan_sender s;
    lifx_lan_sender_init(&s);
    struct lifx_lan_color color;

    if (ARG_IS("get-service")) { lifx_lan_sender_get_service(&s); }
    else if (ARG_IS("get")) { lifx_lan_sender_get(&s); }
    else if (ARG_IS("on")) { lifx_lan_sender_set_power(&s, true, DURATION); }
    else if (ARG_IS("off")) { lifx_lan_sender_set_power(&s, false, DURATION); }
    else if (ARG_IS("high"))
    {
        lifx_lan_color_white_kelvin(&color, LIFX_LAN_BRIGHTNESS_MAX, 6000);

        lifx_lan_sender_set_power(&s, true, DURATION);
        lifx_lan_sender_set_color(&s, &color, DURATION);
    }
    else if (ARG_IS("med"))
    {
        lifx_lan_color_white_kelvin(&color, LIFX_LAN_BRIGHTNESS_MAX*0.4, 4500);

        lifx_lan_sender_set_power(&s, true, DURATION);
        lifx_lan_sender_set_color(&s, &color, DURATION);
    }
    else if (ARG_IS("low"))
    {
        lifx_lan_color_white_kelvin(&color, LIFX_LAN_BRIGHTNESS_MAX*0.2, 3500);

        lifx_lan_sender_set_power(&s, true, DURATION);
        lifx_lan_sender_set_color(&s, &color, DURATION);
    }
    else if (ARG_IS("sun"))
    {
        lifx_lan_color_hsbk(&color, LIFX_LAN_HUE_MAX*0.15, LIFX_LAN_SATURATION_MAX*0.36, LIFX_LAN_BRIGHTNESS_MAX*0.85, 3000);

        lifx_lan_sender_set_power(&s, true, DURATION);
        lifx_lan_sender_set_color(&s, &color, DURATION);
    }
    else if (ARG_IS("white"))
    {
        assert(argc > 2);
        lifx_lan_sender_set_power(&s, true, DURATION);
        lifx_lan_color_white(&color, atoi(argv[2]));
        lifx_lan_sender_set_color(&s, &color, DURATION);
    }
    else { fprintf(stderr, "Unrecognized command\n"); return 2; }

    lifx_lan_sender_uninit(&s);
    return 0;
}
