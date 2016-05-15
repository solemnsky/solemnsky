# solemnsky technical guide

## Building

solemnsky is built from the ground up on a GCC / CMake toolchain. Our only non-trivial 
 dependency is boost; a comprehensive list of our dependencies can be found in the
 nix build file (default.nix) at the top-level.

The project should be buildable on Windows, Linux, and OSX. It is, however, currently only 
 actively tested on Windows and Linux.

 * Windows: install http://nuwen.net/mingw.html -style MinGW, build with CMake.
 * OSX: Install Boost (homebrew works fine), and CMake to Xcode projects. You can build those.
 * Linux: Should build easily with CMake, if you're having problems just 
   `nix-build . -A default` and then you don't have problems anymore.

## Thirdparty

We source 4 thirdparty libraries in the thirdparty/ directory:

 * Box2D: Simple 2D physics.
 * cereal: Efficient binary serialization / deserialization.
 * enet: Packet-based transmission control.
 * SFML: Graphics for the client and geometry types.

## Documentation

solemnsky's documentation is intended to provide a sufficiently detailed overview of the
 core C++ codebase to allow developers to contribute in meaningful ways.

It is present in the form of three components:

 * Inline documentation alongside declarations in the header files.
 * A brief manual in this directory (`manual/`)
 * Some blueprint diagrams, to give a sense of the project's overall structure (`uml/`).

## Status

To give a sense of how the project is moving along, we periodically publish a status update;
 an archive of past updates, along with the current one, can be found in `status/`.

