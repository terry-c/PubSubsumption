/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <CruiseControl.h>

CruiseControl::CruiseControl( CommandDispatcher* pCD, Position* pOD ) : Actor( pCD ) 
{
    _pPosition = pOD;

    _kP = 1.0;
    _kI = 0.5;
    _kD = 0.0;

    _pName = F("Cruise Control");
    _pHelpString =  F(  "  S <Speed> : Set cruising speed (IPS)\n"
                        "  P <kP> <kI> <kD> : Set PID coefficients" 
                        );

    _bCruising = false;

    SubscribeTo( pCD, 'C' );    // All our commands begin with "C"
}


// if we were not already cruising, but have just assumed control (i.e., no other
// behavior has asserted itself this interval), note the Position readings so 
// we can maintain our heading by keeping the same differential.
void CruiseControl::handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams )
{
    // nothing to do if we're not enabled
    if ( _bEnabled ) {
        ControlParams* pControlParams = (ControlParams*) pEvent->pData;

        if ( NULL != pControlParams->ActorInControl() ) {
            _bCruising = false;
        }
        else {  // nobody else cares, so it's our turn
            float targetSpeedInchesPerInterval = ( _targetSpeedIPS * pControlParams->GetInterval() ) / 1000;

            if ( _bCruising ) {    // this means we were already cruising
                // check our position and calculate error values

                // delta is how far we have moved in this interval
                float deltaLeft  = _pPosition->_leftInches  - _prevPositionLeft;
                float deltaRight = _pPosition->_rightInches - _prevPositionRight;

                // error is the difference between how far we expected to move and how far we actually moved.
                float errorInchesLeft  = targetSpeedInchesPerInterval - deltaLeft;
                float errorInchesRight = targetSpeedInchesPerInterval - deltaRight;

                // Derivative uses the change in error between the last two intervals
                float deltaErrorLeft  = _prevErrorLeft  - errorInchesLeft;
                float deltaErrorRight = _prevErrorRight - errorInchesRight;

                // Integral term uses error in absolute position
                float cumulativeErrorLeft  = _idealPositionLeft  - _pPosition->_leftInches;
                float cumulativeErrorRight = _idealPositionRight - _pPosition->_rightInches;

                // calculate new throttle positions using PID
                _throttleLeft  += ( ( _kP * errorInchesLeft  ) + ( _kI * cumulativeErrorLeft  ) + ( _kD * deltaErrorLeft ) );
                _throttleRight += ( ( _kP * errorInchesRight ) + ( _kI * cumulativeErrorRight ) + ( _kD * deltaErrorRight ) );

                // Show our work
                if ( _messageMask & MM_PROGRESS ) {
                    Serial.println( F( "\nCruise Control PID calc:" ) );
                    PRINT_VAR( _idealPositionLeft );
                    PRINT_VAR( _pPosition->_leftInches );
                    PRINT_VAR( _prevPositionLeft );
                    PRINT_VAR( deltaLeft );
                    PRINT_VAR( targetSpeedInchesPerInterval );
                    PRINT_VAR( errorInchesLeft );
                    PRINT_VAR( cumulativeErrorLeft );
                    PRINT_VAR( _kI * cumulativeErrorLeft );
                }

                IF_CSV( MM_CSVBASIC ) {
                    CSV_OUT( targetSpeedInchesPerInterval );                      
                    CSV_OUT( _idealPositionLeft );                     
                    CSV_OUT( _prevPositionLeft );                      
                    CSV_OUT( deltaLeft );                              
                    CSV_OUT( errorInchesLeft );                              
                    CSV_OUT( cumulativeErrorLeft );                    
                    CSV_OUT( _kI * cumulativeErrorLeft );              
                    CSV_OUT( _idealPositionRight );                     
                    CSV_OUT( _prevPositionRight );                      
                    CSV_OUT( deltaRight );                              
                    CSV_OUT( errorInchesRight );                              
                    CSV_OUT( cumulativeErrorRight );                    
                    CSV_OUT( _kI * cumulativeErrorRight );              
                }

                // Upate some numbers for the next time
                _prevErrorLeft = errorInchesLeft;
                _prevErrorRight = errorInchesRight;

                _prevPositionLeft = _pPosition->_leftInches;
                _prevPositionRight = _pPosition->_rightInches;
            }
            else {  // we just took control, so let's cruise!
                _bCruising = true;

                // set up for 0 errors next pass
                _prevPositionLeft = _idealPositionLeft = _pPosition->_leftInches;
                _prevPositionRight = _idealPositionRight = _pPosition->_rightInches;
            }

            // set the next ideal target positions
            _idealPositionLeft += targetSpeedInchesPerInterval;
            _idealPositionRight += targetSpeedInchesPerInterval;

            // set the throttle positions.
            pControlParams->SetThrottles( _throttleLeft, _throttleRight, this );
        }
    }
}


void CruiseControl::handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs )
{
    CommandArgs* pData = (CommandArgs*) pEvent->pData;

    // check the sub-command.
    switch( pData->inputBuffer[1] ) {
        case 'S' : // set target speed in IPS
            // Set the crusing speed from the command argument
            // Speed is in IPS, calculate encoder ticks per interval and use this as the target speed
            _targetSpeedIPS = pData->fParams[ 0 ];

            if ( _messageMask & MM_RESPONSES ) {
                Serial.print( F( "\nCruise Speed set to " ) );
                Serial.print( _targetSpeedIPS );
                Serial.println( F( " IPS" ) );
            }
            break;
        case 'P' : // Set PID parameters
            _kP = pData->fParams[ 0 ];
            _kI = pData->fParams[ 1 ];
            _kD = pData->fParams[ 2 ];
            if ( _messageMask & MM_RESPONSES ) {
                Serial.println( F( "P\tI\tD" ) );
                Serial.print( _kP ); Serial.print( '\t' );
                Serial.print( _kI ); Serial.print( '\t' );
                Serial.println( _kD );
            }
            break;
        }
}


void CruiseControl::PrintSpecificParameterValues()
{
    Serial.print( F( " Cruising Speed (IPS): " ) );
    Serial.println( _targetSpeedIPS );

    Serial.print( F( " PID:\t" ) );
    Serial.print( _kP );
    Serial.print( '\t' );
    Serial.print( _kI );
    Serial.print( '\t' );
    Serial.println( _kD );
}
