/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <Navigator.h>

Navigator::Navigator( CommandDispatcher* pCD, Position* pOd ) : Actor( pCD )
{
    _pPosition = pOd;

    _pName = "Navigator";

    // we need to subscribe to events from the two Publishers
    SubscribeTo( pCD, 'N' );

    _eState = eNormal;
}



void Navigator::handleControlEvent( EventNotification* pEvent, MotorParams* pMotorParams )
{
    // here's where we do our Subsumption Behavior

    // position computation has been moved into the new Position class, fka Position

    // now, if this event has not already been subsumed, we need to plot a course
    // from our current position to our target.
    if ( _bEnabled ) {

        if ( _verbosityLevel >= 3 ) {
            PRINT_VAR( _pPosition->_leftInches );
            PRINT_VAR( _pPosition->_rightInches );
            PRINT_VAR( _pPosition->_distanceInches );
            PRINT_VAR( _pPosition->_theta );
        }
        if ( _verbosityLevel >= 2 ) {
            PRINT_VAR( _pPosition->_xInches );
            PRINT_VAR( _pPosition->_yInches );
            PRINT_VAR( _pPosition->_headingDegrees );
        }

        if ( NULL == pMotorParams->_pTakenBy ) {
            switch ( _eState ) {  // ballistic behavior
                case eNormal :
                    // adjust the motors' speeds as necessary to correct our heading

                    // if we changed anything, set the subsumption flag.
                    // pMotorParams->_pTakenBy = this;
                    break;

                case eManual :
                    pMotorParams->_throttleLeft = _leftMotorSpeed;
                    pMotorParams->_throttleRight = _rightMotorSpeed;
                    pMotorParams->_pTakenBy = this;
                    break;

                case eManualEnd :
                    _eState = eNormal;
                    pMotorParams->_throttleLeft = 0;
                    pMotorParams->_throttleRight = 0;
                    pMotorParams->_pTakenBy = this;
                    break;
                case eTurning :
                    break;
            }
        }
    }
}


void Navigator::handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs )
{
    switch( pArgs->inputBuffer[1] ) {
        case 0 : // no subcommand
            break;
        case 'G' : // forward, 2 params
            _leftMotorSpeed = pArgs->nParams[0];
            _rightMotorSpeed = pArgs->nParams[1];
            _eState = eManual;
            if ( _verbosityLevel >= 1 ) {
                Serial.print( F( "Navigator speeds set to " ) );
                Serial.print( _leftMotorSpeed ); Serial.print( '\t' );
                Serial.println( _rightMotorSpeed );
            }
            break;
        case 'S' : // stop
            _leftMotorSpeed = 0;
            _rightMotorSpeed = 0;
            _eState = eManualEnd;
            if ( _verbosityLevel >= 1 ) {
                Serial.print( F( "Navigator resuming automatic control." ) );
            }
            break;
        case 'T' : // turn right or left n degrees at radius r
            _targetHeading += pArgs->nParams[1];
            _turnRadius = pArgs->nParams[2];
            _eState = eTurning;
            if ( _verbosityLevel >= 1 ) {
                Serial.print( F( "Navigator turning to heading " ) );
                Serial.print( _targetHeading );
            }
            break;
        case 'H' : // turn to heading at radius r
            _targetHeading = pArgs->nParams[1];
            _turnRadius = pArgs->nParams[2];
            _eState = eTurning;
            if ( _verbosityLevel >= 1 ) {
                Serial.print( F( "Navigator turning to heading " ) );
                Serial.print( _targetHeading );
            }
            break;
    }
}


void Navigator::PrintHelp( uint8_t eventID ) 
{
    // we only handle one event, the "L" command:
    Serial.println( F( "\nNavigator Control:" ) );
    Actor::PrintHelp( 'N' );
    Serial.println( F(  "  NT <+/- degrees> <radius> : Turn right or left, radius multiplier\n"
                        "  NH <heading> <radius> : turn to heading" ) );
}

