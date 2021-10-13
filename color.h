#ifndef H_LIFX_LAN_COLOR
#define H_LIFX_LAN_COLOR

#define LIFX_LAN_HUE_MAX        65535
#define LIFX_LAN_BRIGHTNESS_MAX 65535
#define LIFX_LAN_SATURATION_MAX 65535
#define LIFX_LAN_KELVIN_MIN     2500
#define LIFX_LAN_KELVIN_MAX     9000
#define LIFX_LAN_KELVIN_DEFAULT 3500


#include "types.h"

void lifx_lan_color_white(struct lifx_lan_light_color* color, uint16_t brightness);
void lifx_lan_color_white_kelvin(struct lifx_lan_light_color* color, uint16_t brightness, uint16_t kelvin);
void lifx_lan_color_hsb(struct lifx_lan_light_color* color, uint16_t h, uint16_t s, uint16_t b);
void lifx_lan_color_hsbk(struct lifx_lan_light_color* color, uint16_t h, uint16_t s, uint16_t b, uint16_t k);
void lifx_lan_color_rgb(struct lifx_lan_light_color* color, uint8_t red, uint8_t green, uint8_t blue);
void lifx_lan_color_rgbk(struct lifx_lan_light_color* color, uint8_t red, uint8_t green, uint8_t blue, uint16_t kelvin);

void lifx_lan_color_white(struct lifx_lan_light_color* color, uint16_t brightness)
{
    lifx_lan_color_white_kelvin(color, brightness, LIFX_LAN_KELVIN_DEFAULT);
}

void lifx_lan_color_white_kelvin(struct lifx_lan_light_color* color, uint16_t brightness, uint16_t kelvin)
{
    lifx_lan_color_hsbk(color, 0, 0, brightness, kelvin);
}

void lifx_lan_color_hsb(struct lifx_lan_light_color* color, uint16_t h, uint16_t s, uint16_t b)
{
    lifx_lan_color_hsbk(color, h, s, b, LIFX_LAN_KELVIN_DEFAULT);
}

void lifx_lan_color_hsbk(struct lifx_lan_light_color* color, uint16_t h, uint16_t s, uint16_t b, uint16_t k)
{
    assert(k >= LIFX_LAN_KELVIN_MIN && k <= LIFX_LAN_KELVIN_MAX);
    color->hue = h;
    color->saturation = s;
    color->brightness = b;
    color->kelvin = k;
}

void lifx_lan_color_rgb(struct lifx_lan_light_color* color, uint8_t red, uint8_t green, uint8_t blue)
{
    lifx_lan_color_rgbk(color, red, green, blue, LIFX_LAN_KELVIN_DEFAULT);
}

// this algorithm is copied almost directly from the lifx gem
void lifx_lan_color_rgbk(struct lifx_lan_light_color* color, uint8_t red, uint8_t green, uint8_t blue, uint16_t kelvin)
{
    double r = red / 255.0, g = green / 255.0, b = blue / 255.0;
    double max = (r > g) ? r : g; if (b > max) max = b;
    double min = (r < g) ? r : g; if (b < min) min = b;

    double hue = max;
    double delta = max - min;

    if (max == min) { hue = 0; }
    else
    {
        if (max == r) { hue = (g - b) / delta + (g < b ? 6 : 0); }
        else if (max == g) { hue = (b - r) / delta + 2; }
        else if (max == b) { hue = (r - g) / delta + 4; }
        else { assert(false); }
        hue *= (60.0/360.0);
    }

    color->hue = (uint16_t) hue * LIFX_LAN_HUE_MAX;
    color->saturation = (uint16_t) (max == 0 ? 0 : delta / max) * LIFX_LAN_SATURATION_MAX;
    color->brightness = (uint16_t) max * LIFX_LAN_BRIGHTNESS_MAX;
    color->kelvin = kelvin;
}

#endif
