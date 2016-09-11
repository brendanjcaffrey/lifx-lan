void parse_light_ids(int argc, char** argv, int required_args, int* num_lights_out, uint64_t** light_ids_out)
{
    int num_lights = 0;
    uint64_t* light_ids = NULL;
    if (argc == required_args) {
        num_lights = 1;
        light_ids = malloc(sizeof(uint64_t));
        *light_ids = LIFX_LAN_TARGET_ALL;
    } else {
        num_lights = argc - required_args;
        light_ids = malloc(sizeof(uint64_t) * num_lights);
        char** light_strs = argv + required_args;

        for (int i = 0; i < num_lights; ++i) {
            light_ids[i] = strtoll(light_strs[i], NULL, 10);
        }
    }

    *num_lights_out = num_lights;
    *light_ids_out = light_ids;
}
