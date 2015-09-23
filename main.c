#include <stdio.h>
#include "sender.h"

#define ARG_IS(test) !strcasecmp(argv[1], test)

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [command]\n", argv[0]);
        exit(2);
    }

    struct lifx_lan_sender s;
    lifx_lan_sender_init(&s);

    if (ARG_IS("get-service")) { lifx_lan_sender_get_service(&s); }
    else if (ARG_IS("get")) { lifx_lan_sender_get(&s); }
    else if (ARG_IS("on")) { lifx_lan_sender_set_power(&s, true, 0); }
    else if (ARG_IS("on-slowly")) { lifx_lan_sender_set_power(&s, true, 5000); }
    else if (ARG_IS("off")) { lifx_lan_sender_set_power(&s, false, 0); }
    else if (ARG_IS("off-slowly")) { lifx_lan_sender_set_power(&s, false, 5000); }
    else { fprintf(stderr, "Unrecognized command\n"); return 2; }

    lifx_lan_sender_uninit(&s);
    return 0;
}
