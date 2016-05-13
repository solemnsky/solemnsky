# Engine

The heart of solemnsky is the engine. It's comprised of a small set of utilities, which
 define some basic project "glue" in the form of handy methods and expressive types,
 and a central `sky` namespace that answers a simple question: what a game of solemnsky 
 *is*. The presentation of this game is delegated to the client, and the serving of a
 game to multiple clients is handled by the server. What remains for `sky` to handle is
 the intersection of the server and the client: state, simulation, networking, protocols,
 maps, and an entry-point for modding and modularization of state and logic, among other
 things.

## Utils (`src/util`)

### Networking and Packets

A Networkable entity is described with the templated `Networked<Init, Delta>` interface.
 It describes a class that can be copied over the network at any point with a 
 serializable `Init` type: 

    Networked() = delete;
    Networked(const Init &) { }
    virtual Init captureInitializer() const = 0;
 
and can encode sequent changes in the `Delta` type: 

    virtual void applyDelta(const Delta &) = 0;

The particular way in which Deltas are formed is variable; `sky::Arena`'s deltas are
 explicitly formed (constructed by the user), while other entities such as `sky::Sky`
 form deltas automatically with `Delta captureDelta()`. This decision is made per-case.

{Des,S}erialization is accomplished through cereal. 

`src/util/telegraph.h` is a small wrapper around enet, defined in the `tg` namespace; 
 all networking happens through it.

## Sky Namespace (`src/sky`)

### Arena 

At the very heart of solemnsky is the `sky` namespace. It defines a small game engine, 
 held together by `sky::Arena`, which represents the backbone of a multiplayer game.
 An Arena manages players along with some basic associated data, controls their join / quits,
 and stores some basic game / server state (name, motd, etc.). Both the client and
 server hold an Arena to model the game.

The arena itself does not manage the more complex state / logic associated
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

### Sky

`sky::Sky` is the juicy bit of the game engine. It's instantiated when the Arena is in
 game mode, and represents the coherent definition of a physical world, in which each
 `sky::Player` holds a `sky::Participation`, which embodies a potential plane, and a set of
 additional toys with which they can do a whole manner of things.

### Protocol

#### TODO

#### TODO



