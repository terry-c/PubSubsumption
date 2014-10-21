/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <CollisionRecovery.h>
#include <Director.h>

CollisionRecovery::CollisionRecovery( CommandDispatcher* pCD, uint8_t leftPin, uint8_t rightPin ) : Actor( pCD )
{
    _pName = "Crash Recover";

    // we need to subscribe to events from the two Publishers
    SubscribeTo( pCD, 'B' );

    _eState = eNormal;

    _bSimBumpLeft = false;
    _bSimBumpRight = false;

    // put some reasonable test values in the state arrays
    _StateTimes[ eNormal    ] = 0;
    _StateTimes[ eStopped   ] = 2;
    _StateTimes[ eReversing ] = 2;
    _StateTimes[ eTurning   ] = 2;
    _StateTimes[ eForward   ] = 2;

    _StateSpeeds[ eNormal    ] = 0;
    _StateSpeeds[ eStopped   ] = 0;
    _StateSpeeds[ eReversing ] = -5;
    _StateSpeeds[ eTurning   ] = -10;
    _StateSpeeds[ eForward   ] = 20;
}


void CollisionRecovery::handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams )
{
    if ( ! pControlParams->ActorInControl() ) {
        bool bInControl = true;
        int leftMotorSpeed = 0;
        int rightMotorSpeed = 0;
        leftMotorSpeed = _StateSpeeds[_eState];
        rightMotorSpeed = _StateSpeeds[_eState];
        switch ( _eState ) {  // ballistic behavior

            case eNormal :
                // check the bump sensors
                // if we've hit something, stop
                if ( _bSimBumpLeft || _bSimBumpRight ) {
                    leftMotorSpeed = 0;
                    rightMotorSpeed = 0;
                    _eState = eStopped;
                    _nStateTimer = _StateTimes[ _eState ] - 1;
                    if ( _messageMask & MM_PROGRESS ) {
                        Serial.println( F( "Stopped" ) );
                    }
                }
                else {
                    // nothing hit, pass for now . . .
                    bInControl = false;
                }
                break;

            case eStopped :
                if ( --_nStateTimer == 0 ) {
                    _eState = eReversing;
                    _nStateTimer = _StateTimes[ _eState ];
                    if ( _messageMask & MM_PROGRESS ) {
                        Serial.println( F( "Reverse" ) );
                    }
                }
                break;

            case eReversing :
                if ( --_nStateTimer == 0 ) {
                    _eState = eTurning;
                    _nStateTimer = _StateTimes[ _eState ];
                    if ( _messageMask & MM_PROGRESS ) {
                        Serial.println( F( "Turning" ) );
                    }
                }
                break;

            case eTurning :
                if ( _bSimBumpLeft ) {
                    leftMotorSpeed = _StateSpeeds[ _eState ] / 2;
                }
                else {
                    rightMotorSpeed = _StateSpeeds[ _eState ] / 2;
                }
                if ( --_nStateTimer == 0 ) {
                    _eState = eForward;
                    _nStateTimer = _StateTimes[ _eState ];
                    if ( _messageMask & MM_PROGRESS ) {
                        Serial.println( F( "Forward" ) );
                    }
                }
                break;

            case eForward :
                if ( --_nStateTimer == 0 ) {
                    _eState = eNormal;
                    _bSimBumpLeft = false;
                    _bSimBumpRight = false;
                    if ( _messageMask & MM_PROGRESS ) {
                        Serial.println( F( "Done" ) );
                    }
                }
                break;

        }
        if ( bInControl ) {
            pControlParams->ControlledBy( this );
            pControlParams->SetThrottles( leftMotorSpeed, rightMotorSpeed );
        }
    }
}


void CollisionRecovery::handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs )
{
    switch( pArgs->inputBuffer[1] ) {

        case 0 : // no subcommand
            break;
        
        case 'L' : // Simulated Bump left
            if ( _messageMask & MM_PROGRESS ) {
                Serial.println("Bump Left!");
            }
            _bSimBumpLeft = true;
            break;
        
        case 'R' : // Simulated Bump right
            if ( _messageMask & MM_PROGRESS ) {
                Serial.println("Bump Right!");
            }
            _bSimBumpRight = true;
            break;

        case 'S' : // Set recovery state speeds
            _StateSpeeds[0] = 0;
            for ( int ixArg = 0; ixArg <= 3; ixArg++ ) {
                _StateSpeeds[ixArg + 1] = pArgs->nParams[ixArg];
                if ( _messageMask & MM_RESPONSES ) {
                    Serial.print( "Bump speed " );
                    Serial.print( ixArg + 1 );
                    Serial.print( " set to " );
                    Serial.println( _StateSpeeds[ixArg + 1] );
                }
            }
            break;

        case 'T' : // Set recovery state times (interval counts)
            _StateTimes[0] = 0;
            for ( int ixArg = 0; ixArg <= 3; ixArg++ ) {
                _StateTimes[ixArg + 1] = pArgs->nParams[ixArg];
                if ( _messageMask & MM_RESPONSES ) {
                    Serial.print( "Bump time " );
                    Serial.print( ixArg + 1 );
                    Serial.print( " set to " );
                    Serial.println( _StateTimes[ixArg + 1] );
                }
            }
            break;
    }
}

void CollisionRecovery::PrintHelp() 
{ 
    Actor::PrintHelp();
    Serial.println( F(  "  L: Simulate bump left\n"
                        "  R: Simulate bump right\n"
                        "  S: <s1> <s2> <s3> <s4> Speeds (throttle) \n"
                        "  T: <t1> <t2> <t3> <t4> Times (ticks)" 
                        ) ); 
}


void CollisionRecovery::PrintSpecificParameterValues()
{
    Serial.println( F( "\t\tStop\tBack\tTurn\tFwd" ) );
    Serial.print( F( " Speeds:" ) );
    for ( int ix = 1; ix <= 4; ix++ ) {
        Serial.print( '\t' );
        Serial.print( _StateSpeeds[ ix ] );
    }
    Serial.println();

    Serial.print( F( " Times: " ) );
    for ( int ix = 1; ix <= 4; ix++ ) {
        Serial.print( '\t' );
        Serial.print( _StateTimes[ ix ] );
    }
}
