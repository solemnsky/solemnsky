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
      arena(sky::ArenaInit("special arena", "NULL", sky::ArenaMode::Lobby)),
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

  ASSERT_EQ(remoteSky.getSettings().gravity, -1);

  sky.changeSettings(sky::SkySettingsDelta::ChangeView(2));
  remoteSky.applyDelta(sky.collectDelta());

  ASSERT_EQ(remoteSky.getSettings().viewScale, 2);

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
    sky::ParticipationInput input;
    sky::PlaneStateClient stateInput;
    stateInput.physical = sky::PhysicalState({300, 300}, {}, 50, 0);
    input.planeState.emplace(stateInput);

    sky::PlaneControls controls(participation.getControls());
    controls.doAction(sky::Action::Left, true);
    input.controls = controls;

    participation.applyInput(input);

    ASSERT_EQ(participation.plane->getState().physical.pos.x, 300);
    ASSERT_EQ(participation.plane->getState().physical.rot, 50);
    ASSERT_EQ(participation.getControls().getState<sky::Action::Left>(), true);
  }

  // We can collect inputs from Participations.
  {
    sky::Arena remoteArena(arena.captureInitializer());
    sky::Sky remoteSky(remoteArena, nullMap, sky.captureInitializer());
    sky::Player &remotePlayer = *remoteArena.getPlayer(0);
    remotePlayer.doAction(sky::Action::Right, true);

    optional<sky::ParticipationInput> input =
        remoteSky.getParticipation(remotePlayer).collectInput();
    ASSERT_EQ(bool(input), true);
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

  sky::Arena remoteArena{arena.captureInitializer()};
  sky::Sky remoteSky{remoteArena, nullMap, sky.captureInitializer()};
  auto &remotePlayer = *remoteArena.getPlayer(0);
  auto &remoteParticip = remoteSky.getParticipation(remotePlayer);

  // Spawn state is of the server's authority.
  {
    ASSERT_EQ(remoteParticip.isSpawned(), false);

    player.spawn({}, {200, 200}, 0);
    auto delta = sky.collectDelta();
    remoteSky.applyDelta(delta.respectAuthority(player));

    ASSERT_EQ(remoteParticip.isSpawned(), true);
  }

  // Position state is of the client's authority.
  {
    ASSERT_EQ(remoteParticip.plane->getState().physical.pos.x, 200);

    sky::ParticipationInput input;
    sky::PlaneStateClient stateInput(participation.plane->getState());
    stateInput.physical = sky::PhysicalState({300, 300}, {}, 50, 0);
    input.planeState.emplace(stateInput);

    auto delta = sky.collectDelta();
    remoteSky.applyDelta(delta.respectAuthority(player));

    ASSERT_EQ(remoteParticip.plane->getState().physical.pos.x, 200);
  }

}

/**
 * Props can be spawned by Participations, and are Networked correctly
 */
TEST_F(SkyTest, PropTest) {
  arena.connectPlayer("nameless plane");
  auto &player = *arena.getPlayer(0);
  auto &participation = sky.getParticipation(player);

  sky.getParticipation(player).spawnProp(sky::PropInit());
  ASSERT_EQ(participation.props.size(), size_t(1));

  sky::Arena remoteArena{arena.captureInitializer()};
  sky::Sky remoteSky{remoteArena, nullMap, sky.captureInitializer()};
  auto &remotePlayer = *remoteArena.getPlayer(0);
  auto &remoteParticipation = remoteSky.getParticipation(remotePlayer);

  ASSERT_EQ(remoteParticipation.props.size(), size_t(1));

  participation.spawnProp(sky::PropInit());
  remoteSky.applyDelta(sky.collectDelta());

  ASSERT_EQ(remoteParticipation.props.size(), size_t(2));

}

/**
 * Tuning values can be accessed by name, for use in the rcon and sanbox.
 */
TEST_F(SkyTest, TuningTest) {
  auto tuning = sky::PlaneTuning();
  *tuning.accessParamByName("flight.threshold") = 42.0f;
  ASSERT_EQ(tuning.flight.threshold, 42.0f);
}
