/**
 * Splash screen to cover loading resources.
 */
#pragma once
#include <memory>
#include "widgets/textlog.h"
#include <mingw.thread.h>
#include "control.h"

namespace ui {

namespace detail {
// TODO: make this threaded so the window doesn't freeze up
class SplashScreen: public Control {
 private:
  TextFormat loadingText;

  float progress;
  ui::TextLog textLog;
  std::function<std::unique_ptr<Control>(AppState &)> afterLoading;
  std::thread resourceThread;

 public:
  SplashScreen(AppState &appState,
               std::function<std::unique_ptr<Control>(AppState &)>
               afterLoading);
  ~SplashScreen();

  virtual void poll(float delta) override final;
  virtual void tick(float delta) override final;
  virtual void render(Frame &f) override final;
  virtual bool handle(const sf::Event &event) override final;
};
}

}
