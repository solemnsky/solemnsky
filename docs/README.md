# solemnsky technical documentation

Welcome to the solemnsky technical documentation. This directory aims to give an 
 up-to-date overview of the solemnsky project for developers.

## Necessary Technology

solemnsky is build from the ground up on a GCC toolchain. Our only non-trivial system
 dependency is boost; a comprehensive list of our dependencies can be found in the
 nix build file (default.nix) at the top-level.

The project should be buildable on Windows, Linux, and OSX. It is, however, currently only 
 actively tested on Windows and Linux.

 * Windows: install http://nuwen.net/mingw.html -style MinGW, build with CLion.
 * OSX: Install Boost (homebrew works fine), and CMake to Xcode projects. You can build those.
 * Linux: Should build easily, if you're having problems just `nix-build . -A default` and then you don't have problems anymore.

## Thirdparty

We source (via submodules) 4 thirdparty libraries in the thirdparty/ directory:

* Box2D: Simple 2d physics.
* cereal: Efficient binary serialization / deserialization.
* enet: packet-based transmission control.
* SFML: graphics and geometry types.

## Global Structure

To supplement the per-component documentation present in the code, here is an overview
 of solemnsky's general design.

### Networking and Packets

A Networkable entity is described with the templated `Networked<Init, Delta>` interface.
 It can be copied over the network at any point with a serializable `Init` type: 

    Networked() = delete;
    Networked(const Init &) { }
    virtual Init captureInitializer() const = 0;
 
 and encodes sequent changes in the `Delta` type: 

    virtual void applyDelta(const Delta &) = 0;

{Des,S}erialization is accomplished through cereal. 

`src/util/telegraph.h` is a small wrapper around enet, defined in the `tg` namespace; 
 all networking happens through it.

### Engine (`src/sky`)

At the very heart of solemnsky is the `sky` namespace. It defines a small game engine, 
 held together by `sky::Arena`, which represents the backbone of a multiplayer game.
 It manages players along with some basic associated data, controls their join / quits,
 and stores some basic game / server state (name, motd, etc.).

The arena in itself does not manage much of the more complex state / logic associated
 with a game, such as physical game state or client-side graphical state. Instead,
 these additional layers are abstracted as "subsystems". A subsystem, derived from the
 base template class `sky::Subsystem<PlayerData>` can store data (of type PlayerData) in
 each player, and can listen to callbacks from the Arena. 

To exemplify the operation of a subsystem:

    sky::Arena arena(...);
    sky::Sky sky(arena, ...); // sky is a subsystem attached to arena
    arena.connectPlayer("nameless plane"); // sky gets an onJoin() callback and attaches
                                           // state to the new player
    arena.getPlayer(0)->spawn(...); // sky gets an onSpawn() callback, and changes the state
                                    // of the player to reflect that it is now spawned

See also: `tests/subsystemtest.cpp`.

##### TODO #####

