/**
 * Metadata and caching for our resources.
 */
#ifndef SOLEMNSKY_RESOURCES_H
#define SOLEMNSKY_RESOURCES_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

/****
 * Resource metadata.
 */

enum class Res {
  Font,
  PlayerSheet,
  TitleScreen,
  LAST // for checking number of resources defined
};

enum class ResType {
  Font, Texture
};

struct ResRecord {
  std::string path;
  ResType type;

  // if it's a spritesheet, we have some metadata about how it's laid out
  bool isSheet;
  int tileX, tileY, countX, countY;

  ResRecord(std::string path,
            ResType type,
            bool isSheet,
            int tileX = 0, int tileY = 0, int countX = 0, int countY = 0) :
      path(path),
      type(type),
      isSheet(isSheet),
      tileX(tileX), tileY(tileY), countX(countX), countY(countY) { }
};

std::string filepathTo(Res res);
ResRecord recordOf(Res res);

/****
 * Resource caching.
 */

namespace detail {

static class ResMan {
private:
  std::map<int, sf::Texture> textures;
  std::map<int, sf::Font> fonts;

public:
  void loadRes();

  sf::Texture recallTexture(Res res);
  sf::Font recallFont(Res res);
} resMan;

}

inline void loadResources() { detail::resMan.loadRes(); }

inline sf::Texture textureFrom(Res res) {
  return detail::resMan.recallTexture(res);
}

inline sf::Font fontFrom(Res res) { return detail::resMan.recallFont(res); }

#endif //SOLEMNSKY_RESOURCES_H
