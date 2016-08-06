#include "multimedia.hpp"

namespace sky {

/**
 * VisualEntity.
 */

VisualEntity::VisualEntity(const bool isDefault, const PID index) :
    isDefault(true), index(0) { }


FillStyle FillStyle::WithColor(const sf::Color &fillColor) {
  FillStyle result;
  result.fillColor = fillColor;
  return result;
}

FillStyle FillStyle::WithTexture(const VisualEntity &entity, const TextureType type, const float alpha) {
  FillStyle result;
  result.fillEntity = entity;
  result.textureAlpha = alpha;
  result.textureType = type;
  return result;
}

}
