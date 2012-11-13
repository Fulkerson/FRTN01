===========
 Batchtank 
===========

This project aims at creating a control system for a batch process in
the course FRTN01_

.. _FRTN01: http://www.control.lth.se/course/FRTN01/

----

The system is composed of the following parts:

batchtank_server
  TCP server used for reading and writing to the actual process. Resides
  in the `server` directiory.

control
  Library containing control things. At the moment only a PID regulator.

client
  Client which connects to the server and regulates using the PID in control.
 
