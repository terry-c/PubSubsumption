/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

#include "Arduino.h"
//#include <EEPROM.h>

/// Print the value of the given variable, using the variable name as a label.
/// Uses the F() macro to print strings directly from Program Memory without using RAM
#define PRINT_VAR( x ) Serial.print( F( #x " = " ) ); Serial.println( x );

#define USE_CSV
#ifdef USE_CSV

/// Determine whether to output CSV headings or data
#define IF_CSV( MASK ) if ( _messageMask & MASK && pControlParams->PrintingCsv() )

#define IF_MSG( MASK ) if ( _messageMask & MASK )

/// Output either heading or data, separated by commas (or other delimiter)
#define CSV_OUT( VAR ) \
    if ( pControlParams->PrintingCsvHeadings() ) { \
        Serial.print( _pName ); \
        Serial.print( ':' ); \
        Serial.print( F( #VAR ) ); \
    } \
    else \
        Serial.print( VAR ); \
    Serial.print( pControlParams->GetCsvDelimiter() );
#else
#define IF_CSV
#define IF_MSG
#define CSV_OUT
#endif

/// Message Mask bits
/// Serial informational and diagnostic output is controlled by
/// bitmapped variables in each class.  In Actors, this is controlled
/// by the *V command.  For example, to enable all messages in the LED
/// class, issue the command "LV 31"
#define MM_RESPONSES    0x01
#define MM_ID           0x02
#define MM_PROGRESS     0x04
#define MM_CALC         0x08
#define MM_INFO         0x10
#define MM_CSVBASIC     0x20
#define MM_CSVEXTENDED  0x40


//template <class T> int EEPROM_writeAnything(int ee, const T& value)
//{
//  const byte* p = (const byte*)(const void*)&value;
//  unsigned int i;
//  for (i = 0; i < sizeof(value); i++)
//    EEPROM.write(ee++, *p++);
//  return i;
//}
//
//template <class T> int EEPROM_readAnything(int ee, T& value)
//{
//  byte* p = (byte*)(void*)&value;
//  unsigned int i;
//  for (i = 0; i < sizeof(value); i++)
//    *p++ = EEPROM.read(ee++);
//  return i;
//}


