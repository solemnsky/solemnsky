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
      appLog("Loading texture " + record.path + progress,
             LogOrigin::App);

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
  appLog("Loading resources ...", LogOrigin::App);

  for (ResID res = ResID::Font; res < ResID::LAST;
       res = (ResID) (int(res) + 1)) {
    if (splashResources.find(res) != splashResources.end()) continue;
    loadResource(res,
                 " ... (" + std::to_string((int) res + 1) +
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
  } else appErrorRuntime("Accessing texture that isn't loaded yet!");
}

const sf::Font &ResMan::fontOf(ResID res) {
  auto font = fonts.find(int(res));
  if (font != fonts.end()) {
    return font->second;
  } else appErrorRuntime("Accessing font that isn't loaded yet!");
}

}
