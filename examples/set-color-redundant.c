#include <stdio.h>
#include "../sender.h"
#include "../color.h"

int main(int argc, char** argv)
{
    if (argc != 6) {
        fprintf(stderr, "Usage: %s h s b k duration\n", argv[0]);
        exit(2);
    }

    struct lifx_lan_sender s;
    lifx_lan_sender_init(&s);
    s.messages.seqnum = 120;
    struct lifx_lan_light_color color;
    lifx_lan_color_hsbk(&color, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));

    lifx_lan_sender_light_set_color(&s, LIFX_LAN_TARGET_ALL, &color, atoi(argv[5]));
    lifx_lan_sender_light_set_color(&s, LIFX_LAN_TARGET_ALL, &color, atoi(argv[5]));

    lifx_lan_sender_uninit(&s);
    return 0;
}
