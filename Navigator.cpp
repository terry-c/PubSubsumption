/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <Navigator.h>

Navigator::Navigator( CommandDispatcher* pCD, Position* pOd, WaypointManager* pWM ) : Actor( pCD )
{
    _pPosition = pOd;
    _pWaypointManager = pWM;

    _pName = "Navigator";

    // we need to subscribe to events from the two Publishers
    SubscribeTo( pCD, 'N' );

    // get our initial waypoint
    _waypointNumber = 0;
    _pCurrentWaypoint = _pWaypointManager->GetWaypoint( _waypointNumber );

    _eState = eNormal;
    _bCorrecting = false;
//    _bAtDestination = false;

    _headingTolerance = 2.0 * PI / 180;   // 5°, in radians

    _brakingFactor = 0.25;    // for heading adjustments, slow one side by this factor.
}



void Navigator::handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams )
{
    float   headingToWaypoint;
    float   headingError;
    int     distanceToWaypoint;

    if ( _bEnabled ) {

        // now, if this event has not already been subsumed, we need to plot a course
        // from our current position to our current waypoint, if any.
        if ( ! pControlParams->ActorInControl() ) {
            // adjust the motors' speeds as necessary to correct our heading

            headingToWaypoint = _pPosition->_theta;   // current heading in radians, in case we don't have a waypoint
                    
            if ( _pCurrentWaypoint ) {
                // compute distance to target
                float dx = _pCurrentWaypoint->_x - _pPosition->_xInches;
                float dy = _pCurrentWaypoint->_y - _pPosition->_yInches;
                distanceToWaypoint = sqrt( dx * dx + dy * dy );    // thank you, Mr. Pythagoras
                      
                // If we're close enough to this waypoint, move to the next
                if ( distanceToWaypoint < _pCurrentWaypoint->_radius ) {
                    _pCurrentWaypoint = _pWaypointManager->GetWaypoint( ++_waypointNumber );
                    _bCorrecting = false;
                    IF_MSG( MM_PROGRESS ) {
                        Serial.println( F( "\nNext Waypoint\n" ) );
                    }
                    
                    if ( !_pCurrentWaypoint ) {

                        // no further waypoints, so shut down and take control
                        IF_MSG( MM_PROGRESS ) {
                            Serial.println( F( "\nWe have arrived!\n" ) );
                        }

                        pControlParams->SetThrottles( 0, 0 );
                        pControlParams->ControlledBy( this );
                        _waypointNumber = 0;
                    }

                }
                else {

                    // compute heading to current waypoint
                    // note that atan2() calls for dy/dx, but that yields angles referenced to the
                    // x-axis, or 0 = East.  For navigation, we want 0 = North, so we swap the
                    // arguments to get the correct alignment.
                    headingToWaypoint = atan2( dx, dy );

                    IF_MSG( MM_CALC ) {
                        PRINT_VAR( dx );
                        PRINT_VAR( dy );
                        PRINT_VAR( headingToWaypoint );
                    }

                    headingError = _pPosition->_theta - headingToWaypoint;
                    IF_MSG( MM_CALC ) {
                        PRINT_VAR( headingError );
                    }
                    // normalize the error value
                    float piOffset = headingError < 0.0 ? -PI : PI;
                    headingError = fmod( headingError + piOffset, 2.0 * PI ) - piOffset;
//                    headingError = atan( tan( headingError ) );
                    IF_MSG( MM_CALC ) {
                        Serial.print( F("Adjusted ") );
                        PRINT_VAR( headingError );
                    }

                    // if heading is outside our tolerance band, perform correction
                    if ( fabs( headingError ) > _headingTolerance ) {

                        // _bCorrecting means we already have a current snapshot
                        if ( ! _bCorrecting ) { 
                            _bCorrecting = true;
                            // snapshot current throttle positions as a baseline
                            _leftThrottleSnapshot = pControlParams->GetLeftThrottle();
                            _rightThrottleSnapshot = pControlParams->GetRightThrottle();
                            IF_MSG( MM_CALC ) {
                                PRINT_VAR( _leftThrottleSnapshot );
                                PRINT_VAR( _rightThrottleSnapshot );
                            }
                        }

                        // negative error means too far left, so slow the right motor
                        if ( headingError < 0 ) {
                            // map error (0..3) to throttle ( rightsnapshot .. -leftsnapshot )
                            int rightThrottle = fmap( -headingError, 0.0, 3.14, _rightThrottleSnapshot, -_leftThrottleSnapshot );
                            pControlParams->SetThrottles( _leftThrottleSnapshot, rightThrottle  );
                            IF_MSG( MM_CALC ) {
                                PRINT_VAR( rightThrottle );
                            }
                        }
                        else {
                            int leftThrottle = fmap( headingError, 0.0, 3.14, _leftThrottleSnapshot, -_rightThrottleSnapshot );
                            pControlParams->SetThrottles( leftThrottle, _rightThrottleSnapshot );
                            IF_MSG( MM_CALC ) {
                                PRINT_VAR( leftThrottle );
                            }
                        }
                        pControlParams->ControlledBy( this );
                    }
                    else {
                        // on course
                        _bCorrecting = false;
                    }
                }
            }
            else {
                // no waypoints, so shut down and take control

                IF_MSG( MM_PROGRESS ) {
                    Serial.println( F( "Destination reached." ) );
                }
                pControlParams->SetThrottles( 0, 0 );
                pControlParams->ControlledBy( this );
                _waypointNumber = 0;
            }
        }
        else { // subsumed
            _bCorrecting = false;
        }
    }

    IF_CSV( MM_CSVBASIC ) {
        CSV_OUT( _waypointNumber );
#ifdef USE_CSV
        if ( _pCurrentWaypoint || ( pControlParams->PrintingCsvHeadings() ) ) {
            CSV_OUT( _pCurrentWaypoint->_x );
            CSV_OUT( _pCurrentWaypoint->_y );
        }
        else {  // hard-code dummy output if no waypoint*
            Serial.print( F( "-1\t-1\t" ) );
        }
#endif
        CSV_OUT( distanceToWaypoint );
        CSV_OUT( headingToWaypoint );
        CSV_OUT( headingError );
        CSV_OUT( _headingTolerance );
    }
}


void Navigator::handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs )
{
    switch( pArgs->inputBuffer[1] ) {
        case 0 : // no subcommand
            break;
        case 'A' : {
            float dx = pArgs->fParams[ 0 ];

            PRINT_VAR( fmod( dx + PI, 2 * PI ) - PI );
            PRINT_VAR( fmod( dx - PI, 2 * PI ) + PI );

            PRINT_VAR( atan( tan( dx ) ) );

            break; }
        case 'R' : // restart
            _waypointNumber = 0;
            _pCurrentWaypoint = _pWaypointManager->GetWaypoint( _waypointNumber );
            if ( _messageMask & MM_RESPONSES ) {
                Serial.println( F( "Navigator restarting at first waypoint." ) );
                _waypointNumber = 0;
            }
            break;
        case 'T' : // set heading tolerance (dead-band) in degrees
            _headingTolerance = pArgs->fParams[ 0 ];
            if ( _messageMask & MM_RESPONSES ) {
                Serial.print( F( "Navigator heading tolerance set to (degrees): " ) );
                Serial.println( _headingTolerance );
            }
            break;
    }
}


void Navigator::PrintHelp() 
{
    Actor::PrintHelp();
    Serial.println( F(  "  T <degrees> : set heading tolerance\n"
                        "  R : restart at first waypoint" 
                        ) );
}

