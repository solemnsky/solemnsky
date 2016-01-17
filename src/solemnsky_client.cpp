/**
 * Client main program.
 */
#include "client/client.h"

int main(int argc, char **argv) {
  ui::runSFML([]() { return std::make_unique<Client>(); });
}
