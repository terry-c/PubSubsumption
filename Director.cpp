/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include "Director.h"

Director::Director( CommandDispatcher* pCD, uint16_t interval) : Actor( pCD ), _pCD( pCD ), _intervalMS( interval ), _bEnabled( true ), _bInhibit( true )
{
    _pName = "Director";


    // we'll turn on the LED during execution of the subsumption chain
    // to give some idea of how much time it takes
    pinMode( 13, OUTPUT );

    _tickTimeMS = millis() + _intervalMS;

    notification.pData = &_controlParams;

    SubscribeTo( pCD, 'D' );
}


Director::~Director(void)
{
}


// Update() gets called from setup() as frequently as possible.  At
// intervals specified by _intervalMS, it initiates a Subsumption control event
// and publishes it to all the Actors.
void Director::Update()
{
    if ( millis() >= _tickTimeMS ) {
        digitalWrite( 13, HIGH );   // turn the LED on for the duration of this event to give a visual indication of the time required.

        _tickTimeMS += _intervalMS;

        if ( _bInhibit ) {
            _controlParams.SetThrottles( 0, 0 );
            _controlParams.ControlledBy( this );
        }
        else {
            // reset the flag and send it down the line
            _controlParams.ControlledBy( NULL );
        }
        IF_MSG( MM_PROGRESS ) {
            Serial.println( F( "\n----" ) );
        }
        publish( _pFirstSubscriber, &notification );

        // CSV state change.  If we've done headings, move on to data.
        if ( _controlParams.PrintingCsvHeadings() ) {
            _controlParams.PrintCsvData();
        }

        if ( _controlParams.PrintingCsv() ) {
            Serial.println();
        }

        digitalWrite( 13, LOW );
//        Serial.println( millis() - ( _tickTimeMS - _intervalMS) );
    }
}

void Director::PrintHelp( uint8_t eventID ) 
{

    // we only handle one event, the "D" command:
    Serial.println( F( "\nDirector:" ) );
    Actor::PrintHelp( 'D' );
    Serial.println( F(  "  DI <ms>: set interval ms\n"
                        "  DG : Go\n"
                        "  DL : Start CSV Logging\n"
                        "  DS : stop"
                        ) );
}


void Director::handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs )
{
    switch ( pArgs->inputBuffer[1] ) {
        case 'I' : // set interval
            _intervalMS = pArgs->nParams[0];
            if ( _messageMask & MM_RESPONSES ) {
                Serial.print( F( "Subsumption Interval milliseconds = " ) );
                Serial.println( _intervalMS );
            }
            break;
        case 'S' :  // Stop -- inhibit all Actors
            _bInhibit = true;
            _controlParams.StopCsvOutput();
            if ( _messageMask & MM_RESPONSES ) {
                Serial.println( F( "Director Stopped" ) );
            }
            break;
        case 'G' :  // Go -- allow Actors to act
            _bInhibit = false;
            if ( _messageMask & MM_RESPONSES ) {
                Serial.println( F( "\n==========================\nDirector Started" ) );
            }
            break;
        case 'L' : // begin CSV logging
            _controlParams.PrintCsvHeadings();
            if ( _messageMask & MM_RESPONSES ) {
                Serial.println( F( "Director Starting CSV data logging" ) );
                Serial.println( F( "First, the CSV headings . . ." ) );
            }
            break;
    }
}