/**
 * Wrapper control that makes the base Control's life easier.
 */
#pragma once
#include <memory>
#include "widgets/textlog.h"
#include "control.h"

namespace ui {

namespace detail {
/**
 * Supplied with a constructor for a main Control, this loads resources while
 * displaying a splash screen and fades into the main Control when resources
 * are ready. It also relays the wrapped control's `quitting` flag.
 */
// TODO: make this threaded so the window doesn't freeze up
class ExecWrapper: public Control {
 private:
  TextFormat loadingText;
  std::function<std::unique_ptr<Control>(AppState &)> mainCtrlCtor;
  optional<Control> mainCtrl;

 public:
  ExecWrapper(AppState &appState,
              std::function<std::unique_ptr<Control>(AppState &)>
              mainCtrlCtor);

  virtual void poll(float delta) override final;
  virtual void tick(float delta) override final;
  virtual void render(Frame &f) override final;
  virtual bool handle(const sf::Event &event) override final;
};
}

}
