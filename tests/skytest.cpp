#include <gtest/gtest.h>
#include "engine/sky/sky.hpp"

/**
 * The Sky subsystem operates and networks correctly.
 */
class SkyTest: public testing::Test {
 public:
  sky::Arena arena;
  sky::Map nullMap;
  sky::Sky sky;

  SkyTest() :
      arena(sky::ArenaInit("special arena", "NULL", sky::ArenaMode::Lobby), {}, true),
      nullMap(),
      sky(arena, nullMap, sky::SkyInit(), nullptr) { }

};

/**
 * SkySettings are correctly copied and transmitted over the network.
 */
TEST_F(SkyTest, SettingsTest) {
  sky.changeSettings(sky::SkySettingsDelta::ChangeGravity(-1));

  sky::Arena remoteArena(arena.captureInitializer());
  sky::Sky remoteSky(remoteArena, nullMap, sky.captureInitializer());

  ASSERT_EQ(remoteSky.settings.getGravity(), -1);

  sky.changeSettings(sky::SkySettingsDelta::ChangeView(2));
  const auto delta = sky.collectDelta();
  ASSERT_TRUE(bool(delta));
  remoteSky.applyDelta(*delta);
  ASSERT_EQ(remoteSky.settings.getViewscale(), 2);
  ASSERT_FALSE(bool(sky.collectDelta()));

}

/**
 * Spawns and kills are transmitted over the network.
 */
TEST_F(SkyTest, SpawnTest) {
  // Join and spawn a player.
  arena.connectPlayer("nameless plane");
  auto &player = *arena.getPlayer(0);
  player.spawn({}, {}, 0);

  // Make a client arena for this player.
  sky::Arena remoteArena(arena.captureInitializer(), 0, true);
  sky::Sky remoteSky(remoteArena, nullMap, sky.captureInitializer());
  sky::Participation &remoteParticip =
      remoteSky.getParticipation(*remoteArena.getPlayer(0));

  // The player is spawned on the client.
  ASSERT_TRUE(remoteParticip.isSpawned());

  {
    player.doAction(sky::Action::Suicide, true);
    arena.tick(0.1f); // The suicide is simulated here.
    const auto delta = sky.collectDelta();
    ASSERT_TRUE(delta);
    remoteSky.applyDelta(*delta);
    ASSERT_FALSE(sky.collectDelta());
  }

  // The player is no longer spawned on the client.
  ASSERT_FALSE(remoteParticip.isSpawned());

  // We spawn them again.
  {
    player.spawn({}, {}, 0);
    const auto delta = sky.collectDelta();
    ASSERT_TRUE(delta);
    remoteSky.applyDelta(*delta);
  }

  // The player is once again spawned on the client.
  ASSERT_TRUE(remoteParticip.isSpawned());

}

/**
 * ParticipationInput allows players limited authority over a remote game state.
 */
TEST_F(SkyTest, InputTest) {
  arena.connectPlayer("nameless plane");
  auto &player = *arena.getPlayer(0);
  auto &participation = sky.getParticipation(player);

  player.spawn({}, {200, 200}, 0);

  // We can modify position and control state.
  {
    // We construct a ParticipationInput.
    sky::ParticipationInput input;
    sky::PlaneStateClient stateInput;
    stateInput.physical = sky::PhysicalState({300, 300}, {}, 50, 0);
    input.planeState.emplace(stateInput);

    sky::PlaneControls controls(participation.getControls());
    controls.doAction(sky::Action::Left, true);
    input.controls = controls;

    // Now apply the input to a participation.
    participation.applyInput(input);

    // The results are visible.
    ASSERT_EQ(participation.plane->getState().physical.pos.x, 300);
    ASSERT_EQ(participation.plane->getState().physical.rot, 50);
    ASSERT_EQ(participation.getControls().getState<sky::Action::Left>(), true);
  }

  // We can collect inputs from Participations.
  {
    // We construct a remote Sky responsible for the client with PID 0.
    sky::Arena remoteArena(arena.captureInitializer(), 0);
    sky::Sky remoteSky(remoteArena, nullMap, sky.captureInitializer());
    sky::Player &remotePlayer = *remoteArena.getPlayer(0);

    // Execute an action on the remote sky.
    remotePlayer.doAction(sky::Action::Right, true);

    // Collect the input.
    optional<sky::ParticipationInput> input =
        remoteSky.getParticipation(remotePlayer).collectInput();
    ASSERT_EQ(bool(input), true);

    // Apply the input to our original participation and see the results.
    participation.applyInput(input.get());
    ASSERT_EQ(participation.getControls().getState<sky::Action::Right>(), true);
  }

}

/**
 * SkyDeltas can be rewritten to respect the authority of a client.
 */
TEST_F(SkyTest, AuthorityTest) {
  arena.connectPlayer("nameless plane");
  auto &player = *arena.getPlayer(0);
  auto &participation = sky.getParticipation(player);

  // Create a remote sky to function as a client.
  sky::Arena remoteArena{arena.captureInitializer(), 0};
  sky::Sky remoteSky{remoteArena, nullMap, sky.captureInitializer()};
  auto &remotePlayer = *remoteArena.getPlayer(0);
  auto &remoteParticip = remoteSky.getParticipation(remotePlayer);

  // Spawn state is of the server's authority.
  {
    ASSERT_EQ(remoteParticip.isSpawned(), false);

    // Spawn the player on the server.
    player.spawn({}, {200, 200}, 0);

    // Transmit a delta to the remote, respecting the player's authority.
    auto delta = sky.collectDelta();
    ASSERT_TRUE(bool(delta));
    remoteSky.applyDelta(delta->respectAuthority(player));

    // The player is spawned on the remote sky.
    ASSERT_EQ(remoteParticip.isSpawned(), true);
  }

  // Position state is of the client's authority.
  {
    // Check a value on the client.
    ASSERT_EQ(remoteParticip.plane->getState().physical.pos.x, 200);

    // Modify the server-side Participation through a ParticipationInput.
    // Normally these are used to send Participation state from the client to the server.
    sky::ParticipationInput input;
    sky::PlaneStateClient stateInput(participation.plane->getState());
    stateInput.physical = sky::PhysicalState({300, 300}, {}, 50, 0);
    input.planeState.emplace(stateInput);
    sky.getParticipation(player).applyInput(input);

    // Transmit a delta to the client, respecting his authority.
    auto delta = sky.collectDelta();
    ASSERT_TRUE(bool(delta));
    remoteSky.applyDelta(delta->respectAuthority(player));

    // Nothing has changed, on the client.
    ASSERT_TRUE(bool(remoteParticip.plane));
    ASSERT_EQ(remoteParticip.plane->getState().physical.pos.x, 200);
  }

}

/**
 * Entities can be created and synchronized over the network.
 */
TEST_F(SkyTest, EntityTest) {
  // If there are any components (home bases) spawned on the map, this would not be null.
  ASSERT_FALSE(sky.collectDelta());

  // We create an entity locally.
  ASSERT_EQ(sky.getEntities().size(), 0);
  sky.spawnEntity(sky::EntityState(
      {}, {}, sky::Shape::Circle(1),
      sf::Vector2f(200, 200), sf::Vector2f(0, 0)));
  ASSERT_EQ(sky.getEntities().size(), 1);

  // The entity is copied to a client through the initialization protocol.
  sky::Arena remoteArena{arena.captureInitializer(), 0};
  sky::Sky remoteSky{remoteArena, nullMap, sky.captureInitializer()};
  ASSERT_EQ(remoteSky.getEntities().size(), 1);

  // We can spawn more entities, and they are synchronized through the delta protocol.
  {
    sky.spawnEntity(sky::EntityState(
        {}, {}, sky::Shape::Circle(1),
        sf::Vector2f(200, 200), sf::Vector2f(0, 0)));
    const auto delta = sky.collectDelta();
    ASSERT_TRUE(bool(delta));
    remoteSky.applyDelta(delta.get());
    ASSERT_EQ(sky.getEntities().size(), 2);
    ASSERT_EQ(remoteSky.getEntities().size(), 2);
  }

  // We can mark entities for deletion, and they are deleted on the next tick.
  auto &entity = *sky.getEntities().begin();
  entity.destroy();
  arena.tick(20);
  ASSERT_EQ(sky.getEntities().size(), 1);

  // The deletions are also transmitted over SkyDeltas.
  {
    const auto delta = sky.collectDelta();
    ASSERT_TRUE(bool(delta));
    remoteSky.applyDelta(delta.get());
    ASSERT_EQ(remoteSky.getEntities().size(), 1);
  }

}

/**
 * Tuning values can be accessed by name, for use in the rcon and sanbox.
 */
TEST_F(SkyTest, TuningTest) {
  auto tuning = sky::PlaneTuning();
  *tuning.accessParamByName("flight.threshold") = 42.0f;
  ASSERT_EQ(tuning.flight.threshold, 42.0f);
}
