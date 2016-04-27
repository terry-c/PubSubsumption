/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <Navigator.h>

Navigator::Navigator( CommandDispatcher* pCD, Position* pOd, WaypointManager* pWM ) : Behavior( pCD )
{
    _pPosition = pOd;
    _pWaypointManager = pWM;

    _pName = F("Navigator");
    _pHelpString = F(  "  T <degrees> : set heading tolerance\n"
                        "  R : restart at first waypoint" 
                        );

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



void Navigator::handleSubsumptionEvent( EventNotification* pEvent, SubsumptionParams* pSubsumptionParams )
{
    float   headingToWaypoint;
    float   headingError;
    int     distanceToWaypoint;

    if ( _bEnabled ) {

        // now, if this event has not already been subsumed, we need to plot a course
        // from our current position to our current waypoint, if any.
        if ( ! pSubsumptionParams->ControlFreak() ) {
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
                    PROGRESS_MSG( "\nNext Waypoint\n" );
                    
                    if ( !_pCurrentWaypoint ) {

                        // no further waypoints, so shut down and take control
                        PROGRESS_MSG( "\nWe have arrived!\n" );

                        pSubsumptionParams->SetThrottles( 0, 0, this);
                        _waypointNumber = 0;
                    }

                }
                else {

                    // compute heading to current waypoint
                    // note that atan2() calls for dy/dx, but that yields angles referenced to the
                    // x-axis, or 0 = East.  For navigation, we want 0 = North, so we swap the
                    // arguments to get the correct alignment.
                    headingToWaypoint = atan2( dx, dy );

                    IF_MASK( MM_CALC ) {
                        PRINT_VAR( dx );
                        PRINT_VAR( dy );
                        PRINT_VAR( headingToWaypoint );
                    }

                    headingError = _pPosition->_theta - headingToWaypoint;
                    IF_MASK( MM_CALC ) {
                        PRINT_VAR( headingError );
                    }
                    // normalize the error value
                    float piOffset = headingError < 0.0 ? -PI : PI;
                    headingError = fmod( headingError + piOffset, 2.0 * PI ) - PI;
//                    headingError = atan( tan( headingError ) );
                    IF_MASK( MM_CALC ) {
                        Serial.print( F("Adjusted ") );
                        PRINT_VAR( headingError );
                    }

                    // if heading is outside our tolerance band, perform correction
                    if ( fabs( headingError ) > _headingTolerance ) {

                        // _bCorrecting means we already have a current snapshot
                        if ( ! _bCorrecting ) { 
                            _bCorrecting = true;
                            // snapshot current throttle positions as a baseline
                            _leftThrottleSnapshot = pSubsumptionParams->GetLeftThrottle();
                            _rightThrottleSnapshot = pSubsumptionParams->GetRightThrottle();
                            IF_MASK( MM_CALC ) {
                                PRINT_VAR( _leftThrottleSnapshot );
                                PRINT_VAR( _rightThrottleSnapshot );
                            }
                        }

                        // negative error means too far left, so slow the right motor
                        if ( headingError < 0 ) {
                            // map error (0..3) to throttle ( rightsnapshot .. -leftsnapshot )
                            int rightThrottle = fmap( -headingError, 0.0, 3.14, _rightThrottleSnapshot, -_leftThrottleSnapshot );
                            pSubsumptionParams->SetThrottles( _leftThrottleSnapshot, rightThrottle , this);
                            IF_MASK( MM_CALC ) {
                                PRINT_VAR( rightThrottle );
                            }
                        }
                        else {
                            int leftThrottle = fmap( headingError, 0.0, 3.14, _leftThrottleSnapshot, -_rightThrottleSnapshot );
                            pSubsumptionParams->SetThrottles( leftThrottle, _rightThrottleSnapshot, this);
                            IF_MASK( MM_CALC ) {
                                PRINT_VAR( leftThrottle );
                            }
                        }
                    }
                    else {
                        // on course
                        _bCorrecting = false;
                    }
                }
            }
            else {
                // no waypoints, so shut down and take control

                PROGRESS_MSG( "Destination reached." );
                
                pSubsumptionParams->SetThrottles( 0, 0, this);
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
        if ( _pCurrentWaypoint || ( pSubsumptionParams->PrintingCsvHeadings() ) ) {
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

void Navigator::PrintSpecificParameterValues()
{
    Serial.print( F( " Current Waypoint: " ) );
    Serial.println( _waypointNumber );

    Serial.print( F( " Heading tolerance: " ) );
    Serial.println( _headingTolerance );

}
