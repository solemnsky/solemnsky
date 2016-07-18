Travis CI says: [![Build Status](https://travis-ci.org/solemnsky/solemnsky.svg?branch=master)](https://travis-ci.org/solemnsky/solemnsky)

# solemnsky
 
[Altitude](http://altitudegame.com) demonstrated the viability of an uncommon genera 
 of multiplayer game. Despite its cartoonish appearance, it offered a competitive 
 experience that kept a small dedicated player-base coming back year after year.

However, it suffers some unfortunate limitations and in recent years has become
 increasingly stagnant. Cue solemnsky: a new altitude -- an alti2ude, if you will -- that 
 exposes itself to community development and addresses its predecessor's old problems with 
 a new light.

This is solemnsky: the open-source competitive multiplayer 2D plane game.

## solemnsky-info

solemnsky is in active development. See our [solemnsky-info](https://github.com/solemnsky/solemnsky-info) repository for an overview into the technical design and current status of our continuing efforts to build the best "alti2ude" we can manage.

## You Can Help!

There are a lot of roles to fill on our team! Whether you want to contribute or just 
 drop by and say hi, contact Chris Gadzinski (MagneticDuck) via 
 [email](mailto:zenmags@gmail.com) or via skype (magnetic.duck); we'll get up set up
 on our slack group along with the rest of the team.

Some roles we'd particularly like to see filled:
 * Scheme guru.
 * C++ programmer.
 * Web designer.

We're dedicated to pulling this project through; your contributions will be matched.

## First Development Alpha Release

As of the moment that I write this, I've sketched support for almost every fundamental feature that we'll need in the final product. In order to become more attractive to potential contributors and avoid any over-extension of our limited resources -- my time and our team's interest in the project -- it would be prudent to aim directly at a near-future goal: presenting a development alpha release.

By the development alpha release, I plan to have a highly usable client/server system, with a concrete set of foundations for all that will come. It will be playable and presentable, and prepare the ground for the addition of scripting.

A list of the foreseeable developments necessary to reach this point follows.

* Client UI 
  * Should leave a good first impression, and be intuitive and functional. (Issue #18)
  * Should have useful settings, with support for persistence. (Issue #17)
* Environment (.sky) Format. 
  * Should be constructable from altitude's maps with an automated tool. (Issue #19) (DONE)
  * Should be easy to add custom game graphics to. (Issue #20)
* Engine 
  * Should be improved to allow the server's implementation of a basic TBD-like mode. (Issue #22)
* Engine Sandbox 
  * Should allow experimenting with engine tuning values. (Issue #22) (DONE)
* Multiplayer Client 
  * Should display lobby, environment loading, game, and score screens. (Issue #24) (DONE)
* Multiplayer Server 
  * Should implement a basic TBD-like mode. (Issue #23)
* Multiplayer 
  * Should employ packet buffering to compensate latency fluctuation in `SkyDelta` (dynamic game state) packets, on both client and server. (Issue #21)
* Logging
  * Should use an external library for fast asynchronous logging, and have a nice informational format. (Issue #16) (DONE)

## Licensing and Credits.

solemnsky  Copyright (C) 2016 Chris Gadzinski

This program is licensed under GPL v3, see LICENSE.

Credits: coming soon.

Of course, we'd like to thank [Nimbly Games](http://nimblygames.com), the authors of
 Altitude, for their great work that started this whole mess!

