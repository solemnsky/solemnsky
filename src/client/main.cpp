/**
 * Client top-level.
 */

int main() {
  // TODO: commandline arguments?

  // and He said,
  ui::runSFML([]() { return std::make_unique<Client>(); });
  // and lo, there appeared a client
}
