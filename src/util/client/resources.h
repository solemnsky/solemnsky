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
 * Metadata and caching for our resources.
 */
#ifndef SOLEMNSKY_RESOURCES_H
#define SOLEMNSKY_RESOURCES_H

#include <string>
#include <vector>
#include <set>
#include <SFML/Graphics.hpp>
#include <util/printer.h>

/****
 * Resource metadata.
 */

enum class ResID {
  Font, // fonts everywhere

  Title, // ui stuff
  MenuBackground,
  Credits,
  Lobby,
  Scoring,
  ScoreOverlay,

  PlayerSheet, // game stuff

  LAST // for checking number of resources defined
};

static const std::set<ResID> splashResources(
    {ResID::Font, ResID::MenuBackground});

enum class ResType {
  Font, Texture
};

struct ResRecord {
  ResRecord() = delete;
  ResRecord(
      std::string path,
      ResType type,
      const bool isSheet,
      const int tileX = 0, const int tileY = 0,
      const int countX = 0, const int countY = 0);

  std::string path;
  ResType type;

  // if it's a spritesheet, we have some metadata about how it's laid out
  bool isSheet;
  int tileX, tileY, countX, countY;

  // get the real usable filesystem path to the resource
  std::string realPath() const;
};

/**
 * Top-level functions to load resources, and then access pre-loaded data and
 * ResRecord metadata from Res values.
 */
void loadSplashResources();
void loadResources();
const ResRecord &recordOf(const ResID res);
const sf::Texture &textureOf(const ResID res);
const sf::Font &fontOf(const ResID res);

namespace detail {

static class ResMan {
 private:
  std::map<int, sf::Texture> textures;
  std::map<int, sf::Font> fonts;
  std::vector<ResRecord> resRecords;

  bool initialized, splashInitialized;

  void loadResource(
      const ResID res, const std::string &progress);

 public:
  ResMan();

  void loadSplashResources();
  void loadResources();

  const sf::Texture &textureOf(ResID res);
  const sf::Font &fontOf(ResID res);
  const ResRecord &recordOf(const ResID res);
} resMan;

}

#endif //SOLEMNSKY_RESOURCES_H
