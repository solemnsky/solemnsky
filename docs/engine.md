# Engine

The heart of solemnsky is the engine. It's comprised of a small set of utilities, which
 define some basic project "glue" in the form of handy methods and expressive types,
 and a central `sky` namespace that answers a simple question: what a game of solemnsky 
 *is*. The presentation of this game is delegated to the client, and the serving of a
 game to multiple clients is handled by the server. What remains for `sky` to handle is
 the intersection of the server and client dependencies: particularly, a networkable model 
 of the game being played with an entry-point for additional layers of logic and state. 

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

### Invariant Protection in Packets

The `VerifyStructure` interface is used to protect against invariant violations in packets;
 defined in `src/util/types.h` it describes a class that can check the integrity of its 
 invariants. `tg::Telegraph` protects against malformed packets through this interface
 when available, printing an error message and refusing to catch the responsible packet when 
 invariant are not respected.

## Sky Namespace (`src/sky`)

### Arena 

 The engine is held together by `sky::Arena`, a representation of the backbone of a 
 multiplayer game. An Arena manages players along with some basic associated data, 
 controls their join / quits, and stores some basic game / server state (name, motd, 
 etc.). Both the client and server hold an Arena to model the game being played.

## Subsystem

The arena itself does not manage the more complex state / logic associated
 with a game, such as physical game state or client-side graphical state. Instead,
 these additional layers are abstracted as "Subsystems". A Subsystem, derived from the
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

`sky::Sky` is defined as a Subsystem, specifically `sky::Subsystem<Participation>`.

### Multiplayer Protocol

Game protocol packet types for use by the server and the client respectively are also defined
 in the engine, along with their serialization rules. They implement `VerifyStructure`, 
 for basic protection against malformed packets, as described in "Invariant Protection in 
 Packets"

### Events

To provide an overview of events that occured in an Arena to a user, an `ArenaLogger` can be 
 attached to an Arena, in order to capture `ArenaEvent`s. ArenaEvents, unlike other elements
 of the Arena API, have substance even when taken away from their parent arena -- instead 
 of PIDs or references, they contain printable information.

`ClientEvent` and `ServerEvent`, defined outside of the `sky` namespace, are dependant
 on ArenaEvent. They encode, in addition to ArenaEvent, events specific to the client
 or the server, respectively.



