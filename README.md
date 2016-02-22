# solemnsky

Hello wandering traveler.

solemnsky is an endeavour to, finding foundations in http://altitudegame.com,
 create a new open-source multiplayer 2d plane game designed to support 
 competition and community-driven design.
 
This is the solemnsky codebase; barring the contributions of a benevolent 
 artificial super-intelligence or some other wholly unlikely turn of events, 
 writing code is our best bet at turning solemnsky into a reality.
 
## the making of solemnsky

Our design process consists in simultaneously writing useful code and not 
 making everything into a great Big Ball of Mud (a surprisingly difficult task).
 
After due consideration, we've chosen C++ as our language de résistance. 
 Eventually, we plan to use Lua for scripting, in order to make design 
 iteration faster and modding easier.
 
## roadmap

LOC is the only true quantifier of project growth; therefore all roadmapping 
 and growth extrapolation will be metricized in LOC and its derivatives. 
 Teeworlds (a comparable game written in a comparable source) has ~48,000 LOC;
 after due calculation, I'm aiming for a solid release of solemnsky at 
 ~30,000 LOC. I'm frightened to extrapolate solemnsky's release date 
 from this.
 
## licensing and credits

solemnsky is licensed under GPL.v3

Magnetic Duck writes the code.
LeGrape draws the graphics stuff.
CouleeApps (glcoulee) and the one known only as 'Jeff' occasionally fix 
things I fuck up in C++.
A few other people shout at us to create things faster, thanks guys.

## Building

It's possible to build this project on most Windows, OSX, and Linux setups. 
Listed below are the particular setups that we test against.

Windows: install http://nuwen.net/mingw.html -style MinGW, build with CLion.
OSX: Just Works (tm), plus or minus libraries. (Probably.)
Linux: Should build easily, if you're having problems nix is a sure-fire way 
to not have problems anymore (see default.nix in top-level).
