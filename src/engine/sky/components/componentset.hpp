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
 * Derivation of AutoNetworked<> for collections of components. Held in Sky.
 */
#pragma once
#include "util/types.hpp"
#include "util/networked.hpp"
#include "component.hpp"

namespace sky {

/**
 * Initializer type for NetworkedMap.
 */
template<typename Data>
using ComponentSetInit = std::map<PID, typename Data::InitType>;

/**
 * Delta type for NetworkedMap.
 */
template<typename Data>
using ComponentSetDelta = std::pair<
    std::map<PID, typename Data::InitType>,
    std::map<PID, optional<typename Data::DeltaType>>>;

template<typename Data>
struct Components;

/**
 * Set of components, with automatic network derivation.
 */
template<typename Data>
// cppcheck-suppress syntaxError
class ComponentSet:
    public AutoNetworked<ComponentSetInit<Data>,
                         ComponentSetDelta<Data>> {
 private:
  // References used to initialize components.
  Physics &physics;

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

  // This is private. Removal should be accomplished through the destroy() flag.
  void remove(const PID pid) {
    auto x = data.find(pid);
    if (x != data.end()) data.erase(x);
  }

 public:
  ComponentSet() = delete;
  ComponentSet(
      const ComponentSetInit<Data> &init, Physics &physics) :
      AutoNetworked<ComponentSetInit<Data>, ComponentSetDelta<Data>>(init),
      physics(physics) {
    for (const auto &x : init) {
      data.emplace(std::piecewise_construct,
                   std::forward_as_tuple(x.first),
                   std::forward_as_tuple(
                       std::piecewise_construct,
                       std::forward_as_tuple(false),
                       std::forward_as_tuple(x.second, physics)));
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

  void put(const typename Data::InitType &init) {
    data.emplace(std::piecewise_construct,
                 std::forward_as_tuple(smallestUnused(data)),
                 std::forward_as_tuple(
                     std::piecewise_construct,
                     std::forward_as_tuple(false),
                     std::forward_as_tuple(init, physics)));
  }

  struct Components<Data> getData();

  void forData(std::function<void(Data &, const PID)> f) {
    for (auto &data: data) {
      f(data.second.second, data.second.first);
    }
  }

  /**
   * Destruction. Remove components destroyable flags set through the base destroy() method.
   */
  void applyDestruction() {
    std::vector<PID> removable;
    forData([&removable](Data &e, const PID pid) {
      if (e.isDestroyable()) removable.push_back(pid);
    });
    for (const PID pid: removable) {
      remove(pid);
    }
  }

  // Networked impl + Args... .

  void applyDelta(const ComponentSetDelta<Data> &delta) {
    const auto &inits = delta.first;
    const auto &deltas = delta.second;

    auto iter = data.begin();
    while (iter != data.end()) {
      const auto entityDelta = deltas.find(iter->first);
      if (entityDelta == deltas.end()) {
        iter->second.second.destroy();
      } 
      ++iter;
    }
    applyDestruction();

    for (const auto &init : inits) {
      data.emplace(std::piecewise_construct,
                   std::forward_as_tuple(init.first),
                   std::forward_as_tuple(
                       std::piecewise_construct,
                       std::forward_as_tuple(false),
                       std::forward_as_tuple(init.second, physics)));
    }
  }

  ComponentSetInit<Data> captureInitializer() const {
    ComponentSetInit<Data> init;
    for (const auto &spanishInquisition: data)
      init.emplace(
          spanishInquisition.first, spanishInquisition.second.second.captureInitializer());
    // You weren't expecting that for sure.
    return init;
  }

  optional<ComponentSetDelta<Data>> collectDelta() {
    ComponentSetDelta<Data> delta;
    bool useful{false};

    // Initializers for uninitialized components.
    for (auto &datum: data) {
      if (!datum.second.first) {
        useful = true;
        delta.first.emplace(datum.first, datum.second.second.captureInitializer());
        datum.second.first = true;
        // This sure reads like plain English.
      }
    }

    // Deltas for all.
    for (auto &datum: data) {
      const auto elemDelta = datum.second.second.collectDelta();
      useful |= bool(elemDelta);
      delta.second.emplace(datum.first, elemDelta);
    }

    if (useful) return delta;
    else return {};
  }

};

template<typename Data>
struct ComponentIterator {
  using MapType = std::map<PID, std::pair<bool, Data>>;
  MapType &map;
  typename MapType::iterator it;

  ComponentIterator(MapType &map, typename MapType::iterator it) :
      map(map), it(it) { }

  ComponentIterator &operator++() {
    ++it;
    return *this;
  }

  bool operator==(const ComponentIterator &rhs) const { return it == rhs.it; }
  bool operator!=(const ComponentIterator &rhs) const { return it != rhs; }
  Data &operator*() const { return it->second.second; }

};

/**
 * Iterator handle to the data of a ComponentSet.
 */
template<typename Data>
struct Components {
 private:
  using MapType = std::map<PID, std::pair<bool, Data>>;
  MapType &map;

 public:
  Components(MapType &map) : map(map) { }

  ComponentIterator<Data> begin() { return ComponentIterator<Data>(map, map.begin()); }
  ComponentIterator<Data> end() { return ComponentIterator<Data>(map, map.end()); }

  size_t size() {
    return map.size();
  }

};

template<typename Data>
Components<Data> ComponentSet<Data>::getData() {
  return Components<Data>(data);
}

}
