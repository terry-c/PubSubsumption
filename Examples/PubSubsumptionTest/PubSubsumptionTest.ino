/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#define USE_LED_EMULATOR
//#define ROVER5_DUE
#define USE_CSV

// Platform geometry defines
#define WHEEL_DIAMETER                  2.5
#define WHEEL_SPACING                   7.25
#define ENCODER_TICKS_PER_REVOLUTION    333

#include "CommonDefs.h"
#include <CommandDispatcher.h>
#include <CommandSubscriber.h>
#include <Actor.h>
#include <CruiseControl.h>
#include <WaypointManager.h>
#include <CollisionAvoidance.h>
#include <CollisionRecovery.h>
#include <Position.h>
#include <PubSub.h>
#include <Director.h>
#include <Navigator.h>
#include <LEDDriver.h>
#include <MotorDriver.h>

/// these will point to the member elements in the Position class which track
/// encoder positions.  These pointers are global so we can update them in
/// interrupt handlers
uint32_t* _pEncoderPositionLeft;
uint32_t* _pEncoderPositionRight;

#ifdef ROVER5_DUE

uint8_t _pinLeftEncoderA = 49;
uint8_t _pinLeftEncoderB = 47;
uint8_t _pinRightEncoderA = 53;
uint8_t _pinRightEncoderB = 51;
uint8_t _pinLeftEncoderXor = 45;//grn,brn,ch3int
uint8_t _pinRightEncoderXor = 44;//red,grn,ch4int

#else   // Pro Micro

uint8_t _pinLeftEncoderA = 2;
uint8_t _pinLeftEncoderB = 10;
uint8_t _pinRightEncoderA = 3;
uint8_t _pinRightEncoderB = 12;

#endif


// our publishers
CommandDispatcher   dispatcher;
Director            director( &dispatcher, 1000 );    // 1000 ms interval

// other entities
WaypointManager     waypointManager( &dispatcher );
Position            position( &dispatcher, &director, _pEncoderPositionLeft, _pEncoderPositionRight, TicksPerInch( ENCODER_TICKS_PER_REVOLUTION, WHEEL_DIAMETER ), WHEEL_SPACING );     // this carries the current positions of all motors.

// Actors.  These are objects which participate in the Subsumption chain.
// They will be subscribed to Director command events in setup().
//
// LEDDriver is the last Behavior, consuming any adjustments made by higher-priority behaviors.
// LEDDriver is a motor simulator
#ifdef USE_LED_EMULATOR
LEDDriver           led( 9, 6, 10, 5, &dispatcher, &position, TicksPerInch( ENCODER_TICKS_PER_REVOLUTION, WHEEL_DIAMETER ) );
#else
MotorDriver         motor( 2, 3, 4, 5, 8, 9, 10, 11, &dispatcher, &position );
#endif

// Navigator keeps us on course toward the next waypoint
Navigator           navigator( &dispatcher, &position, &waypointManager );

// CollisionRecovery responds to bumping into things.
CollisionRecovery   bumper( &dispatcher, 0, 0 );

// CruiseControl maintains the current heading and speed
CruiseControl       cruise( &dispatcher, &position, TicksPerInch( ENCODER_TICKS_PER_REVOLUTION, WHEEL_DIAMETER ), WHEEL_SPACING );

void setup()
{
    Serial.begin(115200);

#ifndef USE_LED_EMULATOR
    pinMode( _pinLeftEncoderA , INPUT );
    pinMode( _pinLeftEncoderB , INPUT );
    pinMode( _pinRightEncoderA, INPUT );
    pinMode( _pinRightEncoderB, INPUT );
    pinMode( _pinRightEncoderXor, INPUT );
    pinMode( _pinLeftEncoderXor, INPUT );
#endif

#ifdef ROVER5_DUE
    attachInterrupt( _pinLeftEncoderA , encoderLeftA, CHANGE );
    attachInterrupt( _pinLeftEncoderB , encoderLeftB, CHANGE );
    attachInterrupt( _pinRightEncoderA, encoderRightA, CHANGE );
    attachInterrupt( _pinRightEncoderB, encoderRightB, CHANGE );
#else // Pro Micro
    attachInterrupt( 0, encoderLeftA, RISING );
    attachInterrupt( 1, encoderRightA, RISING );
#endif

    // subscribe Actors to the Director in reverse priority order

#ifdef USE_LED_EMULATOR
    led.SubscribeTo( &director );
#else
    motor.SubscribeTo( &director, 0 );    // this is first (last) because it's the endpoint.
#endif

    cruise.SubscribeTo( &director );
    navigator.SubscribeTo( &director );
    bumper.SubscribeTo( &director );
    position.SubscribeTo( &director );  // Position is top priority so it can snapshot encoders at regular intervals
}

void loop()
{
    // time slices for CommandDispatcher and Director
    dispatcher.Update();
    director.Update();

    // time slices for other objects which need time:
//    led1.Update();
//    led2.Update();
  /* add main program code here */

}

#ifdef ROVER5_DUE
// On Rover5, we have enough interrupts to capture both A & B transitions.
// In addition, the Rover5 driver board includes an XOR of A and B, which we can 
// use to determine direction.  On every A transition, the XOR will be high for
// one direction and low for the other direction.  On every B transition, the
// direction/XOR relationship will be reversed.
void encoderLeftA()
{
    if ( ! digitalRead( _pinLeftEncoderXor ) ) {
        (*_pEncoderPositionLeft)--;
    }
    else {
        (*_pEncoderPositionLeft)++;
    }
}

void encoderRightA()
{
    if ( ! digitalRead( _pinRightEncoderXor ) ) {
        (*_pEncoderPositionRight)--;
    }
    else {
        (*_pEncoderPositionRight)++;
    }
}

void encoderLeftB()
{
    if ( digitalRead( _pinLeftEncoderXor ) ) {
        (*_pEncoderPositionLeft)--;
    }
    else {
        (*_pEncoderPositionLeft)++;
    }
}

void encoderRightB()
{
    if ( digitalRead( _pinRightEncoderXor ) ) {
        (*_pEncoderPositionRight)--;
    }
    else {
        (*_pEncoderPositionRight)++;
    }
}

#else
void encoderLeftA()
{
    // this assumes A & B are connected to the same bit on ports D & B.  A (int0) is PD2, B is PB2
    // compare the two channels using XOR, mask to the appropriate bit (2)
    if ( (PIND ^ PINB) & 0x04 ) {
        (*_pEncoderPositionLeft)--;
    }
    else {
        (*_pEncoderPositionLeft)++;
    }
}

void encoderRightA()
{
    // this assumes A & B are connected to the same bit on ports D & B.  A (int1) is PD3, B is PB3
    // compare the two channels using XOR, mask to the appropriate bit (3)
    if ( (PIND ^ PINB) & 0x08 ) {
        (*_pEncoderPositionRight)--;
    }
    else {
        (*_pEncoderPositionRight)++;
    }
}
#endif
