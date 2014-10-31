/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <LEDDriver.h>

// LED motor emulator

LEDDriver::LEDDriver( uint8_t lfpin, uint8_t rfpin, uint8_t lbpin, uint8_t rbpin, CommandDispatcher* pCD, Position* pOD, float ticksPerInch ) : 
    Actor( pCD ),
    ledPinLF(lfpin), 
    ledPinRF(rfpin), 
    ledPinLB(lbpin), 
    ledPinRB(rbpin),
    _pPosition( pOD ),
    _ticksPerInch( ticksPerInch )
{
    _pName = "LED 'Motor'";
    // note that pNextSub is being overwritten here, but this should not be a problem as long as
    // the next subscriber for each event is the same, which it should be here.
    // if necessary, these could be separate variables, and the appropriate pointer would need
    // to be returned in HandleEvent().
    SubscribeTo( pCD, 'L' );    // All our commands begin with "L"

    _leftRatio = 1.0;
    _rightRatio = 1.0;   // introduce a differential to simulate extra drag on this side

    _throttleChangeLimit = 64;  // prevent throttle from changing more than this in each step

    pinMode( ledPinLF, OUTPUT );
    pinMode( ledPinRF, OUTPUT );
    pinMode( ledPinLB, OUTPUT );
    pinMode( ledPinRB, OUTPUT );
}

void LEDDriver::Update( void ) {
/*    if ( LED_On_duration_ms > 0 ) {
        unsigned int currTime = millis();
        if ( currTime > LED_Change_Time ) {
            LED_State = !LED_State;
            digitalWrite( ledPin, LED_State );
            LED_Change_Time = currTime + (LED_State == LOW ? LED_Off_duration_ms : LED_On_duration_ms);
        }
    }
*/
}


void LEDDriver::handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams )
{
    if ( _bEnabled ) {
        ControlParams* pControlParams = (ControlParams*) pEvent->pData;
        _throttleLeft = constrain( pControlParams->GetLeftThrottle(), _throttleLeft - _throttleChangeLimit, _throttleLeft + _throttleChangeLimit );
        _throttleRight = constrain( pControlParams->GetRightThrottle(), _throttleRight - _throttleChangeLimit, _throttleRight + _throttleChangeLimit );

        analogWrite( ledPinLF, _throttleLeft < 0 ? 0 : constrain( _throttleLeft, 0, 255 ) );
        analogWrite( ledPinRF, _throttleRight < 0 ? 0 : constrain( _throttleRight, 0, 255 ) );
        analogWrite( ledPinLB, _throttleLeft < 0 ? constrain( -_throttleLeft, 0, 255 ) : 0 );
        analogWrite( ledPinRB, _throttleRight < 0 ? constrain( -_throttleRight, 0, 255 ) : 0 );

        // display name of subsuming Actor
        if ( _messageMask & MM_INFO && pControlParams->ActorInControl() ) {
            Serial.print( '[' );
            Serial.print( pControlParams->ActorInControl()->GetName() );
            Serial.print( F( "] " ) );
            Serial.print( _throttleLeft );
            Serial.print( '/' );
            Serial.println( _throttleRight );
        }

        // simulate Position update.  Assume full throttle yields 10 IPS, scale to produce encoder ticks per step
        // apply differential ratios to simulate motor/gear/wheel differences in throttle response.
        float maxTicksPerStep = _ticksPerInch * 10.0 * ( (float) pControlParams->GetInterval() / 1000.0 );
        _pPosition->_currentEncoderPositionLeft += map( _throttleLeft, 0, 255, 0, maxTicksPerStep ) * _leftRatio;
        _pPosition->_currentEncoderPositionRight += map( _throttleRight, 0, 255, 0, maxTicksPerStep )  * _rightRatio;

        if ( _messageMask & MM_PROGRESS ) {
            PRINT_VAR( maxTicksPerStep );
            Serial.print( F( "Positions set to: " ) );
            Serial.print( _pPosition->_currentEncoderPositionLeft );
            Serial.print( '/' );
            Serial.println( _pPosition->_currentEncoderPositionRight );
        }

        IF_CSV( MM_CSVBASIC ) {
            CSV_OUT( _throttleLeft );
            CSV_OUT( _throttleRight );
            CSV_OUT( pControlParams->ActorInControl()->GetName() );
        }
    }
}


void LEDDriver::handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs )
{
    CommandArgs* pData = (CommandArgs*) pEvent->pData;

    // check the sub-command.
    switch( pData->inputBuffer[1] ) {
        case 'S' : // set "speeds"
            if ( _bEnabled ) {

                int leftSpeed = pData->nParams[0];
                int rightSpeed = pData->nParams[1];

                analogWrite( ledPinLF, leftSpeed < 0 ? 0 : leftSpeed );
                analogWrite( ledPinRF, rightSpeed < 0 ? 0 : rightSpeed );
                analogWrite( ledPinLB, leftSpeed < 0 ? -leftSpeed : 0 );
                analogWrite( ledPinRB, rightSpeed < 0 ? -rightSpeed : 0 );
                if ( _messageMask & MM_RESPONSES ) {
                    Serial.print( F( "LED simulator speeds directly set to: " ) );
                    Serial.print( leftSpeed ); Serial.print( '\t' );
                    Serial.println( rightSpeed );
                }
            }
            break;
        case 'D' : // set throttle/speed differential
            _leftRatio = pData->fParams[0];
            _rightRatio = pData->fParams[1];
                if ( _messageMask & MM_RESPONSES ) {
                    Serial.print( F( "LED simulator throttle/speed ratios set to: " ) );
                    Serial.print( _leftRatio ); Serial.print( '\t' );
                    Serial.println( _rightRatio );
                }
            break;
        case 'L' : // set throttle limit
            _throttleChangeLimit = pData->nParams[0];\
            break;
    }
}


void LEDDriver::PrintHelp() 
{
    Actor::PrintHelp();
    Serial.println( F(  "  D <LeftRatio> <RightRatio>: Set 'Motor' differential ratios\n"
                        "  L <Limit>: Set throttle change limit\n"
                        "  S <LeftSpeed> <RightSpeed>: Set 'Motor' speeds"
                        ) );
}

