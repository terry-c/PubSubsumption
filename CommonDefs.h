/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once
#define REAL_DUINO
#ifdef REAL_DUINO
#include "Arduino.h"
#else
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "WString.h"

#define HEX 16
#define PI  3.141592653
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define HIGH 0x1
#define LOW  0x0


// fakeduino stuff
class SimSerial {
public:
    void print() {;}
    void print( int, int f=0 )  {;}
    void print( unsigned int, int f=0 )  {;}
    void print( const char[], int f=0 ){;}
    void print( unsigned long, int f=0 ) {;}
    void print( float, int f=0 ){;}
    void print( double, int f=0 ) {}

    void println() {;}
    void println( int, int f=0 )  {;}
    void println( unsigned int, int f=0 )  {;}
    void println( const char[], int f=0 ){;}
    void println( long, int f=0 ) {;}
    void println( unsigned long, int f=0 ) {;}
    void println( float, int f=0 ){;}
    void println( double, int f=0 ) {}

    int available( void );
    void begin( long );
    int read();
};

void pinMode(uint8_t, uint8_t) {}
void digitalWrite(uint8_t, uint8_t) {}
int digitalRead(uint8_t) { return 0; }
int analogRead(uint8_t) { return 0; }
void analogReference(uint8_t mode) {}
void analogWrite(uint8_t, int) {}

unsigned long millis(void) {}
unsigned long micros(void) {}

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
long map(long, long, long, long, long) { return 0; }

SimSerial Serial;
#endif

//#include <EEPROM.h>

//#define F( x ) x

/// Print the value of the given expression, using the expression text as a label.
/// Uses the F() macro to print strings directly from Program Memory without using RAM
#define PRINT_VAR( x ) Serial.print( F( #x " =\t" ) ); Serial.println( x );

#define IF_MSG( MASK ) if ( _messageMask & MASK )

#define USE_CSV

#ifdef USE_CSV

/// Determine whether to output CSV headings or data
#define IF_CSV( MASK ) if ( _messageMask & MASK && pControlParams->PrintingCsv() )

/// Output either heading or data, separated by commas (or other delimiter)
#define CSV_OUT( VAR ) \
    if ( pControlParams->PrintingCsvHeadings() ) { \
        Serial.print( _pName ); \
        Serial.print( ':' ); \
        Serial.print( F( #VAR ) ); \
    } \
    else { \
        Serial.print( VAR ); \
    } \
    Serial.print( pControlParams->GetCsvDelimiter() );
#else
#define IF_CSV( X ) 
#define CSV_OUT( X )
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


#define TicksPerInch( TicksPerRev, Diameter )  ( TicksPerRev / (Diameter * PI) )