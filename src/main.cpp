#include "base/util.h"
#include <iostream>
#include "ui/control.h"
#include "demo/demo.h"

int main() {
  runSFML(std::make_shared<Demo>());
}