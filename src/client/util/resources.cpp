#include <assert.h>
#include "resources.h"
#include "util/methods.h"

const static std::vector<ResRecord> resRecords{ // edit this side-by-side with resources.h
    {"fonts/Atarian/SF Atarian System.ttf",
        ResType::Font,    false},

    {"render-2d/title.png",
        ResType::Texture, false},
    {"render-2d/pageselector.png",
        ResType::Texture, false},
    {"render-2d/homeicon.png",
        ResType::Texture, false},
    {"render-2d/settingsicon.png",
        ResType::Texture, false},
    {"render-2d/listingicon.png",
        ResType::Texture, false},
    {"render-2d/menubackground.png",
        ResType::Texture, false},

    {"render-3d/test_1/player_200.png",
        ResType::Texture, true, 200, 200, 2, 15}

};

static bool asserted{false}; // make sure we have enough entries in data
#define ASSERT if (!asserted) { assert(resRecords.size() == (unsigned long long int) Res::LAST); asserted = true; }
// TODO: move this check to compile-time? >_>

const std::string filepathTo(const Res res) {
  ASSERT;
  const ResRecord &record = recordOf(res);
  return "../../media/" + record.path;
}

const ResRecord &recordOf(const Res res) {
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
        appLog(LogType::Info, "Loading font " + record.path + progress);
        sf::Font font;
        font.loadFromFile(filepathTo(res));
        fonts.emplace((int) res, std::move(font));
        break;
      }
      case ResType::Texture: {
        appLog(LogType::Info, "Loading texture " + record.path + progress);

        sf::Texture texture;
        texture.loadFromFile(filepathTo(res));
        textures.emplace((int) res, std::move(texture));
      }
    }
  }

  appLog(LogType::Info,
         std::to_string(fonts.size()) + " total fonts available.");
  appLog(LogType::Info,
         std::to_string(textures.size()) + " total textures available.");
  appLog(LogType::Notice, "Finished loading resources.");

  initialized = true;
}

const sf::Texture &ResMan::recallTexture(Res res) {
  assert(initialized);
  const ResRecord &record(recordOf(res));
  if (record.type == ResType::Texture)
    return textures.at((int) res);
  else {
    appErrorLogic(record.path + " is not a texture.");
  }
}

const sf::Font &ResMan::recallFont(Res res) {
  assert(initialized);
  const ResRecord &record(recordOf(res));
  if (record.type == ResType::Font)
    return fonts.at((int) res);
  else {
    appErrorLogic(record.path + " is not a font");
  }
}

}

void loadResources() { detail::resMan.loadRes(); }

const sf::Texture &textureOf(Res res) {
  return detail::resMan.recallTexture(res);
}

const sf::Font &fontOf(Res res) { return detail::resMan.recallFont(res); }