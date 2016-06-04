#include <stdio.h>
#include "../sender.h"

int main(int argc, char** argv)
{
    if (argc != 2 || (argv[1][0] != '0' && argv[1][0] != '1')) {
        fprintf(stderr, "Usage: %s [0/1]\n", argv[0]);
        exit(2);
    }

    struct lifx_lan_sender s;
    lifx_lan_sender_init(&s);
    s.messages.seqnum = 240;

    lifx_lan_sender_device_set_power(&s, LIFX_LAN_TARGET_ALL, argv[1][0] == '1');
    lifx_lan_sender_device_set_power(&s, LIFX_LAN_TARGET_ALL, argv[1][0] == '1');

    lifx_lan_sender_uninit(&s);
    return 0;
}
