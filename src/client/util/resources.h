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
