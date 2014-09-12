/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

#include "Arduino.h"

/// Print the value of the given variable, using the variable name as a label.
/// Uses the F() macro to print strings directly from Program Memory without using RAM
#define PRINT_VAR( x ) Serial.print( F( #x " = " ) ); Serial.println( x );
