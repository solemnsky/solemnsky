/**
 * Splash screen, a cover for loading our resources (resources.h).
 * This runs implicitly on runSFML.
 */
#ifndef SOLEMNSKY_SPLASH_H
#define SOLEMNSKY_SPLASH_H

#include "ui/control.h"
#include <memory>
#include "base/resources.h"

namespace ui {

namespace detail {
// TODO: make this threaded so the window doesn't freeze up
class SplashScreen : public Control {
private:
  sf::Font font; // we have to manage this because we draw
  // before resources come online

  bool screenDrawn{false};
  std::function<std::unique_ptr<Control>()> afterLoading;

public:
  SplashScreen(std::function<std::unique_ptr<Control>()> afterLoading) :
      afterLoading(afterLoading) {
    font.loadFromFile(filepathTo(Res::Font));
  }

  virtual void tick(float delta) override;
  virtual void render(Frame &f) override;
  virtual void handle(const sf::Event &event) override;
};
}

}

#endif //SOLEMNSKY_SPLASH_H
