#include <stdio.h>
#include "../sender.h"
#include "../receiver.h"

void receive_callback(uint16_t type, void* buf, size_t size);
void alarm_callback(int sig);

int main(void)
{
    struct lifx_lan_sender s;
    lifx_lan_sender_init(&s);
    lifx_lan_sender_device_get_power(&s, LIFX_LAN_TARGET_ALL);
    lifx_lan_sender_uninit(&s);

    signal(SIGALRM, &alarm_callback);
    alarm(3);

    struct lifx_lan_receiver r;
    lifx_lan_receiver_init(&r);
    lifx_lan_receiver_receive(&r, &receive_callback);
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
