# PubSubsumption
___

A Subsumption Architecture robotic control system for Arduino, built around a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the Subsumption Architecture as described by David P. Anderson.

## Synopsis
 
This is an experimental framework to explore the Subsumption Architecture.  It differs from the typical implementation in that there is no arbitrator to determine which behavior "wins" control.
Instead, a "token" is passed to each behavior in priority order.  The first behavior to take control is the "winner".

The Publisher/Subscriber mechanism is used in two ways in this system.  The core of the system is the Subsumption chain, which is controlled by an object called Director.
The Director is a Publisher, which sets up the token (called ControlParams) and passes it to its Subscribers, which are the behaviors (called "Actors").  The ControlParams object contains the Subsumption flag and the throttle settings for left and right motors.
At the end of the Subsumption chain (the last subscriber) is the MotorDriver, which applies the throttle values.

The other Publisher in this system is the CommandDispatcher.  Any object in the system which can be controlled subscribes to events from this Publisher.  The CommandDispatcher checks the Serial port for
console commands and dispatches them to the appropriate Subscriber.  A simple command structure is defined, consisting of one or more characters beginning with a letter, followed by up to 4 numeric arguments
delimited by spaces or commas.  The initial letter can be seen as a noun, corresponding to a specific Subscriber.  The second character is typically a verb, or subcommand.  Other characters may be used as command modifiers.
The CommandDispatcher has no "knowledge" of the commands beyond the basic structure.  All interpretation is up to the Subscribers.

The CommandDispatcher also has a "menu mode".  Entering just the first command letter with no subcommands or arguments puts it into this mode and presents a submenu for that command.  The current implementation smells a bit hacky, but works well enough to evaluate this feature.  The command mode still works as before, with the exception that you have to be sure to be at the top level to enter a command.

## Status

Currently in development and evolving.  This code has been developed to target the Arduino Pro Mini platform, and currently consumes about 70% of the code space and 40% of the RAM on that device.  Much of this is text which may become extraneous.
It has also been tested on the Arduino Due.
