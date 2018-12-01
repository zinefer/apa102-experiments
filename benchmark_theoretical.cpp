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



int main() {
  Timer tmr;

  wiringPiSetup();
  if(wiringPiSPISetup(0, 6000000) < 0) {
    std::cerr << "wiringPiSPISetup failed" << std::endl;
  }

  uint8_t num_leds, r, g, b, brightness;
  num_leds = 16;
  r = 0xFF;
  g = 0x00;
  b = 0x00;
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

    for(int i = 0; i < num_leds; i++) {
      uint8_t frame[4];
      frame[0] = 0b11100000 | (0b00011111 & brightness);
      frame[1] = b;
      frame[2] = g;
      frame[3] = r;

      wiringPiSPIDataRW(0, frame, 4);
    }

    for(int i = 0; i < 4; i++) {
      buf[0] = 0xFF;
      wiringPiSPIDataRW(0, buf, 1);
    }

  }

  t = tmr.elapsed();
  std::cout << t << std::endl;

}
