#include <assert.h>
#include "resources.h"
#include "util/methods.h"

/**
 * ResRecord.
 */

ResRecord::ResRecord(
    std::string path,
    ResType type,
    bool isSheet,
    int tileX, int tileY,
    int countX, int countY)
    : path(path),
      type(type),
      isSheet(isSheet),
      tileX(tileX), tileY(tileY), countX(countX), countY(countY) { }

std::string ResRecord::realPath() const {
  return "../../media/" + path;
}

/**
 * Top-level functions.
 */

void loadResources() {
  detail::resMan.loadResources();
}

const ResRecord &recordOf(const Res res) {
  return detail::resMan.recordOf(res);
}

const sf::Texture &textureOf(const Res res) {
  return detail::resMan.textureOf(res);
}

const sf::Font &fontOf(const Res res) {
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
         ResType::Texture, true, 200, 200, 2, 15}
    } {
  assert(resRecords.size() == (size_t) Res::LAST);
}

const ResRecord &ResMan::recordOf(const Res res) {
  return resRecords.at((size_t) res);
}

void ResMan::loadResources() {
  if (initialized) return;

  std::string resCount = std::to_string((int) Res::LAST);
  std::string progress;
  appLog("Loading resources ...", LogOrigin::App);

  for (Res res = Res::Font; res < Res::LAST;
       res = (Res) (((int) res) + 1)) {
    const ResRecord &record(recordOf(res));
    progress = " ... (" + std::to_string((int) res + 1) +
        " of " + resCount + ")";

    switch (record.type) {
      case ResType::Font: {
        appLog("Loading font " + record.path + progress, LogOrigin::App);
        sf::Font font;
        font.loadFromFile(record.realPath());
        fonts.emplace((int) res, std::move(font));
        break;
      }
      case ResType::Texture: {
        appLog("Loading texture " + record.path + progress,
               LogOrigin::App);

        sf::Texture texture;
        texture.loadFromFile(record.realPath());
        textures.emplace((int) res, std::move(texture));
      }
    }
  }

  appLog(std::to_string(fonts.size()) + " total fonts available.",
         LogOrigin::App);
  appLog(std::to_string(textures.size()) + " total textures available.",
         LogOrigin::App);
  appLog("Finished loading resources.", LogOrigin::App);

  initialized = true;
}

const sf::Texture &ResMan::textureOf(Res res) {
  assert(initialized);
  const ResRecord &record(recordOf(res));
  if (record.type == ResType::Texture) return textures.at((int) res);
  appErrorLogic(record.path + " is not a texture.");
}

const sf::Font &ResMan::fontOf(Res res) {
  assert(initialized);
  const ResRecord &record(recordOf(res));
  if (record.type == ResType::Font) return fonts.at((int) res);
  appErrorLogic(record.path + " is not a font.");
}
}
