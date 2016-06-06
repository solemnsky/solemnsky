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
#include "util/printer.h"
#include "sheet.h"

namespace ui {

/**
 * Metadata for a particular resource entity.
 */
struct TextureMetadata {
 private:
  TextureMetadata(
      const std::string &url,
      const std::string &name,
      const optional<SheetLayout> &spritesheetForm = {});

 public:
  const std::string url, name;
  optional<SheetLayout> spritesheetForm;

  // Constructors.
  static TextureMetadata TextureResource(
      const std::string &url,
      const std::string &name);
  static TextureMetadata SpritesheetResource(
      const std::string &url,
      const std::string &name,
      const sf::Vector2i &spriteDimensions,
      const sf::Vector2i &sheetTiling);
  static TextureMetadata FontResource(
      const std::string &url,
      const std::string &name);

};

namespace detail {

/**
 * Non-templated delegate of ResourceLoader.
 */
struct ResourceSet {
  optional<std::string> loadResource(
      const size_t id, const TextureMetadata &metadata,
      Printer &printer);

  std::map<size_t, sf::Texture> loadedTextures;
  std::map<size_t, sf::Font> loadedFonts;

};

}

/**
 * An access to the data associated with a set of resources.
 */
template<typename FontID, typename TextureID>
class ResourceHolder {
  friend class ResourceLoder;
 private:
  const detail::ResourceSet &resourceSet;
  const std::map<ResID, TextureMetadata> &resourceData;

  ResourceHolder(const detail::ResourceSet &resourceSet,
                 const std::map<ResID, TextureMetadata> &resourceData) :
      resourceSet(resourceSet), resourceData(resourceData) { }

 public:
  const TextureMetadata &getMetadata(const ResID id) const {
    return resourceData.at(id);
  }

  const sf::Font &getFont(const ResID id) const {
    return resourceSet.loadedFonts.at(id);
  }

  const sf::Texture &getTexture(const ResID id) const {
    return resourceSet.loadedTextures.at(id);
  }

};

/**
 * Manages the loading of a set of resources, resulting in a ResourceHolder.
 */
template<typename FontID, typename TextureId>
class ResourceLoader {
 public:
  ResourceLoader(const std::map<ResID, TextureMetadata>);

  void load();
  ResourceHolder const *getHolder() const;

};

}

