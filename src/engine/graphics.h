/**
 * Essential rendering helpers / filepaths to resources.
 */
#ifndef SOLEMNSKY_GRAPHICS_H_H
#define SOLEMNSKY_GRAPHICS_H_H

#include "base/util.h"

namespace sky {
enum class SpriteSheetId {
  BasicPlayer
};

std::string filepathFromId(const SpriteSheetId id);

}

#endif //SOLEMNSKY_GRAPHICS_H_H
