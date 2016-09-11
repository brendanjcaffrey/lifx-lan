#include <stdio.h>
#include "../sender.h"
#include "../color.h"
#include "../types.h"
#include "helpers.h"

#define NUM_REQUIRED_ARGS 6
#define INITIAL_SEQNUM 120
int main(int argc, char** argv)
{
    if (argc < NUM_REQUIRED_ARGS) {
        fprintf(stderr, "Usage: %s h s b k duration [optional list of light ids]\n", argv[0]);
        exit(2);
    }

    int num_lights = 0;
    uint64_t* light_ids = NULL;
    parse_light_ids(argc, argv, NUM_REQUIRED_ARGS, &num_lights, &light_ids);

    struct lifx_lan_sender s;
    lifx_lan_sender_init(&s);

    struct lifx_lan_light_color color;
    lifx_lan_color_hsbk(&color, strtol(argv[1], NULL, 10), strtol(argv[2], NULL, 10), strtol(argv[3], NULL, 10), strtol(argv[4], NULL, 10));
    uint32_t duration = strtol(argv[5], NULL, 10);

    for (int i = 0; i < num_lights; ++i) {
        s.messages.seqnum = INITIAL_SEQNUM + i;
        lifx_lan_sender_light_set_color(&s, light_ids[i], &color, duration);
        lifx_lan_sender_light_set_color(&s, light_ids[i], &color, duration);
    }

    lifx_lan_sender_uninit(&s);
    free(light_ids);
    return 0;
}
