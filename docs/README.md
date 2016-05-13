# solemnsky technical documentation

Welcome to the solemnsky technical documentation. This directory aims to give an 
 up-to-date overview of the solemnsky project for developers.

## Necessary Technology

solemnsky is built from the ground up on a GCC toolchain. Our only non-trivial 
 dependency is boost; a comprehensive list of our dependencies can be found in the
 nix build file (default.nix) at the top-level.

The project should be buildable on Windows, Linux, and OSX. It is, however, currently only 
 actively tested on Windows and Linux.

 * Windows: install http://nuwen.net/mingw.html -style MinGW, build with CMake.
 * OSX: Install Boost (homebrew works fine), and CMake to Xcode projects. You can build those.
 * Linux: Should build easily with CMake, if you're having problems just `nix-build . -A default` and then you don't have problems anymore.

## Thirdparty

We source 4 thirdparty libraries in the thirdparty/ directory:

* Box2D: Simple 2d physics.
* cereal: Efficient binary serialization / deserialization.
* enet: packet-based transmission control.
* SFML: graphics and geometry types.

## Global Structure

To supplement the per-component documentation present in the code, this directory
 contains an overview of solemnsky's design, in three components:

 * `engine.md`: The `sky` namespace and control types. The core of solemnsky. Corresponds to `src/sky` and `src/util`.
 * `client.md`: The `ui` namespace, and the game client. The UI of solemnsky. Corresponds to `src/ui` and `src/client`
 * `server.md`: The game server. Corresponds to `src/server`.

