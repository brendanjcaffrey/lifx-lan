#include <stdio.h>
#include "../sender.h"
#include "helpers.h"

#define NUM_REQUIRED_ARGS 2
#define INITIAL_SEQNUM 220
int main(int argc, char** argv)
{
    if (argc < NUM_REQUIRED_ARGS || (argv[1][0] != '0' && argv[1][0] != '1')) {
        fprintf(stderr, "Usage: %s [0/1]\n", argv[0]);
        exit(2);
    }

    int num_lights = 0;
    uint64_t* light_ids = NULL;
    parse_light_ids(argc, argv, NUM_REQUIRED_ARGS, &num_lights, &light_ids);

    struct lifx_lan_sender s;
    lifx_lan_sender_init(&s);

    for (int i = 0; i < num_lights; ++i) {
        s.messages.seqnum = INITIAL_SEQNUM + i;
        lifx_lan_sender_device_set_power(&s, light_ids[i], argv[1][0] == '1');
        lifx_lan_sender_device_set_power(&s, light_ids[i], argv[1][0] == '1');
    }

    lifx_lan_sender_uninit(&s);
    free(light_ids);
    return 0;
}
