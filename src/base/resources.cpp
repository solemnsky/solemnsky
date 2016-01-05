#include <assert.h>
#include "resources.h"
#include "util.h"

const static std::vector<ResRecord> resRecords{ // edit this side-by-side with resources.h
    {"fonts/arial.ttf",
        ResType::Font,    false},
    {"render-3d/test_1/player_200.png",
        ResType::Texture, true, 200, 200, 2, 15},
    {"render-2d/placeholder/title.png",
        ResType::Texture, false}
};

static bool asserted{false}; // make sure we have enough entries in data
#define ASSERT if (!asserted) { assert(resRecords.size() == (unsigned long long int) Res::LAST); asserted = true; }
// TODO: move this check to compile-time? >_>

std::string filepathTo(const Res res) {
  ASSERT;
  const ResRecord &record = recordOf(res);
  return "../../media/" + record.path;
}

ResRecord recordOf(const Res res) {
  ASSERT;
  return resRecords[(unsigned long long int) res];
}

/****
 * ResMan
 */

namespace detail {

void ResMan::loadRes() {
  if (initialized) return;

  std::string resCount = std::to_string((int) Res::LAST);
  std::string progress;
  appLog(LogType::Notice, "Loading resources ...");

  for (Res res = Res::Font; res < Res::LAST; res = (Res) (((int) res) + 1)) {
    const ResRecord &record(recordOf(res));
    progress = " ... (" + std::to_string((int) res + 1) +
               " of " + resCount + ")";

    switch (record.type) {
      case ResType::Font: {
        appLog(LogType::Notice, "Loading font " + record.path + progress);
        sf::Font font;
        font.loadFromFile(filepathTo(res));
        fonts.emplace(std::pair<int, sf::Font>((int) res, font));
        break;
      }
      case ResType::Texture: {
        appLog(LogType::Notice, "Loading texture " + record.path + progress);

        sf::Texture texture;
        texture.loadFromFile(filepathTo(res));
        textures.emplace(std::pair<int, sf::Texture>((int) res, texture));
      }
    }
  }

  appLog(LogType::Info, "Finished loading resources!");
  initialized = true;
}

const sf::Texture &ResMan::recallTexture(Res res) const {
  const ResRecord &record(recordOf(res));
  if (record.type == ResType::Texture)
    return textures.at((int) res);
  else {
    appLog(LogType::Error, record.path + " is not a texture.");
    assert(false); // no need for exceptions, this is practically a syntax error
  }
}

const sf::Font &ResMan::recallFont(Res res) const {
  const ResRecord &record(recordOf(res));
  if (record.type == ResType::Font)
    return fonts.at((int) res);
  else {
    appLog(LogType::Error, record.path + " is not a font");
    assert(false);
  }
}

}
