============================
 Batchtank regulator server
============================

This folder contains the parts specific to the server. It consists of
the following parts:

IORegistry:
  This class is used as an interface against the batchtank server.
  Mutexes are used in order to make reading and writing to the process
  atomic operations.

ConnectionThread:
  Whenever a connection is made to the server a ConnectionThread is
  fired and given control over that particular connection. Each
  connection contains a main reading loop executing controller events.
  It is running as a detached thread.

PeriodicTask:
  Executes a function on a strictly regular basis. Uses clock_nanosleep
  with TIMER_ABSTIME and CLOCK_MONOTONIC and should have a fairly high
  resolution.

Sampler:
  Ran as periodic task to sample at specified intervals.


Use scenario
------------

The server listens to a port specified in an .ini file. When a client
connects the connection is assigned a dedicated ConnectionThread. Upon a
register event recieved a PeriodicTask is fired together with a sampler
that samples and sends samples to the client at a regular interval. The
register message contains data such as period time and what sensors to
read.

The client may respond with control signal events which specify what
value should be set as well as to what output. Also contains the
reference value used for this controlsignal.

Setting and getting values from the batch process is protected by a
IORegistry however locking of the registry is external, i.e. it contains a
public mutex and does no locking on its own. This is done in order to
enable batching several gets and sets as single atomic operations.
The IORegistry also contains copies of reference values as well as
control signals for plotting use cases.

Since calls to the process are fairly slow a polling thread updates the
IORegistry monitor on a periodic basis.


Design choices
--------------
The server is written in C++ in order to run well on the Raspberry Pi as
there is currently not any acceptable JVM with proper JIT available. It
also makes calling timing functions more appropriate for real time tanks
considerably easier.



BUILD Debian/Ubuntu/Raspbian
============================
The following packages are required to build::
    protobuf-compiler libprotobuf-dev libboost-system-dev libboost-thread-dev
