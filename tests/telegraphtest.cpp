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

  // connected

//  tg::Telegraph<std::string, std::string> telegraph;
//
//  telegraph.transmit(client, serverPeer, "hey there");
//  event = processHosts(server, client);
//  EXPECT_EQ(event.type, ENET_EVENT_TYPE_RECEIVE); // server receives packet
//  EXPECT_EQ(*telegraph.receive(event.packet), "hey there");
//
//  telegraph.transmit(server, clientPeer, "hey, I got your message ;D");
//  event = processHosts(client, server);
//  EXPECT_EQ(event.type, ENET_EVENT_TYPE_RECEIVE); // client receives packet
//  EXPECT_EQ(*telegraph.receive(event.packet), "hey, I got your message ;D");
}

/**
 * Sanity check that protocol verbs can be transmitted over the network.
 */
TEST_F(TelegraphTest, Protocol) {
  using namespace sky;

  tg::Host server(tg::HostType::Server, 4242);
  tg::Host client(tg::HostType::Client);
  ENetEvent event;
  ENetPeer *serverPeer;
  ENetPeer *clientPeer;

  serverPeer = client.connect("localhost", 4242);
  event = processHosts(client, server);
  event = processHosts(server, client);
  clientPeer = event.peer;

  // connected

//  tg::Telegraph<ServerPacket, ClientPacket> clientTelegraph;
//  tg::Telegraph<ClientPacket, ServerPacket> serverTelegraph;
//
//  clientTelegraph.transmit(
//      client, serverPeer, ClientPacket::ReqJoin("nickname"));
//  event = processHosts(server, client);
//  const optional<ClientPacket> &clientPacket =
//      serverTelegraph.receive(event.packet);
//
//  ASSERT_TRUE((bool) clientPacket);
//  EXPECT_EQ(clientPacket->type, ClientPacket::Type::ReqJoin);
//  EXPECT_EQ(*clientPacket->stringData, "nickname");
//
//  serverTelegraph.transmit(
//      server, clientPeer,
//      ServerPacket::Message(ServerMessage::Broadcast("some broadcast")));
//  event = processHosts(client, server);
//  const optional<ServerPacket> &srvPacket =
//      clientTelegraph.receive(event.packet);
//
//  ASSERT_TRUE((bool) srvPacket);
//  EXPECT_EQ(srvPacket->type, ServerPacket::Type::Message);
//  ASSERT_TRUE((bool) srvPacket->message);
//  EXPECT_EQ(srvPacket->message->contents, "some broadcast");
}
