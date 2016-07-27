/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "client/elements/style.hpp"
#include "sandbox.hpp"

/**
 * SandboxCommand.
 */

SandboxCommand::SandboxCommand(const Type type) :
    type(type) { }

optional<SandboxCommand> SandboxCommand::parseCommand(
    const std::string &input) {
  // Split the command by whitespace
  std::stringstream tmp(input);
  std::vector<std::string> command{std::istream_iterator<std::string>{tmp},
                                   std::istream_iterator<std::string>{}};
  if (command.empty()) return {};

  if (command[0] == "start" and (command.size() == 2)) {
    SandboxCommand parsed{Type::Start};
    parsed.mapName.emplace(command[1]);
    return parsed;
  }

  if (command[0] == "stop" and (command.size() == 1)) {
    return SandboxCommand{Type::Stop};
  }

  if (command[0] == "tuning" and (command.size() == 2 or (command.size() == 3))) {
    if (command[1] == "default") {
      return SandboxCommand{Type::DefaultTuning};
    }

    SandboxCommand parsed{Type::Tune};
    parsed.tuningParam.emplace(command[1]);

    // Potentially include the tuning value to set.
    if (command.size() > 2) {
      if (const auto tuningValue = readFloat(command[2])) {
        parsed.tuningValue = tuningValue;
      } else return {};
    }

    return parsed;
  }

  if (command[0] == "dump" and command.size() == 1) {
    return SandboxCommand(Type::DumpTuning);
  }

  return {};
}

/**
 * SandboxLogger.
 */

void SandboxLogger::onEvent(const sky::ArenaEvent &event) {
  event.print(sandbox.enginePrinter);
  sandbox.enginePrinter.breakLine();
}

SandboxLogger::SandboxLogger(sky::Arena &arena, Sandbox &sandbox) :
    sky::ArenaLogger(arena), sandbox(sandbox) { }

/**
 * Sandbox.
 */

void Sandbox::startHandle() {
  stopHandle();
  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Game));
  status = "loading...";
  skyHandle.start();
}

void Sandbox::stopHandle() {
  arena.applyDelta(sky::ArenaDelta::Mode(sky::ArenaMode::Lobby));
  status = "stopped";
  skyRender.reset();
  skyHandle.stop();
}

void Sandbox::runCommand(const SandboxCommand &command) {
  switch (command.type) {
    case SandboxCommand::Type::Start: {
      arena.applyDelta(sky::ArenaDelta::EnvChange(
          command.mapName.get()));
      startHandle();
      return;
    }
    case SandboxCommand::Type::Stop: {
      stopHandle();
      return;
    }
    case SandboxCommand::Type::Tune: {
      const auto &param = command.tuningParam.get();
      if (float *paramPtr = spawnTuning.accessParamByName(param)) {
        if (const auto &value = command.tuningValue) {
          *paramPtr = value.get();
          consolePrinter.output(
              "Setting " + inQuotes(param) + " to " + printFloat(value.get()));
        } else {
          consolePrinter.output(
              inQuotes(param) + " = " + printFloat(*paramPtr));
        }
      } else {
        consolePrinter.output("Unknown tuning parameter: " + inQuotes(param));
      }
      return;
    }
    case SandboxCommand::Type::DefaultTuning: {
      consolePrinter.output("Resetting tuning to default.");
      spawnTuning = {};
      return;
    }
    case SandboxCommand::Type::DumpTuning: {
      consolePrinter.output("Dumping custom tuning values to log.");
      appLog("\n" + spawnTuning.toString());
      return;
    }
  }

  throw enum_error();
}

void Sandbox::displayStatus(ui::Frame &f, const std::string &status) {
  f.drawText({500, 500}, status, sf::Color::White,
             style.base.normalText, resources.defaultFont);
}

Sandbox::Sandbox(ClientShared &state) :
    Game(state, "sandbox"),
    arena(sky::ArenaInit("sandbox", "ball_funnelpark")),
    skyHandle(arena, sky::SkyHandleInit()),
    debugView(arena, skyHandle),
    logger(arena, *this),
    messageInteraction(references),
    enginePrinter(messageInteraction),
    clientPrinter(messageInteraction),
    consolePrinter(messageInteraction) {
  arena.connectPlayer("offline player");
  player = arena.getPlayer(0);
  stopHandle();
  areChildren({&messageInteraction});
}

void Sandbox::onChangeSettings(const ui::SettingsDelta &settings) {
  ClientComponent::onChangeSettings(settings);
  if (skyRender) skyRender->onChangeSettings(settings);
  if (settings.nickname) {
    sky::PlayerDelta delta;
    delta.nickname = *settings.nickname;
    arena.applyDelta(sky::ArenaDelta::Delta(0, delta));
  }
}

void Sandbox::onBlur() {

}

void Sandbox::onFocus() {

}

void Sandbox::doExit() {
  quitting = true;
}

void Sandbox::tick(const TimeDiff delta) {
  if (!skyHandle.getSky()) {
    if (auto environment = skyHandle.getEnvironment()) {
      if (environment->getMap() and environment->getVisuals()) {
        // Ready to load!
        status = "loaded";
        skyHandle.instantiateSky({});
        skyRender.emplace(shared, resources, arena, *skyHandle.getSky());
      } else {
        // Waiting for loading to happen.
        if (!environment->loadingErrored() and environment->loadingIdle()) {
          environment->loadMore(true, false);
        }
      }
    }
  }

  if (shared.getUi().gameFocused()) arena.tick(delta);

  ui::Control::tick(delta);
}

void Sandbox::render(ui::Frame &f) {
  if (auto sky = skyHandle.getSky()) {
    const auto &plane = sky->getParticipation(*player).plane;
    skyRender->render(
        f, plane ?
           plane->getState().physical.pos :
           sf::Vector2f(0, 0));
  } else {
    if (const auto environment = skyHandle.getEnvironment()) {
      if (environment->loadingErrored()) {
        displayStatus(f, "Environment loading errored!");
      } else {
        displayStatus(f, "Loading environment...");
      }
    } else {
      displayStatus(f, "No environment loaded.");
    }
  }

  ui::Control::render(f);
}

bool Sandbox::handle(const sf::Event &event) {
  if (ui::Control::handle(event)) return true;

  for (const auto &action : shared.findClientActions(event)) {
    if (messageInteraction.handleClientAction(action.first, action.second)) return true;

    if (auto sky = skyHandle.getSky()) {
      const auto &participation = sky->getParticipation(*player);

      if (action.first == ui::ClientAction::Spawn
          and action.second
          and !participation.isSpawned()) {
        const auto spawnPoint = sky->getMap().pickSpawnPoint(sky::Team::Spectator);
        player->spawn(spawnTuning, spawnPoint.pos, spawnPoint.angle);
        return true;
      }
    }
  }

  for (const auto &action : shared.findSkyActions(event)) {
    player->doAction(action.first, action.second);
  }

  return false;
}

void Sandbox::reset() {
  ui::Control::reset();
}

void Sandbox::signalRead() {
  ui::Control::signalRead();
  if (const auto messageInput = messageInteraction.inputSignal) {
    const auto input = messageInput.get();
    consolePrinter.input(input);

    auto parsed = SandboxCommand::parseCommand(input);

    if (parsed) {
      runCommand(parsed.get());
    } else {
      consolePrinter.output("Invalid sandbox command!");
    }
  }
}

void Sandbox::signalClear() {
  ui::Control::signalClear();
}

void Sandbox::printDebugLeft(Printer &p) {
  debugView.printArenaReport(p);
}

void Sandbox::printDebugRight(Printer &p) {
  debugView.printSkyReport(p);
}
