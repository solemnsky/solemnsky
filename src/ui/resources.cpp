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
     TextureMetadata::TextureResource("render-2d/title.png", "title screen")}};

}

/**
 * ResourceHolder.
 */

ResourceHolder::ResourceHolder(
    const std::map<FontID, sf::Font> &fonts,
    const std::map<TextureID, sf::Texture> &textures) :
    fonts(fonts), textures(textures) { }

const FontMetadata &ResourceHolder::getFontData(const FontID id) const {
  return detail::fontMetadata.at(id);
}

const TextureMetadata &ResourceHolder::getTextureData(const TextureID id) const {
  return detail::textureMetadata.at(id);
}

const sf::Font &ResourceHolder::getFont(const FontID id) const {
  return fonts.at(id);
}
const sf::Texture &ResourceHolder::getTexture(const TextureID id) const {
  return textures.at(id);
}

/**
 * ResourceLoader.
 */

ResourceLoader::ResourceLoader() :
    loadingProgress(0) { }

void ResourceLoader::writeLog(const std::string &str) {
  logMutex.lock();
  workerLog.push_back(str);
  logMutex.unlock;
}

optional<std::string> ResourceLoader::loadTexture(
    const TextureID id,
    const TextureMetadata &metadata) {
  sf::Texture texture;
  if (texture.loadFromFile(pathFromResourceUrl(metadata.url))) {
    textures.emplace(id, std::move(texture));
    return {};
  } else {
    return {"Texture did not load correctly."};
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
    return {"Font did not load correctly."};
  }
}

void ResourceLoader::loadBoostrap(std::initializer_list<FontID> fonts,
                                  std::initializer_list<TextureID> textures) {
  for (const auto font : fonts) {
    if (auto error = loadFont(font, detail::fontMetadata.at(font)))
      appLog("Error loading bootstrap font: " + error.get(), LogOrigin::App);
  }

  for (const auto texture : textures) {
    if (auto error = loadTexture(texture, detail::textureMetadata.at(texture)))
      appLog("Error loading bootstrap texture: " + error.get(), LogOrigin::App);
  }
}

void ResourceLoader::loadAllThreaded() {
  workingThread = std::thread([&]() {
    const size_t totalWork{
        detail::fontMetadata.size() + detail::textureMetadata.size()};
    size_t progress{0};

    for (const auto font : detail::fontMetadata) {
      if (auto error = loadFont(font.first, font.second)) {
        writeLog("Error loading font: " + error.get());
        return;
      }
      ++progress;
      loadingProgress = float(progress) / float(totalWork);
    }

    for (const auto texture : detail::textureMetadata) {
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

ResourceHolder const *ResourceLoader::getHolder() const {
  if (holder) return holder.get_ptr();
  else return nullptr;
}

