#include <sstream>
#include <iostream>
#include <cmath>
#include "util.h"

void app_log(LogType type, const std::string contents) {
  static std::vector<std::string> prefixes =
      {"INFO:   ",
       "NOTICE: ",
       "ERROR:  ",
       "        "};

  std::stringstream stream(contents);
  std::string line;

  bool isFirstLine(true);
  while (getline(stream, line, '\n')) {
    if (isFirstLine) {
      std::cout << prefixes[(int) type] + line + "\n";
      isFirstLine = false;
    } else {
      std::cout << prefixes[3] + line + "\n";
    }
  }
}

bool Ticker::tick(float delta) {
  cooldown += delta;
  float newCooldown = std::fmod(cooldown, period);
  active = newCooldown != cooldown;
  cooldown = newCooldown;
  return active;
}

void Ticker::prime() {
  cooldown = period;
}

void Ticker::wait() {
  cooldown = 0;
}

Ticker::operator bool() { return active; }

