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
/**
 * Small templated std::map wrapper to manage the various maps in the engine that need to propogate
 * the Networked synchronization properties of their elements.
 */
#include "util/types.hpp"
#pragma once

/**
 * Initializer type for NetworkedMap. I enjoy what one might call 'explicit' types.
 */
template<typename Init>
using NetMapInit = std::map<PID, Init>;

/**
 * Delta type for NetworkedMap.
 */
template<typename Init, typename Delta>
using NetMapDelta = std::pair<std::map<PID, Init> &, std::map<PID, Delta> &>;

/**
 * A PID-indexed map of thingies that we want to synchronize over the network while mutating both the collection
 * and the thingy-states in themselves.
 */
template<typename Data, typename Init, typename Delta>
struct NetMap: public Networked<NetMapInit<Init>, NetMapDelta<Init, Delta>> {
 private:
  // Also defined in util/methods.hpp, redefined here because including that header here would
  // likely affect compile times. In a *reasonable* language we could abstract away this template.
  template<typename T>
  static PID smallestUnused(const std::map<PID, T> &map) {
    PID i{0};
    for (const auto &pair: map) {
      if (pair.first == i) ++i;
      else break;
    }
    return i;
  }

 public:
  NetMap(const Init &init) : Networked(init) {

  }

  std::map<PID, Data> data;

  // Get one of them; alternatively, give us a fast and ergonomic way to segfault.
  Data *getData(const PID pid) {
    auto x = data.find(pid);
    if (x != data.end()) return &x->second;
    return nullptr;
  }

  // Put something in and tell us where it ended up.
  // TODO: Substitute this viscerally nauseating linear search for something more clever iff this becomes a bottleneck.
  template<typename... Args>
  std::pair<PID, Data &> putData(Args... args) {
    data.emplace(std::piecewise_construct,
                 std::forward_as_tuple(smallestUnused(data), args...));
  }

  // Networked impl:




};
