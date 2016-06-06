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
    const optional<SheetLayout> &spriteSheetForm) :
    url(url),
    name(name),
    spritesheetForm(spritesheetForm) { }


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
 * ResourceSet
 */

optional<std::string> ResourceSet::loadTexture(
    const size_t id,
    const TextureMetadata &metadata) {
  sf::Texture texture;
  if (texture.loadFromFile(pathFromResourceUrl(metadata.url))) {
    textures.emplace(id, std::move(texture));
    return {};
  } else {
    return {"Texture did not load correctly."};
  }
}

optional<std::string> ResourceSet::loadFont(
    const size_t id,
    const FontMetadata &metadata) {
  sf::Font font;
  if (font.loadFromFile(pathFromResourceUrl(metadata.url))) {
    fonts.emplace(id, std::move(font));
    return {};
  } else {
    return {"Font did not load correctly."};
  }

}

}

}
