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
using NetMapDelta = std::pair<std::map<PID, Init>, std::map<PID, Delta>>;

/**
 * A PID-indexed map of thingies that we want to synchronize over the network while mutating both the collection
 * and the thingy-states in themselves.
 */
template<typename Data, // underlying data ("thingy")
    typename Init, // initializer type for data
    typename Delta, // delta type for data
    typename... Args> // argument to supply to the Data ctor along with Init
class NetMap {
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

  std::map<PID, std::pair<bool, Data>> data;
  // the bool tracks whether we've sent initializers. 'false' means we haven't.

 public:
  NetMap(const NetMapInit<Init> &init, Args... args) {
    for (const auto &x : init) {
      data.emplace(std::piecewise_construct,
                   std::forward_as_tuple(x.first),
                   std::forward_as_tuple(false, std::forward_as_tuple(x.second, args...)));
    }
  }

  /**
   * Operations on data.
   */

  Data *get(const PID pid) {
    auto x = data.find(pid);
    if (x != data.end()) return &x->second.second;
    return nullptr;
  }

  void remove(const PID pid) {
    auto x = data.find(pid);
    if (x != data.end()) data.erase(x);
  }

  // Put something in and tell us where it ended up.
  // TODO: Substitute this viscerally nauseating linear search for something more clever iff this becomes a bottleneck.
  std::pair<PID, Data &> put(const Init &init, Args... args) {
    data.emplace(std::piecewise_construct,
                 std::forward_as_tuple(smallestUnused(data)),
                 std::forward_as_tuple(init, args...));
  }

  // Iterate over the data. C++ has a terrible way of dealing with iterators and we only ever need forward
  // iteration that runs to completion in our usages.
 void forData(std::function<void(Data &, const PID)> f) {
    for (auto &datum: data) {
      f(datum.second.second, datum.first);
    }
  }

  // Networked impl + Args... .

  void applyDelta(const NetMapDelta<Init, Delta> &delta, Args... args) {
    const auto &inits = delta.first; // Thingy inits.
    const auto &deltas = delta.second; // Thingy deltas.

    auto iter = data.begin();
    while (iter != data.end()) {
      if (deltas.find(iter->first) == deltas.end()) {
        const auto toErase = iter;
        ++iter;
        data.erase(toErase);
      } else ++iter;
    }
    for (const auto &init : inits) {
      data.emplace(std::piecewise_construct,
                   std::forward_as_tuple(init.first),
                   std::forward_as_tuple(init.second, args...));
    }
  }

  NetMapInit<Init> captureInitializer() const {
    NetMapInit<Init> init;
    for (const auto &spanishInquisition: data)
      init.emplace(
          spanishInquisition.first, spanishInquisition.second.second.captureInitializer());
    // You weren't expecting that for sure.
    return init;
  }

  NetMapDelta<Init, Delta> collectDelta() {
    NetMapDelta<Init, Delta> delta;

    // Initializers for uninitialized components.
    for (auto &datum: data) {
      if (!datum.second.first) {
        delta.first.emplace(datum.first, datum.second.second.captureInitializer());
        datum.second.first = true;
        // This sure reads like plain English.
      }
    }

    // Deltas for all.
    for (const auto &datum: data) {
      delta.second.emplace(datum.first, datum.second.second.collectDelta());
    }

    return delta;
  }

};
