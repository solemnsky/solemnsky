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
#include "util/clientutil.h"
#include "resources.h"
#include "util/methods.h"
#include "util/printer.h"

namespace ui {


/**
 * TextureMetadata.
 */

TextureMetadata::TextureMetadata(
    const std::string &url,
    const std::string &name,
    const optional<SheetLayout> &sheetForm) :
    url(url),
    name(name),
    spritesheetForm(sheetForm) { }

TextureMetadata TextureMetadata::TextureResource(
    const std::string &url,
    const std::string &name) {
  return TextureMetadata(url, name);
}

TextureMetadata TextureMetadata::SpritesheetResource(
    const std::string &url,
    const std::string &name,
    const sf::Vector2i &spriteDimensions,
    const sf::Vector2i &sheetTiling) {
  return TextureMetadata(url, name, SheetLayout(spriteDimensions, sheetTiling));
}

/**
 * FontMetadata.
 */

FontMetadata::FontMetadata(const std::string &url, const std::string &name) :
    url(url), name(name) { }

namespace detail {

/**
 * Hard-coded meatdata for resources.
 */
const std::map<FontID, FontMetadata> fontMetadata{
    {FontID::Default,
     FontMetadata("fonts/Roboto-Light.ttf", "Roboto")}};
const std::map<TextureID, TextureMetadata> textureMetadata{
    {TextureID::Title,
     TextureMetadata::TextureResource("render-2d/title.png",
                                      "title screen")},
    {TextureID::MenuBackground,
     TextureMetadata::TextureResource("render-2d/menubackground.png",
                                      "menu background")},
    {TextureID::Credits,
     TextureMetadata::TextureResource("render-2d/credits.png",
                                      "credits")},
    {TextureID::Lobby,
     TextureMetadata::TextureResource("render-2d/lobby.png",
                                      "lobby")},
    {TextureID::Scoring,
     TextureMetadata::TextureResource("render-2d/scoring.png",
                                      "scoring")},
};

}

/**
 * AppResources.
 */

AppResources::AppResources(
    const std::map<FontID, sf::Font> &fonts,
    const std::map<TextureID, sf::Texture> &textures) :
    fonts(fonts),
    textures(textures),
    defaultFont(getFont(FontID::Default)) { }

const FontMetadata &AppResources::getFontData(const FontID id) const {
  return detail::fontMetadata.at(id);
}

const TextureMetadata &AppResources::getTextureData(const TextureID id) const {
  return detail::textureMetadata.at(id);
}

const sf::Font &AppResources::getFont(const FontID id) const {
  return fonts.at(id);
}
const sf::Texture &AppResources::getTexture(const TextureID id) const {
  return textures.at(id);
}

/**
 * ResourceLoader.
 */

ResourceLoader::ResourceLoader(
    std::initializer_list<FontID> bootstrapFonts,
    std::initializer_list<TextureID> bootstrapTextures) :
    loadingProgress(0) {
  // TODO: move resource metadata checks to compile-time?
  for (const auto font : bootstrapFonts) {
    assert(detail::fontMetadata.find(font)
               != detail::fontMetadata.end());
    if (auto error = loadFont(font, detail::fontMetadata.at(font)))
      appLog("Error loading bootstrap font: " + error.get(), LogOrigin::App);
  }

  for (const auto texture : bootstrapTextures) {
    assert(detail::textureMetadata.find(texture)
               != detail::textureMetadata.end());
    if (auto error = loadTexture(texture, detail::textureMetadata.at(texture)))
      appLog("Error loading bootstrap texture: " + error.get(), LogOrigin::App);
  }
  appLog("Loaded bootstrap resources.", LogOrigin::App);
}

ResourceLoader::~ResourceLoader() {
  workingThread.join();
}

const sf::Font &ResourceLoader::accessFont(const FontID id) {
  return fonts.at(id);
}

const sf::Texture &ResourceLoader::accessTexture(const TextureID id) {
  return textures.at(id);
}

void ResourceLoader::writeLog(const std::string &str) {
  logMutex.lock();
  workerLog.push_back(str);
  logMutex.unlock();
}

optional<std::string> ResourceLoader::loadTexture(
    const TextureID id,
    const TextureMetadata &metadata) {
  sf::Texture texture;
  if (texture.loadFromFile(pathFromResourceUrl(metadata.url))) {
    textures.emplace(id, std::move(texture));
    return {};
  } else {
    return std::string("Texture did not load correctly.");
    // TODO: get more information about failure to load
  }
}

optional<std::string> ResourceLoader::loadFont(
    const FontID id,
    const FontMetadata &metadata) {
  sf::Font font;
  if (font.loadFromFile(pathFromResourceUrl(metadata.url))) {
    fonts.emplace(id, std::move(font));
    return {};
  } else {
    return std::string("Font did not load correctly.");
    // TODO: get more information about failure to load
  }
}

void ResourceLoader::loadAllThreaded() {
  workingThread = std::thread([&]() {
    const size_t totalWork{
        detail::fontMetadata.size() + detail::textureMetadata.size()};
    size_t progress{0};

    for (const auto font : detail::fontMetadata) {
      writeLog("Loading font: " + font.second.url
                   + "(" + font.second.name + ")");
      if (auto error = loadFont(font.first, font.second)) {
        writeLog("Error loading font: " + error.get());
        return;
      }
      ++progress;
      loadingProgress = float(progress) / float(totalWork);
    }

    for (const auto texture : detail::textureMetadata) {
      writeLog("Loading texture: " + texture.second.url
                   + "(" + texture.second.name + ")");
      if (auto error = loadTexture(texture.first, texture.second)) {
        writeLog("Error loading texture: " + error.get());
        return;
      }
      ++progress;
      loadingProgress = float(progress) / float(totalWork);
    }

    holder.emplace(fonts, textures);
  });
}

float ResourceLoader::getProgress() const {
  return 0;
}

void ResourceLoader::printNewLogs(Printer &p) {
  logMutex.lock();
  for (const auto msg : workerLog) {
    p.printLn(msg);
  }
  workerLog.clear();
  logMutex.unlock();
}

bool ResourceLoader::getErrorStatus() const {
  return loadingErrored;
}

AppResources const *ResourceLoader::getHolder() const {
  if (holder) return holder.get_ptr();
  else return nullptr;
}

}
