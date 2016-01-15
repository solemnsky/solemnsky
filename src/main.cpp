#include "base/util.h"
#include <iostream>
#include "ui/control.h"
#include "client/demo/demo.h"

int main(int, char **) {
  ui::runSFML(std::make_shared<Demo>());
}