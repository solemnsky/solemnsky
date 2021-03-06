/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * UI components specialized for the client.
 */
#pragma once
#include "ui/widgets.hpp"
#include "ui/settings.hpp"
#include "util/printer.hpp"

/**
 * Text entry / variable message log combo. Used in the sandbox and client.
 */
class MessageInteraction: public ui::Control {
 private:
 public:
  MessageInteraction() = delete;
  MessageInteraction(const ui::AppRefs &references);

  // Subcomponents.
  ui::TextEntry messageEntry;
  ui::TextLog messageLog;

  // Control impl.
  virtual void tick(const TimeDiff delta) override;
  virtual void render(ui::Frame &f) override;
  virtual bool handle(const sf::Event &event) override;
  virtual void reset() override;
  virtual void signalRead() override;
  virtual void signalClear() override;

  // ClientAction handling.
  virtual bool handleClientAction(const ui::ClientAction action, const bool state);

  // Signals.
  optional<std::string> inputSignal;

};

/**
 * Printer for engine events.
 */
class EnginePrinter: public JointPrinter {
 private:
  ConsolePrinter consolePrinter;

 public:
  EnginePrinter(MessageInteraction &messageInteraction);

};

/**
 * Printer printer for client events.
 */
class ClientPrinter: public JointPrinter {
 private:
  ConsolePrinter consolePrinter;

 public:
  ClientPrinter(MessageInteraction &messageInteraction);

};


/**
 * Printer for game console input/output.
 */
class GameConsolePrinter: private JointPrinter {
 private:
  ConsolePrinter consolePrinterBase;
  PrefixPrinter consolePrinter;

 public:
  GameConsolePrinter(MessageInteraction &messageInteraction);

  void input(const std::string &string);
  void output(const std::string &string);

};
