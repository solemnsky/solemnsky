/**
 * Splash screen to cover loading resources.
 */
#pragma once
#include <memory>
#include "client/ui/control.h"

namespace ui {

namespace detail {
// TODO: make this threaded so the window doesn't freeze up
class SplashScreen: public Control {
 private:
  const sf::Font &loadFont();
  sf::Font font; // resources aren't loaded yet

  TextFormat loadingText;

  bool screenDrawn{false};
  std::function<std::unique_ptr<Control>()> afterLoading;

 public:
  SplashScreen(std::function<std::unique_ptr<Control>()> afterLoading);

  virtual void tick(float delta) override;
  virtual void render(Frame &f) override;
  virtual bool handle(const sf::Event &event) override;
};
}

}
