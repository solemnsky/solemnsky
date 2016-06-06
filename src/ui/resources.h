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
 * Resource management for global assets.
 */
#pragma once

#include <SFML/Graphics.hpp>
#include "util/methods.h"
#include "util/printer.h"
#include "sheet.h"
#include "util/threads.h"

namespace ui {

/**
 * Metadata for a font.
 */
struct FontMetadata {
  // Constructors.
  FontMetadata(const std::string &url, const std::string &name);

  // Data.
  const std::string url, name;

};

/**
 * Metadata for a texture / spritesheet.
 */
struct TextureMetadata {
 private:
  TextureMetadata(
      const std::string &url,
      const std::string &name,
      const optional<SheetLayout> &spritesheetForm = {});

 public:
  // Constructors.
  static TextureMetadata TextureResource(
      const std::string &url,
      const std::string &name);
  static TextureMetadata SpritesheetResource(
      const std::string &url,
      const std::string &name,
      const sf::Vector2i &spriteDimensions,
      const sf::Vector2i &sheetTiling);

  // Data.
  const std::string url, name;
  optional<SheetLayout> spritesheetForm;

};

namespace detail {

/**
 * Non-templated delegate of ResourceLoader.
 */
struct ResourceSet {
  optional<std::string> loadTexture(
      const size_t id, const TextureMetadata &metadata);
  optional<std::string> loadFont(
      const size_t id, const FontMetadata &metadata);

  std::map<size_t, sf::Texture> textures;
  std::map<size_t, sf::Font> fonts;

};

}

/**
 * An access to the data associated with a set of resources.
 */
template<typename FontID, typename TextureID>
class ResourceHolder {
  friend class ResourceLoder;
 private:
  const std::map<FontID, FontMetadata> &fontData;
  const std::map<TextureID, TextureMetadata> &textureData;
  const detail::ResourceSet &resourceSet;

  ResourceHolder(const detail::ResourceSet &resourceSet,
                 const std::map<FontID, FontMetadata> &fontData,
                 const std::map<TextureID, TextureMetadata> &textureData) :
      resourceSet(resourceSet), fontData(fontData),
      textureData(textureData) { }

 public:
  const FontMetadata &getFontData(const FontID id) const {
    return fontData.at(id);
  }

  const TextureMetadata &getTextureData(const TextureID id) const {
    return textureData.at(id);
  }

  const sf::Font &getFont(const FontID id) const {
    return resourceSet.fonts.at(id);
  }

  const sf::Texture &getTexture(const TextureID id) const {
    return resourceSet.textures.at(id);
  }

};

/**
 * Manages the loading of a set of resources, resulting in a ResourceHolder.
 */
template<typename FontID, typename TextureID>
class ResourceLoader {
 private:
  const std::map<FontID, FontMetadata> fontMetadata;
  const std::map<TextureID, TextureMetadata> textureMetadata;

  detail::ResourceSet resourceSet;
  float loadingProgress;
  optional<ResourceHolder> holder;

 public:
  ResourceLoader(const std::map<FontID, FontMetadata> fontMetadata,
                 const std::map<TextureID, TextureMetadata>) :
      fontMetadata(fontMetadata), textureMetadata(textureMetadata),
      loadingProgress(0) { }

  // Execute loading in a new thread.
  bool load() {
    const std::string fontCount = std::to_string(fontMetadata.size());
    for (const auto font : fontMetadata) {
      if (auto error = resourceSet.loadFont(
          size_t(font.first), font.second)) {
        return false;
      }
    }

    const std::string textureCount = std::to_string(fontMetadata.size());
    for (const auto texture : textureMetadata) {
      if (auto error = resourceSet.loadTexture(
          size_t(texture.first), texture.second)) {
      }
    }

    holder.emplace(resourceSet, fontMetadata, textureMetadata);
    return true;
  }

  float getProgress() const {
    return 0;
  }

  void printNewLogs(Printer &p) {

  }

  ResourceHolder const *getHolder() const {
    if (holder) return holder.get_ptr();
    else return nullptr;
  }

};

}

