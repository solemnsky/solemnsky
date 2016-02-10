#include "telegraph/telegraph.h"
#include "util/methods.h"
#include "gtest/gtest.h"

struct server_client {
  unsigned int uid;
  unsigned get_uid() const { return uid; }
};

unsigned int nextUid = 0;
void initClient(server_client &client, const char *ip) {
  client.uid = nextUid;
  nextUid++;
}

class TelegraphFixture: public testing::Test {
 public:
  enetpp::client client;
  enetpp::server<server_client> server;

  TelegraphFixture() {
    enetpp::global_state::get().initialize();
    server.start_listening(
        enetpp::server_listen_params<server_client>().
            set_max_client_count(20).
            set_channel_count(1).
            set_listen_port(801).
            set_initialize_client_function(initClient));
  }

  ~TelegraphFixture() {
    server.stop_listening();
    enetpp::global_state::get().deinitialize();
  }
};

/**
 * We can transmit and receive stuff from telegraphs okay.
 */
TEST_F(TelegraphFixture, TransmitReceive) {
  const std::string message{"hello world"};

  client.connect(
      enetpp::client_connect_params()
          .set_channel_count(1)
          .set_server_host_name_and_port("localhost", 801));
}
