# Server

Thanks to the hard work of the engine, the server's job is, in principle, fairly light. 
 It can reserve its efforts to provide an entry-point for modding (entirely 
 server-side logic modifications) and a remote console for use by clients.

## Structure

### Server and ServerExec

`ServerExec` is analogous to `ui::ControlExec`; instead of running a `ui::Control`, however,
 it runs a `ServerListener`, a subclass of `SubsystemListener`. Similar to how 
 `SubsystemListener` is the type-erased interface to `Subsystem`, `ServerListener` is
 the type-erased interface to `ServerListener`. The ultimate objective of this diagonal
 inheritance (`ServerListener` -> `SubsystemListener`; `Server` -> `ServerListener`;
 `Subsystem` -> `SubsystemListener`; `Server` -> `Subsystem`) is to provide a 
 `Subsystem` with additional, Server-specific callbacks. 

While `ServerExec` manages the basic networking protocol, including the syncing of an
 `sky::Arena`, `sky::SkyHandle` and `sky::Scoreboard`, `Server` is left to add additional
 layers of logic and state, given access to a `ServerShared` object for networking.

### Modding

`Server`, as explained above, is our modding interface. It has access to all the
 classic `Subsystem` callbacks and triggers, along with an `onPacket`
 callback and control of a `ServerShared`. Integration of a scripting language
 will happen through this abstraction; for now, we limit ourselves to implementing
 a sketch of the functionalities, later implemented in our scripting API, with a
 hard-coded `Server<>` implementation.
`
