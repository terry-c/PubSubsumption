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
        float ticksPerInch, float wheelSpacing ) : Behavior( pCD )
{
    leftPosition = &_currentEncoderPositionLeft;
    rightPosition = &_currentEncoderPositionRight;

    _pName = F("Position");
    _bCanBeDisabled = false;
    _pHelpString = F(  "  V <level> : Set verbosity level\n" 
                        "  R 9 : Reset position to zero" 
                        );

    SubscribeTo( pCD, 'P' );

    // compute conversion factors
    _EncoderTicksPerInch = ticksPerInch;
    _WheelSpacingInches = wheelSpacing;
}


void Position::handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs )
{
    switch( pArgs->inputBuffer[1] ) {
        case 'R' : // Reset to 0,0
            if ( pArgs->nParams[ 0 ] == 9 ) {
                if ( _messageMask & MM_RESPONSES ) {
                    Serial.println( F( "Position reset to zero" ) );
                }
                
                _currentEncoderPositionLeft = 0;
                _currentEncoderPositionRight = 0;
            }
            else {
                Serial.println( F( "Enter \"PR 9\" to reset" ) );
            }
            break;
    }
}


void Position::handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams )
{
    // first, we compute our current position (x, y, theta)
    float dLeftInches = _currentEncoderPositionLeft / _EncoderTicksPerInch - _leftInches;
    float dRightInches = _currentEncoderPositionRight / _EncoderTicksPerInch - _rightInches;
    float dDistanceInches = (dLeftInches + dRightInches) / 2.0;

    _leftInches      += dLeftInches;
    _rightInches     += dRightInches;
    _distanceInches  += dDistanceInches;

    _theta           = (_leftInches - _rightInches) / _WheelSpacingInches;
    _xInches         += dDistanceInches * sin( _theta );
    _yInches         += dDistanceInches * cos( _theta );
    _headingDegrees  = _theta * (180.0 / PI);

    IF_MSG( MM_PROGRESS ) {
        PRINT_VAR( _currentEncoderPositionLeft );
        PRINT_VAR( _currentEncoderPositionRight );
        PRINT_VAR( _leftInches     );
        PRINT_VAR( _rightInches    );
        PRINT_VAR( _distanceInches );
        PRINT_VAR( _theta          );
        PRINT_VAR( _xInches        );
        PRINT_VAR( _yInches        );
        PRINT_VAR( _headingDegrees );
    }

    IF_CSV( MM_CSVBASIC ) {
        CSV_OUT( _leftInches     );
        CSV_OUT( _rightInches    );
        CSV_OUT( _distanceInches );
        CSV_OUT( _theta          );
        CSV_OUT( _xInches        );
        CSV_OUT( _yInches        );
        CSV_OUT( _headingDegrees );
    }
}
