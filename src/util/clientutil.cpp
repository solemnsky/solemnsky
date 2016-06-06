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
#include "clientutil.h"

inline sf::Uint8 mix(const sf::Uint8 x1,
                     const sf::Uint8 x2,
                     const float degree) {
  const float _x1(x1), _x2(x2);
  return (sf::Uint8) (std::round(degree * (_x2 - _x1)) + _x1);
}

sf::Color mixColors(const sf::Color color1, const sf::Color color2,
                    const float degree) {
  return sf::Color(mix(color1.r, color2.r, degree),
                   mix(color1.g, color2.g, degree),
                   mix(color1.b, color2.b, degree),
                   mix(color1.a, color2.a, degree)
  );
}

std::string printKey(const sf::Keyboard::Key key) {
  switch (key) {
    case sf::Keyboard::A:
      return "A";
    case sf::Keyboard::B:
      return "B";
    case sf::Keyboard::C:
      return "C";
    case sf::Keyboard::D:
      return "D";
    case sf::Keyboard::E:
      return "E";
    case sf::Keyboard::F:
      return "F";
    case sf::Keyboard::G:
      return "G";
    case sf::Keyboard::H:
      return "H";
    case sf::Keyboard::I:
      return "I";
    case sf::Keyboard::J:
      return "J";
    case sf::Keyboard::K:
      return "K";
    case sf::Keyboard::L:
      return "L";
    case sf::Keyboard::M:
      return "M";
    case sf::Keyboard::N:
      return "N";
    case sf::Keyboard::O:
      return "O";
    case sf::Keyboard::P:
      return "P";
    case sf::Keyboard::Q:
      return "Q";
    case sf::Keyboard::R:
      return "R";
    case sf::Keyboard::S:
      return "S";
    case sf::Keyboard::T:
      return "T";
    case sf::Keyboard::U:
      return "U";
    case sf::Keyboard::V:
      return "V";
    case sf::Keyboard::W:
      return "W";
    case sf::Keyboard::X:
      return "X";
    case sf::Keyboard::Y:
      return "Y";
    case sf::Keyboard::Z:
      return "Z";
    case sf::Keyboard::Num0:
      return "Num0";
    case sf::Keyboard::Num1:
      return "Num1";
    case sf::Keyboard::Num2:
      return "Num2";
    case sf::Keyboard::Num3:
      return "Num3";
    case sf::Keyboard::Num4:
      return "Num4";
    case sf::Keyboard::Num5:
      return "Num5";
    case sf::Keyboard::Num6:
      return "Num6";
    case sf::Keyboard::Num7:
      return "Num7";
    case sf::Keyboard::Num8:
      return "Num8";
    case sf::Keyboard::Num9:
      return "Num9";
    case sf::Keyboard::Escape:
      return "Escape";
    case sf::Keyboard::LControl:
      return "LControl";
    case sf::Keyboard::LShift:
      return "LShift";
    case sf::Keyboard::LAlt:
      return "LAlt";
    case sf::Keyboard::LSystem:
      return "LSystem";
    case sf::Keyboard::RControl:
      return "RControl";
    case sf::Keyboard::RShift:
      return "RShift";
    case sf::Keyboard::RAlt:
      return "RAlt";
    case sf::Keyboard::RSystem:
      return "RSystem";
    case sf::Keyboard::Menu:
      return "Menu";
    case sf::Keyboard::LBracket:
      return "LBracket";
    case sf::Keyboard::RBracket:
      return "RBracket";
    case sf::Keyboard::SemiColon:
      return "SemiColon";
    case sf::Keyboard::Comma:
      return "Comma";
    case sf::Keyboard::Period:
      return "Period";
    case sf::Keyboard::Quote:
      return "Quote";
    case sf::Keyboard::Slash:
      return "Slash";
    case sf::Keyboard::BackSlash:
      return "BackSlash";
    case sf::Keyboard::Tilde:
      return "Tilde";
    case sf::Keyboard::Equal:
      return "Equal";
    case sf::Keyboard::Dash:
      return "Dash";
    case sf::Keyboard::Space:
      return "Space";
    case sf::Keyboard::Return:
      return "Return";
    case sf::Keyboard::BackSpace:
      return "BackSpace";
    case sf::Keyboard::Tab:
      return "Tab";
    case sf::Keyboard::PageUp:
      return "PageUp";
    case sf::Keyboard::PageDown:
      return "PageDown";
    case sf::Keyboard::End:
      return "End";
    case sf::Keyboard::Home:
      return "Home";
    case sf::Keyboard::Insert:
      return "Insert";
    case sf::Keyboard::Delete:
      return "Delete";
    case sf::Keyboard::Add:
      return "Add";
    case sf::Keyboard::Subtract:
      return "Subtract";
    case sf::Keyboard::Multiply:
      return "Multiply";
    case sf::Keyboard::Divide:
      return "Divide";
    case sf::Keyboard::Left:
      return "Left";
    case sf::Keyboard::Right:
      return "Right";
    case sf::Keyboard::Up:
      return "Up";
    case sf::Keyboard::Down:
      return "Down";
    case sf::Keyboard::Numpad0:
      return "Numpad0";
    case sf::Keyboard::Numpad1:
      return "Numpad1";
    case sf::Keyboard::Numpad2:
      return "Numpad2";
    case sf::Keyboard::Numpad3:
      return "Numpad3";
    case sf::Keyboard::Numpad4:
      return "Numpad4";
    case sf::Keyboard::Numpad5:
      return "Numpad5";
    case sf::Keyboard::Numpad6:
      return "Numpad6";
    case sf::Keyboard::Numpad7:
      return "Numpad7";
    case sf::Keyboard::Numpad8:
      return "Numpad8";
    case sf::Keyboard::Numpad9:
      return "Numpad9";
    case sf::Keyboard::F1:
      return "F1";
    case sf::Keyboard::F2:
      return "F2";
    case sf::Keyboard::F3:
      return "F3";
    case sf::Keyboard::F4:
      return "F4";
    case sf::Keyboard::F5:
      return "F5";
    case sf::Keyboard::F6:
      return "F6";
    case sf::Keyboard::F7:
      return "F7";
    case sf::Keyboard::F8:
      return "F8";
    case sf::Keyboard::F9:
      return "F9";
    case sf::Keyboard::F10:
      return "F10";
    case sf::Keyboard::F11:
      return "F11";
    case sf::Keyboard::F12:
      return "F12";
    case sf::Keyboard::F13:
      return "F13";
    case sf::Keyboard::F14:
      return "F14";
    case sf::Keyboard::F15:
      return "F15";
    case sf::Keyboard::Pause:
      return "Pause";
    default:
      return "<unknown key>";
  }
}
std::string pathFromResourceUrl(const std::string &url) {
  return "../../" + url;
}




