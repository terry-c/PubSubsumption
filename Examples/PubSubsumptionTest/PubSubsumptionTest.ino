/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <CollisionAvoidance.h>
#include "CommonDefs.h"
#include <CommandDispatcher.h>
#include <CommandSubscriber.h>
#include <Actor.h>
#include <CruiseControl.h>
#include <WaypointManager.h>
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

uint8_t _encoderLeftPinA = 49;
uint8_t _encoderLeftPinB = 47;
uint8_t _encoderRightPinA = 53;
uint8_t _encoderRightPinB = 51;

// our publishers
CommandDispatcher   dispatcher;
Director            director( &dispatcher, 1000 );    // 1000 ms interval

// other entities
WaypointManager     waypointManager( &dispatcher );
Position            position( &dispatcher, &director, _pEncoderPositionLeft, _pEncoderPositionRight, 123.45, 7.25, 12.34 );     // this carries the current positions of all motors.

// Actors.  These are objects which participate in the Subsumption chain.
// They will be subscribed to Director command events in setup().
//
// LEDDriver is the last Behavior, consuming any adjustments made by higher-priority behaviors.
// LEDDriver is a motor simulator
#ifdef USE_LED_EMULATOR
LEDDriver          led( 9, 6, 5, 3, &dispatcher, &position );
#else
MotorDriver          motor( 2, 3, 4, 5, 8, 9, 10, 11, &dispatcher, &position );
#endif

// Navigator keeps us on course toward the next waypoint
Navigator           navigator(&dispatcher, &position, &waypointManager);

// CollisionRecovery responds to bumping into things.
CollisionRecovery   bumper( &dispatcher, 0, 0 );

// CruiseControl maintains the current heading and speed
CruiseControl       cruise( &dispatcher, &position, 0.1234 ); // last param is a factor for converting from IPS to encoder ticks per interval

void setup()
{
    Serial.begin(115200);

    pinMode( _encoderLeftPinA , INPUT );
    pinMode( _encoderLeftPinB , INPUT );
    pinMode( _encoderRightPinA, INPUT );
    pinMode( _encoderRightPinB, INPUT );

    pinMode( 39, OUTPUT );
    digitalWrite( 39, LOW );

    attachInterrupt( _encoderLeftPinA , encoderLeftA, RISING );
//    attachInterrupt( _encoderLeftPinB , encoderLeftA, RISING );
    attachInterrupt( _encoderRightPinA, encoderRightA, RISING );
//    attachInterrupt( _encoderRightPinB, encoderRightA, RISING );

    /*
    char* line = "--------------------";

    Serial.println( line );
    Serial.println( line );
    Serial.println( F( "Subsumption Robot" ) );
    Serial.println( line );
    Serial.print( F( "Director:\t" ) );
    Serial.println( (int) &director );

    Serial.print( F( "Dispatcher:\t" ) );
    Serial.println( (int) &dispatcher );

    Serial.print( F( "Waypoints:\t" ) );
    Serial.println( (int) &waypointManager );
    
    Serial.println( F( "\nDefined Actors:" ) );
    Serial.println( line );
    Serial.print( navigator.GetName() );
    Serial.print( "\t" );
    Serial.println( (int) &navigator );

    Serial.print( motor.GetName() );
    Serial.print( '\t' );
    Serial.println( (int) &motor );
    
    Serial.print( bumper.GetName() );
    Serial.print( '\t' );
    Serial.println( (int) &bumper );
    */

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

void encoderLeftA()
{
        digitalWrite( 39, LOW );

    bool bPinA = digitalRead( _encoderLeftPinA );
    bool bPinB = digitalRead( _encoderLeftPinB );

    if ( bPinA == bPinB ) {
        (*_pEncoderPositionLeft)--;
    }
    else {
        (*_pEncoderPositionLeft)++;
    }
}

void encoderRightA()
{
        digitalWrite( 39, HIGH );

    bool bPinA = digitalRead( _encoderRightPinA );
    bool bPinB = digitalRead( _encoderRightPinB );

    if ( bPinA == bPinB ) {
        (*_pEncoderPositionRight)--;
    }
    else {
        (*_pEncoderPositionRight)++;
    }
}
