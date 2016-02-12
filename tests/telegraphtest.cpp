#include "telegraph/telegraph.h"
#include "util/methods.h"
#include "gtest/gtest.h"

class TelegraphFixture: public testing::Test {
 public:
  tg::UsageFlag flag;
};

// poll two hosts until the first of the pair returns an event
ENetEvent processHosts(tg::Host &host1, tg::Host &host2) {
  ENetEvent event;
  for (; ;) {
    event = host1.poll();
    host2.poll();
    if (event.type != 0) break;
  }
  return event;
}

/**
 * We can transmit and receive stuff from telegraphs okay.
 */

TEST_F(TelegraphFixture, TransmitReceive) {
  tg::Host server(tg::HostType::Server, 4242);
  tg::Host client(tg::HostType::Client);
  ENetEvent event;
  ENetPeer *serverPeer;
  ENetPeer *clientPeer;

  serverPeer = client.connect("localhost", 4242);
  event = processHosts(client, server);
  EXPECT_EQ(event.type, ENET_EVENT_TYPE_CONNECT); // client gets connect event
  event = processHosts(server, client);
  EXPECT_EQ(event.type, ENET_EVENT_TYPE_CONNECT); // server gets connect event
  clientPeer = event.peer;

  appLog("connected!");
  // **we're connected and all synced up**

  const tg::Pack<std::string> stringPack = tg::StringPack();
  tg::Telegraph<std::string, std::string> telegraph(stringPack, stringPack);

  telegraph.transmit(client, serverPeer, "hey there");
  event = processHosts(server, client);
  EXPECT_EQ(event.type, ENET_EVENT_TYPE_RECEIVE); // server receives packet
  EXPECT_EQ(telegraph.receive(event.packet), "hey there");

  telegraph.transmit(server, clientPeer, "hey, I got your message");
  event = processHosts(client, server);
  EXPECT_EQ(event.type, ENET_EVENT_TYPE_RECEIVE); // client receives packet
  EXPECT_EQ(telegraph.receive(event.packet), "hey, I got your message");
}
