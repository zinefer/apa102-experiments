#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <iostream>
#include <stdint.h>
#include <unistd.h>

#include <timer.cpp>
#include <colors.cpp>

int to_int(char const *s);

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " NUM_LEDS -b BRIGHTNESS -d HUE_DELTA -h INITIAL_HUE -i HUE_INTERVAL -g CHANCE_OF_GLITTER" << std::endl;
    return 1;
  }

  int NUM_LEDS = to_int(argv[1]);

  uint8_t BRIGHTNESS = 15;
  uint8_t HUE_DELTA = 15;
  uint8_t INITIAL_HUE = 1;

  int HUE_INTERVAL = 20;

  bool CHANCE_OF_GLITTER = 0;

  for (int i = 2; i < argc; ++i) {

    std::string arg = std::string(argv[i]);

    if (arg == "-b") {
      BRIGHTNESS = to_int(argv[++i]);
    }

    if (arg == "-d") {
      HUE_DELTA = to_int(argv[++i]);
    }

    if (arg == "-h") {
      INITIAL_HUE = to_int(argv[++i]);
    }

    if (arg == "-i") {
      HUE_INTERVAL = to_int(argv[++i]);
    }

    if (arg == "-g") {
      CHANCE_OF_GLITTER = to_int(argv[++i]);
    }

  }

  wiringPiSetup();
  if(wiringPiSPISetup(0, 6000000) < 0) {
    std::cerr << "wiringPiSPISetup failed" << std::endl;
  }

  RgbColor white = { 255, 255, 255 };

  Timer timer;

  while(true)
  {

    if (timer.elapsed() >= HUE_INTERVAL) {
      timer.reset();

      INITIAL_HUE = (INITIAL_HUE + 1) % 255;
    }

    uint8_t buf[1];
    for(int i = 0; i < 4; i++) {
      buf[0] = 0x00;
      wiringPiSPIDataRW(0, buf, 1);
    }

    HsvColor color = { INITIAL_HUE, 255, 240 };

    for(int i = 0; i < NUM_LEDS; i++) {
      RgbColor frame_color = HsvToRgb(color);

      if (CHANCE_OF_GLITTER > 0) {
        if ((rand() % 100) < CHANCE_OF_GLITTER) {
          frame_color = white;
        }
      }

      uint8_t frame[4];
      frame[0] = 0b11100000 | (0b00011111 & BRIGHTNESS);
      frame[1] = frame_color.b;
      frame[2] = frame_color.g;
      frame[3] = frame_color.r;

      wiringPiSPIDataRW(0, frame, 4);

      color.h = (color.h + HUE_DELTA) % 255;
    }

    for(int i = 0; i < NUM_LEDS; i++) {
      buf[0] = 0xFF;
      wiringPiSPIDataRW(0, buf, 1);
    }

  }

}

// https://stackoverflow.com/questions/4442658/c-parse-int-from-string
int to_int(char const *s)
{
  if ( s == NULL || *s == '\0' )
    throw std::invalid_argument("null or empty string argument");

  bool negate = (s[0] == '-');
  if ( *s == '+' || *s == '-' )
     ++s;

  if ( *s == '\0')
    throw std::invalid_argument("sign character only.");

  int result = 0;
  while(*s)
  {
      if ( *s >= '0' && *s <= '9' )
      {
          result = result * 10  - (*s - '0');  //assume negative number
      }
      else
          throw std::invalid_argument("invalid input string");
      ++s;
  }
  return negate ? result : -result; //-result is positive!
}
