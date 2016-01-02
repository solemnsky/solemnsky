#include "base/util.h"
#include <iostream>
#include "ui/control.h"
#include "demo/demo.h"

#include <stdio.h>

int main(int, char **) {
  runSFML(std::make_shared<Demo>());
}