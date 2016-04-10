/**
 * Splash screen to cover loading resources.
 */
#pragma once
#include <memory>
#include "control.h"

namespace ui {

namespace detail {
// TODO: make this threaded so the window doesn't freeze up
class SplashScreen: public Control {
 private:
  TextFormat loadingText;

  bool screenDrawn{false};
  std::function<std::unique_ptr<Control>(AppState &)> afterLoading;

 public:
  SplashScreen(AppState &appState,
               std::function<std::unique_ptr<Control>(AppState &)>
               afterLoading);

  virtual void tick(float delta) override;
  virtual void render(Frame &f) override;
  virtual bool handle(const sf::Event &event) override;
};
}

}
