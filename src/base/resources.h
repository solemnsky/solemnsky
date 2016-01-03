/**
 * Filepaths / metadata for our resources.
 */
#ifndef SOLEMNSKY_RESOURCES_H
#define SOLEMNSKY_RESOURCES_H

#include <string>
#include <vector>

enum class Res {
  Font,
  PlayerSheet,
  TitleScreen,
  LAST // for checking number of resources defined
};

struct ResRecord {
  std::string path;
  bool isSprite;
  int tileX, tileY, countX, countY;

  ResRecord(std::string path,
            bool isSprite,
            int tileX = 0, int tileY = 0, int countX = 0, int countY = 0) :
      path(path),
      isSprite(isSprite),
      tileX(tileX), tileY(tileY), countX(countX), countY(countY) { }
};

std::string filepathTo(Res res);
ResRecord recordOf(Res res);


#endif //SOLEMNSKY_RESOURCES_H
