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
uint32_t* pEncoderPositionLeft;
uint32_t* pEncoderPositionRight;

// our publishers
CommandDispatcher   dispatcher;
Director            director( &dispatcher, 1000 );    // 1000 ms interval

// other entities
WaypointManager     waypointManager( &dispatcher );
Position            position( &dispatcher, &director, pEncoderPositionLeft, pEncoderPositionLeft, 123.45, 7.25, 12.34 );     // this carries the current positions of all motors.

// Actors.  These are objects which participate in the Subsumption chain.
// They will be subscribed to Director command events in setup().
//
// LEDDriver is the last Behavior, consuming any adjustments made by higher-priority behaviors.
// LEDDriver is a motor simulator
LEDDriver          led( 9, 6, 5, 3, &dispatcher, &position );
//MotorDriver          motor( 2, 3, 4, 5, 8, 9, 10, 11, &dispatcher, &Position );

// Navigator keeps us on course toward the next waypoint
Navigator           navigator(&dispatcher, &position);

// CollisionRecovery responds to bumping into things.
CollisionRecovery   bumper( &dispatcher, 0, 0 );

// CruiseControl maintains the current heading and speed
CruiseControl       cruise( &dispatcher, &position, 0.1234 ); // last param is a factor for converting from IPS to encoder ticks per interval

void setup()
{
    Serial.begin(115200);

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
//    motor.SubscribeTo( &director, 0 );    // this is first (last) because it's the endpoint.
    led.SubscribeTo( &director );
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
