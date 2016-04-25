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

                                                Behavior( pCD ),
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
    _pName = F("Motor");
    _throttleChangeLimit = 64;  // prevent throttle from changing more than this in each step

    SubscribeTo( pCD, 'M' );    // All our commands begin with "M"
    
    pinMode( _pwmPinLF, OUTPUT );
    pinMode( _pwmPinRF, OUTPUT );
    pinMode( _pwmPinLR, OUTPUT );
    pinMode( _pwmPinRR, OUTPUT );
    pinMode( _dirPinLF, OUTPUT );
    pinMode( _dirPinRF, OUTPUT );
    pinMode( _dirPinLR, OUTPUT );
    pinMode( _dirPinRR, OUTPUT );

    _pHelpString = F( "  S <LeftSpeed> <RightSpeed>: Set 'Motor' speeds\n"
                       "  L <Limit>: Set throttle change limit" );
}


void MotorDriver::handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams )
{
    if ( _bEnabled ) {
        ControlParams* pControlParams = (ControlParams*) pEvent->pData;

        // don't allow rapid throttle changes
        _throttleLeft = constrain( pControlParams->GetLeftThrottle(), _throttleLeft - _throttleChangeLimit, _throttleLeft + _throttleChangeLimit );
        _throttleRight = constrain( pControlParams->GetRightThrottle(), _throttleRight - _throttleChangeLimit, _throttleRight + _throttleChangeLimit );

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

        // display name of subsuming Behavior
        if ( _messageMask & MM_INFO && pControlParams->BehaviorInControl() ) {
            Serial.print( '[' );
            Serial.print( pControlParams->BehaviorInControl()->GetName() );
            Serial.print( F( "] " ) );
            Serial.print( _throttleLeft );
            Serial.print( '/' );
            Serial.println( _throttleRight );
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

        case 'L' : // set throttle limit
            _throttleChangeLimit = pData->nParams[0];

            IF_MSG( MM_RESPONSES ) {
                Serial.print( F( "Motor throttle change limit set to " ) );
                Serial.println( _throttleChangeLimit );
            }
            break;
    }
}
