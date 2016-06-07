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
 * Hard-coded resource management for global assets.
 */
#pragma once

#include <SFML/Graphics.hpp>
#include "util/methods.h"
#include "util/printer.h"
#include "util/threads.h"

namespace ui {

/**
 * Description of the way sprites are distributed on a regular orthogonal
 * spritesheet.
 */
struct SheetLayout {
  SheetLayout(const sf::Vector2i &spriteDimensions,
              const sf::Vector2i &sheetTiling);

  const sf::Vector2i spriteDims, tiling;

};

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

/**
 * Resource IDs.
 */
enum class FontID {
  Default, // Font used for all text and titles
  LAST
};

enum class TextureID {
  Title,
  MenuBackground,
  Credits,
  Lobby,
  Scoring,
  ScoreOverlay,
  PlayerSheet,
  LAST
};

namespace detail {

/**
 * Hard-coded metadata for resources.
 */
extern const std::map<FontID, FontMetadata> fontMetadata;
extern const std::map<TextureID, TextureMetadata> textureMetadata;

}

/**
 * Access to the results of a successful resource loading.
 */
class AppResources {
 private:
  const std::map<FontID, sf::Font> &fonts;
  const std::map<TextureID, sf::Texture> &textures;

 public:
  AppResources(const std::map<FontID, sf::Font> &fonts,
                 const std::map<TextureID, sf::Texture> &textures);

  // Accessing data.
  const FontMetadata &getFontData(const FontID id) const;
  const TextureMetadata &getTextureData(const TextureID id) const;
  const sf::Font &getFont(const FontID id) const;
  const sf::Texture &getTexture(const TextureID id) const;

  // Resource handles.
  const sf::Font &defaultFont;

};

/**
 * Manages the loading of a set of resources, resulting in a AppResources.
 */
class ResourceLoader {
 private:
  std::map<FontID, sf::Font> fonts;
  std::map<TextureID, sf::Texture> textures;

  std::thread workingThread;
  std::vector<std::string> workerLog;
  std::mutex logMutex;

  float loadingProgress;
  bool loadingErrored;
  optional<AppResources> holder;

  // Loading subroutines.
  void writeLog(const std::string &str);
  optional<std::string> loadFont(
      const FontID id, const FontMetadata &data);
  optional<std::string> loadTexture(
      const TextureID id, const TextureMetadata &data);

 public:
  ResourceLoader(
      std::initializer_list<FontID> bootstrapFonts,
      std::initializer_list<TextureID> boostrapTextures);
  ~ResourceLoader();

  // Accessing boostrapped resources.
  const sf::Font &accessFont(const FontID id);
  const sf::Texture &accessTexture(const TextureID id);

  // Loading resources.
  void loadAllThreaded(); // non-blocking
  float getProgress() const;
  void printNewLogs(Printer &p);
  bool getErrorStatus() const;

  AppResources const *getHolder() const;

};

}

