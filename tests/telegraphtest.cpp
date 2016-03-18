#include "util/telegraph.h"
#include "util/methods.h"
#include "sky/protocol.h"
#include "gtest/gtest.h"

/**
 * 'Telegraph' and 'Host', using the Packet / Pack system and the enet library.
 */
class TelegraphTest: public testing::Test {
 public:
  tg::UsageFlag flag;

  tg::Host server, client;
  tg::Telegraph<sky::ServerPacket> clientTelegraph;
  tg::Telegraph<sky::ClientPacket> serverTelegraph;
  ENetPeer *serverPeer, *clientPeer;
  ENetEvent event;

  TelegraphTest() :
      server(tg::HostType::Server, 4242),
      client(tg::HostType::Client) {

    serverPeer = client.connect("localhost", 4242);
    event = processHosts(client, server);
    EXPECT_EQ(event.type, ENET_EVENT_TYPE_CONNECT); // client gets connect event
    event = processHosts(server, client);
    EXPECT_EQ(event.type, ENET_EVENT_TYPE_CONNECT); // server gets connect event
    clientPeer = event.peer;

    // now we're connected
  }

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
};

/**
 * We can connect a client and a server and send packets correctly.
 */
TEST_F(TelegraphTest, TransmitReceive) {
  tg::Telegraph<std::string> telegraph;

  telegraph.transmit(client, serverPeer, std::string("hey there"));
  event = processHosts(server, client);
  EXPECT_EQ(event.type, ENET_EVENT_TYPE_RECEIVE); // server receives packet
  EXPECT_EQ(*telegraph.receive(event.packet), "hey there");

  telegraph.transmit(server, clientPeer,
                     std::string("hey, I got your message ;D"));
  event = processHosts(client, server);
  EXPECT_EQ(event.type, ENET_EVENT_TYPE_RECEIVE); // client receives packet
  EXPECT_EQ(*telegraph.receive(event.packet), "hey, I got your message ;D");
}

/**
 * Sanity check that protocol verbs can be transmitted over the network.
 */
TEST_F(TelegraphTest, Protocol) {
  using namespace sky;

  {
    clientTelegraph.transmit(
        client, serverPeer, ClientPacket::ReqJoin("nickname"));
    event = processHosts(server, client);
    const optional<ClientPacket> &packet =
        serverTelegraph.receive(event.packet);

    ASSERT_TRUE((bool) packet);
    EXPECT_EQ(packet->type, ClientPacket::Type::ReqJoin);
    EXPECT_EQ(*packet->stringData, "nickname");
  }

  {
    serverTelegraph.transmit(
        server, clientPeer,
        ServerPacket::Message(ServerMessage::Broadcast("some broadcast")));
    event = processHosts(client, server);
    const optional<ServerPacket> &packet =
        clientTelegraph.receive(event.packet);

    ASSERT_TRUE((bool) packet);
    EXPECT_EQ(packet->type, ServerPacket::Type::Message);
    ASSERT_TRUE((bool) packet->message);
    EXPECT_EQ(packet->message->contents, "some broadcast");
  }

  {
    clientTelegraph.transmit(
        client, serverPeer, ClientPacket::ReqSpawn());
    event = processHosts(server, client);
    const optional<ClientPacket> &packet =
        serverTelegraph.receive(event.packet);

    ASSERT_TRUE((bool) packet);
    EXPECT_EQ(packet->type, ClientPacket::Type::ReqSpawn);
  }
}

/**
 * Protocol packets with invalid structure aren't accepted by the telegraph.
 */
TEST_F(TelegraphTest, Invariant) {
  using namespace sky;

  {
    sky::ClientPacket malformedPacket = sky::ClientPacket::Chat("asdf");
    malformedPacket.stringData.reset(); // necessary field
    clientTelegraph.transmit(client, serverPeer, malformedPacket);
    event = processHosts(server, client);
    const optional<ServerPacket> &packet =
        clientTelegraph.receive(event.packet);
    EXPECT_FALSE((bool) packet);
  }
}
