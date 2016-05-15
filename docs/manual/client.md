# Client

The client is the tool a player uses to ultimately interact with a multiplayer game.
 It should provide information to them clearly, allow them to alter settings, and join and
 play a game. Finally, it should get out of their noses. Simple!

solemnsky uses its own tiny (think ~2000 LOC) UI framework, defined in the `ui` namespace,
 to describe its GUI in a sane way. The client builds on this framework, defining a
 structured hierarchy of interfaces to represent the various elements of the client.

## ui Namespace (`src/ui`)

### Control

Solemnsky's UI framework is built around a `Control` interface that describes an element
 of a user interface. It can handle events, tick logic, and draw to the screen, as well
 as hold child Control objects. Child Control objects are relayed the callbacks of their
 parents by default.
 
Two callbacks for reading and writing signals are also provided, meant to simplify
 the management of signals in the GUI (e.g. button clicks). 

   // 'page' is a parent of 'button'
   page.handle(/*user clicks mouse*/); // button is clicked, sets signal
   page.signalRead(); // page reads button.clickSignal
   page.signalClear(); // all signals are cleared

`ControlExec` runs a `Control`, invoking its callbacks within a game loop.

### Resources

Resources (fonts and images, and soon sounds) are loaded by `ControlExec` during an initial
 splash screen (`ui::detail::ExecWrapper`). They, along with their metadata (e.g. spritesheet
 layout), can be accessed from the global methods exposed in `src/util/client/resources.h`.

## Client (`src/client`)

## Elements

#### TODO

## Styling

#### TODO

#### TODO


