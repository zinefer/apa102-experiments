#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <iostream>
#include <stdint.h>
#include <unistd.h>

int to_int(char const *s);

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " NUM_LEDS -b BRIGHTNESS -d HUE_DELTA -h INITIAL_HUE -i HUE_INTERVAL" << std::endl;
    return 1;
  }

  int NUM_LEDS = to_int(argv[1]);

  uint8_t BRIGHTNESS = 15;
  uint8_t HUE_DELTA = 15;
  uint8_t INITIAL_HUE = 1;

  int HUE_INTERVAL = 250;

  for (int i = 2; i < argc; ++i) {

    std::string arg = std::string(argv[i]);

    std::cout << arg << ":" << i << std::endl;

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

  }

  wiringPiSetup();
  if(wiringPiSPISetup(0, 6000000) < 0) {
    std::cerr << "wiringPiSPISetup failed" << std::endl;
  }


  std::cout << "NUM_LEDS: " << NUM_LEDS << std::endl;
  std::cout << "BRIGHTNESS: " << (int)BRIGHTNESS << std::endl;
  std::cout << "HUE_DELTA: " << (int)HUE_DELTA << std::endl;
  std::cout << "INITIAL_HUE: " << (int)INITIAL_HUE << std::endl;
  std::cout << "HUE_INTERVAL: " << (int)HUE_INTERVAL << std::endl;

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
