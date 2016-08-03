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
#include "types.hpp"

namespace sky {

void forSkyActions(std::function<void(const Action)> fn) {
  for (sky::Action action = sky::Action(0);
       action < sky::Action::MAX;
       action = sky::Action(size_t(action) + 1)) {
    fn(action);
  }
}

static const std::vector<std::string> actionNames = {
    "thrust",
    "reverse",
    "left",
    "right",
    "primary",
    "secondary",
    "special",
    "suicide"
};

std::string showAction(const Action action) {
  return actionNames.at((size_t) action);
}

optional<Action> readAction(const std::string &str) {
  Action action = Action::Thrust;
  for (const auto &search : actionNames) {
    if (search == str) return action;
    action = Action(char(action) + 1);
  }
  return {};
}

}
