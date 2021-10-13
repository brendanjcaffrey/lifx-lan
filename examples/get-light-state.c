#include <stdio.h>
#include <signal.h>
#include "../sender.h"

void receive_callback(uint16_t type, void* buf, size_t size);
void alarm_callback(int sig);

int main(void)
{
    struct lifx_lan_socket socket;
    lifx_lan_socket_init(&socket);

    struct lifx_lan_sender sender;
    lifx_lan_sender_init(&sender, &socket);
    lifx_lan_sender_light_get(&sender, LIFX_LAN_TARGET_ALL);

    signal(SIGALRM, &alarm_callback);
    alarm(3);

    lifx_lan_socket_receive(&socket, &receive_callback);
    lifx_lan_socket_uninit(&socket);
}

void receive_callback(uint16_t type, void* buf, size_t size)
{
    if (type != LIFX_LAN_MESSAGE_TYPE_LIGHT_STATE) return;
    struct lifx_lan_light_state state;
    lifx_lan_messages_decode_light_state(buf, size, &state);
    printf("STATE: target(%llu) hue(%u) saturation(%u) brightness(%u) kelvin(%u) power(%u) label(%.*s)\n",
        state.header.target, state.color.hue, state.color.saturation, state.color.brightness,
        state.color.kelvin, state.power, (int) sizeof(state.label), state.label);
}

void alarm_callback(int sig)
{
    exit(1); (void) sig;
}
