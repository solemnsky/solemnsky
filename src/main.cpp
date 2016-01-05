#include "base/util.h"
#include <iostream>
#include "ui/control.h"
#include "demo/demo.h"
#include "base/resources.h"

#include <stdio.h>

int main(int, char **) {
  loadResources();

  runSFML(std::make_shared<Demo>());
}