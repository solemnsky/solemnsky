/**
 * A cute demo for things.
 */
#ifndef SOLEMNSKY_DEMO_H
#define SOLEMNSKY_DEMO_H

#include "ui/ui.h"

class Demo : public ui::Control {
private:
  std::shared_ptr<ui::Control> previous;

  ui::Button skyButton{{800, 400}, "engine demo"};
  ui::Button graphicsButton{{800, 500}, "graphics demo"};
  ui::Button physicsButton{{800, 600}, "physics demo"};
  ui::Button backButton{{800, 700}, "go back"};

public:
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;
  void signalRead() override;
  void signalClear() override;
};

class LinkBack : public ui::Control {
private:
  std::shared_ptr<ui::Control> underlying;
  ui::Button backButton;

public:
  LinkBack(std::shared_ptr<ui::Control> ctrl);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;
  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_DEMO_H
