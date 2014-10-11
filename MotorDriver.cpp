/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <MotorDriver.h>


MotorDriver::MotorDriver( 
    uint8_t pwmPinLR, uint8_t dirPinLR,
    uint8_t pwmPinRR, uint8_t dirPinRR,
    uint8_t pwmPinLF, uint8_t dirPinLF,
    uint8_t pwmPinRF, uint8_t dirPinRF,
    CommandDispatcher* pCD, Position* pOD ) : 

                                                Actor( pCD ),
                                                _pwmPinLF(pwmPinLF), 
                                                _pwmPinRF(pwmPinRF), 
                                                _pwmPinLR(pwmPinLR), 
                                                _pwmPinRR(pwmPinRR),

                                                _dirPinLF(dirPinLF), 
                                                _dirPinRF(dirPinRF), 
                                                _dirPinLR(dirPinLR), 
                                                _dirPinRR(dirPinRR),

                                                _pPosition( pOD )
{
    _pName = "Motor";
    // note that pNextSub is being overwritten here, but this should not be a problem as long as
    // the next subscriber for each event is the same, which it should be here.
    // if necessary, these could be separate variables, and the appropriate pointer would need
    // to be returned in HandleEvent().
    SubscribeTo( pCD, 'M' );    // All our commands begin with "L"

    //LED_On_duration_ms = 0;
    //LED_Off_duration_ms = 0;
 
    //LED_Change_Time = 0;
    //LED_State = LOW;
    
    pinMode( _pwmPinLF, OUTPUT );
    pinMode( _pwmPinRF, OUTPUT );
    pinMode( _pwmPinLR, OUTPUT );
    pinMode( _pwmPinRR, OUTPUT );
    pinMode( _dirPinLF, OUTPUT );
    pinMode( _dirPinRF, OUTPUT );
    pinMode( _dirPinLR, OUTPUT );
    pinMode( _dirPinRR, OUTPUT );
}


void MotorDriver::Update( void ) {
/*    if ( LED_On_duration_ms > 0 ) {
        unsigned int currTime = millis();
        if ( currTime > LED_Change_Time ) {
            LED_State = !LED_State;
            digitalWrite( _pwmPin, LED_State );
            LED_Change_Time = currTime + (LED_State == LOW ? LED_Off_duration_ms : LED_On_duration_ms);
        }
    }
*/
}


void MotorDriver::handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams )
{
    if ( _bEnabled ) {
        ControlParams* pControlParams = (ControlParams*) pEvent->pData;
        _throttleLeft = pControlParams->GetLeftThrottle();
        _throttleRight = pControlParams->GetRightThrottle();

        bool bReverseLeft = _throttleLeft < 0;
        bool bReverseRight = _throttleRight < 0;


        analogWrite( _pwmPinLF, constrain( abs(_throttleLeft), 0, 255 ) );
        analogWrite( _pwmPinLR, constrain( abs(_throttleLeft), 0, 255 ) );
        analogWrite( _pwmPinRF, constrain( abs(_throttleRight), 0, 255 ) );
        analogWrite( _pwmPinRR, constrain( abs(_throttleRight), 0, 255 ) );

        digitalWrite( _dirPinLF, bReverseLeft ? 1 : 0 );
        digitalWrite( _dirPinLR, bReverseLeft ? 1 : 0 );
        digitalWrite( _dirPinRF, bReverseRight ? 1 : 0 );
        digitalWrite( _dirPinRR, bReverseRight ? 1 : 0 );

        // display name of subsuming Actor
        if ( _messageMask & MM_INFO && pControlParams->ActorInControl() ) {
            Serial.print( '[' );
            Serial.print( pControlParams->ActorInControl()->GetName() );
            Serial.print( F( "] " ) );
            Serial.print( _throttleLeft );
            Serial.print( '/' );
            Serial.println( _throttleRight );
        }

        if ( _messageMask & MM_INFO ) {
            //Serial.print( F( "Positions set to: " ) );
            //Serial.print( _pPosition->_currentEncoderPositionLeft );
            //Serial.print( '/' );
            //Serial.println( _pPosition->_currentEncoderPositionRight );

            PRINT_VAR( bReverseLeft );
            PRINT_VAR( bReverseRight );
            
        }
    }
}


void MotorDriver::handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs )
{
    CommandArgs* pData = (CommandArgs*) pEvent->pData;

    // check the sub-command.
    switch( pData->inputBuffer[1] ) {
        case 'S' : // set "speeds"
            if ( _bEnabled ) {

                int leftSpeed = pData->nParams[0];
                int rightSpeed = pData->nParams[1];

                analogWrite( _pwmPinLF, leftSpeed < 0 ? 0 : leftSpeed );
                analogWrite( _pwmPinRF, rightSpeed < 0 ? 0 : rightSpeed );
                analogWrite( _pwmPinLR, leftSpeed < 0 ? -leftSpeed : 0 );
                analogWrite( _pwmPinRR, rightSpeed < 0 ? -rightSpeed : 0 );

                if ( _messageMask & MM_RESPONSES ) {
                    Serial.print( F( "Motor speeds directly set to: " ) );
                    Serial.print( leftSpeed ); Serial.print( '\t' );
                    Serial.println( rightSpeed );
                }

            }
            break;
    }
}


void MotorDriver::PrintHelp( uint8_t eventID ) 
{
    // we only handle one event, the "L" command:
    Serial.println( F( "\nMotor Driver Control:" ) );
    Actor::PrintHelp( 'M' );
    Serial.println( F( "  MS <LeftSpeed> <RightSpeed>: Set 'Motor' speeds" ) );
}

