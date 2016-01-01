#include <base/control.h>
#include "graphics.h"

namespace sky {

std::string filepathFromId(const SpriteSheetId id) {
  switch (id) {
    case SpriteSheetId::BasicPlayer:
      return
          "media/render-3d/test_1/player_200.png";
  }
}

void SpriteSheet::drawAt(Frame &f, const sf::Vector2f pos,
                         const sf::Vector2f dims, const int index) const {
  // something something
}

}
