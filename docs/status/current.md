# solemnsky status 2016/5/15

This is the first solemnsky status update! Hopefully I've been making a habit of keeping
 these coming out as the project advances. 

I'll try to fill two sections on each status update: 'accomplished', for things we've
 done since the last update, and 'next steps' for whatever we'll probably be working on
 next.

# accomplished

Since this is the first, there isn't any differential to mention relative to a previous
 update; instead, I'll give a very brief picture of what we have working so far:

 * The engine (`src/sky` and `src/util`):
   * A clear model of the underlying game (`sky::Arena`, `sky::Sky` etc.).
   * A simple API along with an abstraction for modularizing state and logic 
     (`sky::Subsystem`).
   * A pleasant network synchronization pattern (`Networked` + cereal + `Telegraph` 
     + `VerifyStructure`).
   * Enslavement of `box2d` and implementation of basic game mechanics.
 * The client (`src/client` and `src/ui`):
   * Underlying UI framework.
   * UI scaffolding, with partitioned designed and support for bells and whistles.
   * Pages (settings, menu, server listing).
   * Shared state, settings, signals, etc.
   * Resources.
   * Game graphics (`sky::RenderSystem`).
   * Multiplayer client.
 * The server (`src/server`):
   * Server interface, derived from Subsystem.
   * Builds / works fine on minimal dependency system.
 * Travis CI integration.
 * GTest testing.
 * Fun placeholder graphics.
 * A first pass at some attractive documentation.

The software is building and working on Windows and Linux. As it stands, we can
 start a server on a remote box, fire up clients, and chat / start games / change maps /
 fly around in our cool planes / etc.

# next steps

Per recommendation of emblis, I'll be continuing to write documentation as I have it
 scaffolded out in `docs/`, along with possibly a pass or two through the code itself
 to verify that things are up to snuff and documented properly.

The next piece of development will be to put a JIT scripting language in the server 
 and attach it to a modding interface.

