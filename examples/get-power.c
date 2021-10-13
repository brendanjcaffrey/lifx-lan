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
    lifx_lan_sender_device_get_power(&sender, LIFX_LAN_TARGET_ALL);

    signal(SIGALRM, &alarm_callback);
    alarm(3);

    lifx_lan_socket_receive(&socket, &receive_callback);
    lifx_lan_socket_uninit(&socket);
}

void receive_callback(uint16_t type, void* buf, size_t size)
{
    if (type != LIFX_LAN_MESSAGE_TYPE_DEVICE_STATE_POWER) return;
    struct lifx_lan_device_state_power power;
    lifx_lan_messages_decode_device_state_power(buf, size, &power);
    printf("STATE-POWER: target(%llu) level(%u)\n", power.header.target, power.level);
}

void alarm_callback(int sig)
{
    exit(1); (void) sig;
}
