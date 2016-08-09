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
 * Derivation of AutoNetworked<>
 */
#include "util/types.hpp"
#include "util/networked.hpp"
#pragma once

/**
 * Initializer type for NetworkedMap.
 */
template<typename Init>
using NetMapInit = std::map<PID, Init>;

/**
 * Delta type for NetworkedMap.
 */
template<typename Init, typename Delta>
using NetMapDelta = std::pair<std::map<PID, Init>, std::map<PID, optional<Delta>>>;

template<typename Data>
struct NetMapData;

/**
 * A PID-indexed map of entities that we want to synchronize over the protocol.
 */
template<typename Data, // underlying entity type
    typename Init, // initializer type
    typename Delta, // delta type
    typename... Args> // arguments to supply to the Data ctor along with Init
class NetMap: public AutoNetworked<NetMapInit<Init>, NetMapDelta<Init, Delta>> {
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
  // The bool tracks whether we've sent initializers through the delta collection yet. 'false' means we haven't.

 public:
  NetMap(const NetMapInit<Init> &init, Args... args,
         typename std::enable_if<!std::is_base_of<
             AutoNetworked<Init, Delta>, Data>::value>::type * = 0) {
    for (const auto &x : init) {
      data.emplace(std::piecewise_construct,
                   std::forward_as_tuple(x.first),
                   std::forward_as_tuple(
                       std::piecewise_construct,
                       std::forward_as_tuple(false),
                       std::forward_as_tuple(x.second, args...)));
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

  // TODO: Substitute the nauseating linear search for something more efficient iff this becomes a bottleneck.
  void put(const Init &init, Args... args) {
    data.emplace(std::piecewise_construct,
                 std::forward_as_tuple(smallestUnused(data)),
                 std::forward_as_tuple(
                     std::piecewise_construct,
                     std::forward_as_tuple(false),
                     std::forward_as_tuple(init, args...)));
  }

  struct NetMapData<Data> getData();

  void forData(std::function<void(Data &, const PID)> f) {
    for (auto &data: data) {
      f(data.second.second, data.second.first);
    }
  }

  // Networked impl + Args... .

  void applyDelta(const NetMapDelta<Init, Delta> &delta, Args... args) {
    const auto &inits = delta.first;
    const auto &deltas = delta.second;

    auto iter = data.begin();
    while (iter != data.end()) {
      const auto entityDelta = deltas.find(iter->first);
      if (entityDelta == deltas.end()) {
        const auto toErase = iter;
        data.erase(toErase);
      } else {
        iter->second.second.applyDelta(entityDelta->second);
      }
      ++iter;
    }

    for (const auto &init : inits) {
      data.emplace(std::piecewise_construct,
                   std::forward_as_tuple(init.first),
                   std::forward_as_tuple(
                       std::piecewise_construct,
                       std::forward_as_tuple(false),
                       std::forward_as_tuple(init.second, args...)));
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

  optional<NetMapDelta<Init, Delta>> collectDelta() {
    NetMapDelta<Init, Delta> delta;
    bool necessary;

    // Initializers for uninitialized components.
    for (auto &datum: data) {
      if (!datum.second.first) {
        necessary = true;
        delta.first.emplace(datum.first, datum.second.second.captureInitializer());
        datum.second.first = true;
        // This sure reads like plain English.
      }
    }

    // Deltas for all.
    for (auto &datum: data) {
      const auto elemDelta = datum.second.second.collectDelta();
      necessary |= bool(elemDelta);
      delta.second.emplace(datum.first, elemDelta);
    }

    if (necessary) return delta;
    else return {};
  }

};

template<typename Data>
struct NetMapIterator {
  using MapType = std::map<PID, std::pair<bool, Data>>;
  MapType &map;
  typename MapType::iterator it;

  NetMapIterator(MapType &map, typename MapType::iterator it) :
      map(map), it(it) { }

  NetMapIterator &operator++() {
    ++it;
    return *this;
  }

  bool operator==(const NetMapIterator &rhs) const { return it == rhs.it; }
  bool operator!=(const NetMapIterator &rhs) const { return it != rhs; }
  Data &operator*() const { return it->second.second; }

};

/**
 * Iterator handle to the data of a NetMap.
 */
template<typename Data>
struct NetMapData {
 private:
  using MapType = std::map<PID, std::pair<bool, Data>>;
  MapType &map;

 public:
  NetMapData(MapType &map) : map(map) { }

  NetMapIterator<Data> begin() { return NetMapIterator<Data>(map, map.begin()); }
  NetMapIterator<Data> end() { return NetMapIterator<Data>(map, map.end()); }

  size_t size() {
    return map.size();
  }

};

template<typename Data, typename Init, typename Delta, typename... Args>
NetMapData<Data> NetMap<Data, Init, Delta, Args...>::getData() {
  return NetMapData<Data>(data);
}
