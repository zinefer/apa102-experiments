#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <iostream>
#include <stdint.h>
#include <unistd.h>



#include <iostream>
#include <ctime>

class Timer
{
public:
    Timer() { clock_gettime(CLOCK_REALTIME, &beg_); }

    double elapsed() {
        clock_gettime(CLOCK_REALTIME, &end_);
        return end_.tv_sec - beg_.tv_sec +
            (end_.tv_nsec - beg_.tv_nsec) / 1000000000.;
    }

    void reset() { clock_gettime(CLOCK_REALTIME, &beg_); }

private:
    timespec beg_, end_;
};






typedef struct RgbColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RgbColor;

typedef struct HsvColor
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
} HsvColor;

RgbColor HsvToRgb(HsvColor hsv)
{
    RgbColor rgb;
    unsigned char region, remainder, p, q, t;

    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6;

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            rgb.r = hsv.v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = hsv.v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = hsv.v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = hsv.v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = hsv.v;
            break;
        default:
            rgb.r = hsv.v; rgb.g = p; rgb.b = q;
            break;
    }

    return rgb;
}

HsvColor RgbToHsv(RgbColor rgb)
{
    HsvColor hsv;
    unsigned char rgbMin, rgbMax;

    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

    hsv.v = rgbMax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
    if (hsv.s == 0)
    {
        hsv.h = 0;
        return hsv;
    }

    if (rgbMax == rgb.r)
        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
    else if (rgbMax == rgb.g)
        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
    else
        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

    return hsv;
}








int main() {
  Timer tmr;

  wiringPiSetup();
  if(wiringPiSPISetup(0, 6000000) < 0) {
    std::cerr << "wiringPiSPISetup failed" << std::endl;
  }

  uint8_t num_leds, hue, brightness;
  num_leds = 16;
  brightness = 1;

  double t = tmr.elapsed();
  std::cout << t << std::endl;
  tmr.reset();

  for(int j = 0; j < 1000; j++) {

    uint8_t buf[1];
    for(int i = 0; i < 4; i++) {
      buf[0] = 0x00;
      wiringPiSPIDataRW(0, buf, 1);
    }

    HsvColor color = { hue++, 255, 240 };

    for(int i = 0; i < num_leds; i++) {
      RgbColor frame_color = HsvToRgb(color);

      uint8_t frame[4];
      frame[0] = 0b11100000 | (0b00011111 & brightness);
      frame[1] = frame_color.b;
      frame[2] = frame_color.g;
      frame[3] = frame_color.r;

      wiringPiSPIDataRW(0, frame, 4);

      color.h+=15;
    }

    for(int i = 0; i < 4; i++) {
      buf[0] = 0xFF;
      wiringPiSPIDataRW(0, buf, 1);
    }

  }

  t = tmr.elapsed();
  std::cout << t << std::endl;

}
