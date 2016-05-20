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
#include <assert.h>
#include "resources.h"
#include "util/methods.h"
#include "util/printer.h"

/**
 * ResRecord.
 */

ResRecord::ResRecord(
    std::string path,
    ResType type,
    const bool isSheet,
    const int tileX, const int tileY,
    const int countX, const int countY) :
    path(path),
    type(type),
    isSheet(isSheet),
    tileX(tileX), tileY(tileY), countX(countX), countY(countY) { }

std::string ResRecord::realPath() const {
  return rootPath() + "media/" + path;
}

/**
 * Top-level functions.
 */

void loadSplashResources() {
  detail::resMan.loadSplashResources();
}

void loadResources() {
  detail::resMan.loadResources();
}

const ResRecord &recordOf(const ResID res) {
  return detail::resMan.recordOf(res);
}

const sf::Texture &textureOf(const ResID res) {
  return detail::resMan.textureOf(res);
}

const sf::Font &fontOf(const ResID res) {
  return detail::resMan.fontOf(res);
}

/**
 * detail::ResMan.
 */
namespace detail {
ResMan::ResMan() :
    resRecords{ // edit this side-by-side with resources.h
        {"fonts/Atarian/SF Atarian System.ttf", ResType::Font, false},

        {"render-2d/title.png", ResType::Texture, false},
        {"render-2d/menubackground.png", ResType::Texture, false},
        {"render-2d/credits.png", ResType::Texture, false},
        {"render-2d/lobby.png", ResType::Texture, false},
        {"render-2d/scoring.png", ResType::Texture, false},
        {"render-2d/scoreoverlay.png", ResType::Texture, false},

        {"render-3d/test_1/player_200.png",
         ResType::Texture, true, 200, 200, 2, 15}},
    initialized(false), splashInitialized(false) {
  assert(resRecords.size() == (size_t) ResID::LAST);
}

const ResRecord &ResMan::recordOf(const ResID res) {
  return resRecords.at((size_t) res);
}

void ResMan::loadResource(const ResID res, const std::string &progress) {
  const ResRecord &record(recordOf(res));

  switch (record.type) {
    case ResType::Font: {
      appLog("Loading font " + record.path + progress, LogOrigin::App);

      sf::Font font;
      font.loadFromFile(record.realPath());
      fonts.emplace((int) res, std::move(font));
      break;
    }
    case ResType::Texture: {
      appLog("Loading texture " + record.path + progress, LogOrigin::App);

      sf::Texture texture;
      texture.loadFromFile(record.realPath());
      textures.emplace((int) res, std::move(texture));
    }
  }
}

void ResMan::loadSplashResources() {
  if (splashInitialized) return;
  for (ResID res : splashResources) loadResource(res, " (pre-loading)");
  splashInitialized = true;
}

void ResMan::loadResources() {
  if (initialized) return;

  std::string resCount = std::to_string((int) ResID::LAST);

  for (ResID res = ResID::Font; res < ResID::LAST;
       res = (ResID) (int(res) + 1)) {
    if (splashResources.find(res) != splashResources.end()) continue;
    loadResource(res, " ... (" + std::to_string((int) res + 1) +
        " of " + resCount + ")");
  }

  appLog(std::to_string(fonts.size()) + " total fonts available.",
         LogOrigin::App);
  appLog(std::to_string(textures.size()) + " total textures available.",
         LogOrigin::App);
  appLog("Finished loading resources.", LogOrigin::App);

  initialized = true;
}

const sf::Texture &ResMan::textureOf(ResID res) {
  auto texture = textures.find(int(res));
  if (texture != textures.end()) {
    return texture->second;
  } else throw std::runtime_error("Accessing texture that isn't loaded yet!");
}

const sf::Font &ResMan::fontOf(ResID res) {
  auto font = fonts.find(int(res));
  if (font != fonts.end()) {
    return font->second;
  } else throw std::runtime_error("Accessing font that isn't loaded yet!");
}

}
