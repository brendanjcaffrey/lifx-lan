#ifndef H_LIFX_LAN_COLOR
#define H_LIFX_LAN_COLOR

#define LIFX_LAN_HUE_MAX 65535
#define LIFX_LAN_BRIGHTNESS_MAX 65535
#define LIFX_LAN_SATURATION_MAX 65535
#define LIFX_LAN_KELVIN_MIN 2500
#define LIFX_LAN_KELVIN_MAX 9000
#define LIFX_LAN_KELVIN_DEFAULT 3500


#include "types.h"

void lifx_lan_color_white(struct lifx_lan_color* color, uint16_t brightness);
void lifx_lan_color_white_kelvin(struct lifx_lan_color* color, uint16_t brightness, uint16_t kelvin);
void lifx_lan_color_hsb(struct lifx_lan_color* color, uint16_t h, uint16_t s, uint16_t b);
void lifx_lan_color_hsbk(struct lifx_lan_color* color, uint16_t h, uint16_t s, uint16_t b, uint16_t k);

void lifx_lan_color_white(struct lifx_lan_color* color, uint16_t brightness)
{
    lifx_lan_color_white_kelvin(color, brightness, LIFX_LAN_KELVIN_DEFAULT);
}

void lifx_lan_color_white_kelvin(struct lifx_lan_color* color, uint16_t brightness, uint16_t kelvin)
{
    lifx_lan_color_hsbk(color, 0, 0, brightness, LIFX_LAN_KELVIN_DEFAULT);
}

void lifx_lan_color_hsb(struct lifx_lan_color* color, uint16_t h, uint16_t s, uint16_t b)
{
    lifx_lan_color_hsbk(color, h, s, b, LIFX_LAN_KELVIN_DEFAULT);
}

void lifx_lan_color_hsbk(struct lifx_lan_color* color, uint16_t h, uint16_t s, uint16_t b, uint16_t k)
{
    assert(k >= LIFX_LAN_KELVIN_MIN && k <= LIFX_LAN_KELVIN_MAX);
    color->hue = h;
    color->saturation = s;
    color->brightness = b;
    color->kelvin = k;
}

#endif
