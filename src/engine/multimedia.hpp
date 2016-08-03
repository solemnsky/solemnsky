/**
 * Handles for multimedia resources that need to be referenced by the engine.
 */
#pragma once
#include "util/types.hpp"
#include <SFML/Graphics/Color.hpp>

namespace sky {

/**
 * Reference to a graphics entity, either from the default resources or from
 * an environment visual component.
 */
struct VisualEntity {
  VisualEntity(const bool isDefault, const PID index);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(isDefault, index);
  }

  bool isDefault; // If it's in the default resources table -- otherwise, it's in the Visuals component.
  PID index;

};

/**
 * A way to fill a region of something.
 * We can use a solid color, a single visual entity as a texture, or perhaps
 * an animation? Keyframes, anybody?
 * TODO: more features.
 */
struct FillStyle {
 private:
  FillStyle() = default;

 public:
  optional<sf::Color> fillColor;
  optional<VisualEntity> fillEntity;
  float textureAlpha; // The alpha to render the entity with.

  enum class TextureType {
    Center, // Center it on the thing you want to draw.
    Tile // Tile it seamlessly.
  } textureType; // How to draw the texture.

  // Cereal serialization.
  template<typename Archive>
  void serialize(Archive &ar) {
    ar(fillColor, fillEntity, textureAlpha, textureType);
  }

  // Constructors.
  static FillStyle WithColor(const sf::Color &fillColor);
  static FillStyle WithTexture(const VisualEntity &entity,
                               const TextureType = TextureType::Center,
                               const float alpha = 1);

};

}
