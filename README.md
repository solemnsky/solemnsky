Travis CI says: [![Build Status](https://travis-ci.org/solemnsky/solemnsky.svg?branch=master)](https://travis-ci.org/solemnsky/solemnsky)

# solemnsky

Hello wandering traveler.

solemnsky is an endeavour to, finding foundations in http://altitudegame.com,
 create a new open-source multiplayer 2d plane game designed to support 
 competition and community-driven design.
 
This is the solemnsky codebase. It's our best shot at making solemnsky happen.
 
## the making of solemnsky

Our design process consists in simultaneously writing useful code and not 
 making everything into a great Big Ball of Mud (a surprisingly difficult task).
 
After due consideration, we've chosen C++ as our _langage de résistance_. 
 Eventually, we plan to use Lua for scripting, in order to make design 
 iteration faster and modding easier.
 
## roadmap

LOC is the only honest quantifier of project growth; therefore all roadmapping 
 and growth extrapolation will be metricized in LOC and its derivatives. 
 Teeworlds (a comparable game written in a comparable source) has ~48,000 LOC;
 after due ~~hopeful guessing~~ calculation, I'm aiming for a solid release of 
 solemnsky at ~30,000 LOC. Extrapolation is possible, but probably 
 disheartening.
 
## licensing and credits

solemnsky is licensed under GPL.v3

Magnetic Duck writes the code.
LeGrape draws the graphics stuff.
CouleeApps (glcoulee) and the one known only as 'Jeff' occasionally fix 
things I mess up in C++.
A few other people shout at us to create things faster, thanks guys.

## Building

It's possible to build this project on most Windows, OSX, and Linux setups. 
Listed below are the particular setups that we test against.

 * Windows: install http://nuwen.net/mingw.html -style MinGW, build with CLion.
 * OSX: Install Boost (homebrew works fine), and CMake to Xcode projects. You can build those.
 * Linux: Should build easily, if you're having problems nix is a sure-fire way 
to not have problems anymore (see default.nix in top-level).

