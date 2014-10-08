/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <CruiseControl.h>

CruiseControl::CruiseControl( CommandDispatcher* pCD, Position* pOD, float IPS2Ticks ) : Actor( pCD ) 
{
    _pPosition = pOD;
    _IPS2Ticks = IPS2Ticks;

    _kP = 1.0;
    _kI = 0.5;
    _kD = 0.0;

    _pName = "Cruise Control";
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
            pControlParams->ControlledBy( this );

            if ( _bCruising ) {    // this means we were already cruising
                // check our position and calculate error values

                // delta is how far we have moved in this interval
                int deltaLeft = _pPosition->_currentEncoderPositionLeft - _prevPositionLeft;
                int deltaRight = _pPosition->_currentEncoderPositionRight - _prevPositionRight;

                // error is the difference between how far we expected to move and how far we actually moved.
                int errorLeft = _targetSpeedTicks - deltaLeft;
                int errorRight = _targetSpeedTicks - deltaRight;

                // Derivative uses the change in error between the last two intervals
                int deltaErrorLeft = _prevErrorLeft - errorLeft;
                int deltaErrorRight = _prevErrorRight - errorRight;

                // Integral term uses error in absolute position
                int cumulativeErrorLeft = _idealPositionLeft - _pPosition->_currentEncoderPositionLeft;
                int cumulativeErrorRight = _idealPositionRight - _pPosition->_currentEncoderPositionRight;

                // calculate new throttle positions using PID
                _throttleLeft  += ( ( _kP * errorLeft  ) + ( _kI * cumulativeErrorLeft  ) + ( _kD * deltaErrorLeft ) );
                _throttleRight += ( ( _kP * errorRight ) + ( _kI * cumulativeErrorRight ) + ( _kD * deltaErrorRight ) );

                // Show our work
                if ( _messageMask & MM_PROGRESS ) {
                    Serial.println( F( "\nCruise Control PID calc:" ) );
                    PRINT_VAR( _idealPositionLeft );
                    PRINT_VAR( _pPosition->_currentEncoderPositionLeft );
                    PRINT_VAR( _prevPositionLeft );
                    PRINT_VAR( deltaLeft );
                    PRINT_VAR( _targetSpeedTicks );
                    PRINT_VAR( errorLeft );
                    PRINT_VAR( cumulativeErrorLeft );
                    PRINT_VAR( _kI * cumulativeErrorLeft );
                }

/*
                if ( _messageMask & MM_CSVBASIC && pControlParams->_csvState != ControlParams::eCsvIdle ) {
                    if ( pControlParams->PrintingCsvHeadings() ) {
                        Serial.print( F("_idealPositionLeft,_pPosition->_currentEncoderPositionLeft,_prevPositionLeft,deltaLeft,_targetSpeedTicks,errorLeft,cumulativeErrorLeft,_kI * cumulativeErrorLeft,") );
                    }
                    else {
                        Serial.print( _idealPositionLeft );                        Serial.print( pControlParams->_csvDelimiter );
                        Serial.print( _pPosition->_currentEncoderPositionLeft );   Serial.print( pControlParams->_csvDelimiter );
                        Serial.print( _prevPositionLeft );                         Serial.print( pControlParams->_csvDelimiter );
                        Serial.print( deltaLeft );                                 Serial.print( pControlParams->_csvDelimiter );
                        Serial.print( _targetSpeedTicks );                         Serial.print( pControlParams->_csvDelimiter );
                        Serial.print( errorLeft );                                 Serial.print( pControlParams->_csvDelimiter );
                        Serial.print( cumulativeErrorLeft );                       Serial.print( pControlParams->_csvDelimiter );
                        Serial.print( _kI * cumulativeErrorLeft );                 Serial.print( pControlParams->_csvDelimiter );
                   }
                }
*/
                // Upsate some numbers for the next time
                _prevErrorLeft = errorLeft;
                _prevErrorRight = errorRight;

                _prevPositionLeft = _pPosition->_currentEncoderPositionLeft;
                _prevPositionRight = _pPosition->_currentEncoderPositionRight;
            }
            else {  // we just took control, so let's cruise!
                _bCruising = true;

                // set up for 0 errors next pass
                _prevPositionLeft = _idealPositionLeft = _pPosition->_currentEncoderPositionLeft;
                _prevPositionRight = _idealPositionRight = _pPosition->_currentEncoderPositionRight;
            }

            // set the next ideal target positions
            _idealPositionLeft += _targetSpeedTicks;
            _idealPositionRight += _targetSpeedTicks;

            // set the throttle positions.
            pControlParams->SetThrottles( _throttleLeft, _throttleRight );

            //Serial.print( F( "Throttles set to: " ) );
            //Serial.print( pControlParams->_throttleLeft );
            //Serial.print( '/' );
            //Serial.println( pControlParams->_throttleRight );
        }
    }
    IF_CSV( MM_CSVBASIC ) {
        CSV_OUT( _targetSpeedTicks );                      
        CSV_OUT( _idealPositionLeft );                     
        CSV_OUT( _prevPositionLeft );                      
//        CSV_OUT( deltaLeft );                              
//        CSV_OUT( errorLeft );                              
//        CSV_OUT( cumulativeErrorLeft );                    
//        CSV_OUT( _kI * cumulativeErrorLeft );              
        CSV_OUT( _idealPositionRight );                     
        CSV_OUT( _prevPositionRight );                      
//        CSV_OUT( deltaRight );                              
//        CSV_OUT( errorRight );                              
//        CSV_OUT( cumulativeErrorRight );                    
//        CSV_OUT( _kI * cumulativeErrorRight );              
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
            _targetSpeedTicks = _IPS2Ticks * _targetSpeedIPS;
            if ( _messageMask & MM_RESPONSES ) {
                Serial.println( F( "\nCruise Speed set to:" ) );
                Serial.print( F( " IPS:\t" ) );
                Serial.println( _targetSpeedIPS );
                Serial.print( F( " Ticks:\t" ) );
                Serial.println( _targetSpeedTicks );
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


void CruiseControl::PrintHelp( uint8_t eventID ) 
{
    // we only handle one event, the "L" command:
    Serial.println( F( "\nCruise Control:" ) );
    Actor::PrintHelp( 'C' );
    Serial.println( F(  "  CS <Speed> : Set cruising speed (IPS)\n"
                        "  CP <kP> <kI> <kD> : Set PID coefficients" 
                        ) );
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
