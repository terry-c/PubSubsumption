/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <Position.h>

Position::Position( CommandDispatcher* pCD, Director* pD, 
        uint32_t*& leftPosition, uint32_t*& rightPosition,  // pointers to _currentEncoderPositionLeft and _currentEncoderPositionRight
        float ticksPerRev, float wheelDiameter, float wheelSpacing ) : Actor( pCD )
{
    leftPosition = &_currentEncoderPositionLeft;
    rightPosition = &_currentEncoderPositionRight;

    _pName = "Position";

    SubscribeTo( pCD, 'P' );

    // compute conversion factors
    // ticks per inch
}


void Position::handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs )
{
    switch( pArgs->inputBuffer[1] ) {
        case 'R' : // no subcommand
            if ( pArgs->nParams[ 0 ] == 123 && pArgs->nParams[ 1 ] == 456 ) {
                if ( _verbosityLevel >= 1 ) {
                    Serial.println( F( "Position reset to zero" ) );
                }
            }
            else {
                Serial.println( F( "Wrong code, position NOT reset" ) );
            }
            break;
    }
}

/// todo: make these member variables passed into ctor
#define EncoderTicksPerInch 99.44
#define WheelSpacingInches 10.0

void Position::handleControlEvent( EventNotification* pEvent, MotorParams* pMotorParams )
{
    // this is temporary, for testing
    if ( _verbosityLevel > 1 ) {
        Serial.println( _pName );
    }

    // first, we compute our current position (x, y, theta)
    _leftInches      = _currentEncoderPositionLeft / EncoderTicksPerInch;
    _rightInches     = _currentEncoderPositionRight / EncoderTicksPerInch;
    _distanceInches  = (_leftInches + _rightInches) / 2.0;
    _theta           = (_leftInches - _rightInches) / WheelSpacingInches;
    _xInches         = _distanceInches * sin( _theta );
    _yInches         = _distanceInches * cos( _theta );
    _headingDegrees  = _theta * (180.0 / PI);


}


void Position::PrintHelp( uint8_t eventID ) 
{
    // we only handle one event, the "L" command:
    Serial.println( F( "\nPosition Control:" ) );
//    Actor::PrintHelp( eventID );  // we don't handle any of the common Actor commands (0,1,
    Serial.println( F( "  PV <level> : Set verbosity level" ) );
    Serial.println( F( "  PR 123 456 : Reset position to zero" ) );
}
