/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include "Director.h"

Director::Director( CommandDispatcher* pCD, uint16_t interval) : Behavior( pCD ), _pCD( pCD ), /*_intervalMS( interval ),*/ _bEnabled( true ), _bInhibit( true )
{
    _pName = F("Director");

    // we'll turn on the LED during execution of the subsumption chain
    // to give some idea of how much time it takes
    pinMode( 13, OUTPUT );

    _tickTimeMS = millis() + _controlParams.GetInterval();

    _notification.pData = &_controlParams;

    SubscribeTo( pCD, 'D' );

    _pHelpString =  F(  "  I <ms>: set interval ms\n"
                        "  G : Go\n"
                        "  L : Start CSV Logging\n"
                        "  S : stop"
                    );
}


Director::~Director(void)
{
}


// Update() gets called from loop() as frequently as possible.  At
// intervals specified by _intervalMS, it initiates a Subsumption control event
// and publishes it to all the Behaviors.
void Director::Update()
{
    if ( millis() >= _tickTimeMS ) {
        digitalWrite( 13, HIGH );   // turn the LED on for the duration of this event to give a visual indication of the time required.

        _tickTimeMS += _controlParams.GetInterval();

        if ( _bInhibit ) {
            _controlParams.SetThrottles( 0, 0, this );
        }
        else {
            // not inhibiting, let someone else have a chance for a change
            _controlParams.ControlledBy( NULL );
        }

        // add a visual divider at the beginning of the subsumption chain
        PROGRESS_MSG( "\n----" );
        
        // send the event down the chain
        publish( _pFirstSubscriber, &_notification );

#ifdef USE_CSV
        // CSV state change.  If we've done headings, move on to data.
        if ( _controlParams.PrintingCsvHeadings() ) {
            _controlParams.PrintCsvData();
        }
#endif

        if ( _controlParams.PrintingCsv() ) {
            Serial.println();
        }

        digitalWrite( 13, LOW );
    }
}


void Director::handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs )
{
    switch ( pArgs->inputBuffer[1] ) {
        case 'I' : // set interval
            _controlParams.SetInterval( pArgs->nParams[0] );
            if ( _messageMask & MM_RESPONSES ) {
                Serial.print( F( "Subsumption Interval milliseconds = " ) );
                Serial.println( _controlParams.GetInterval() );
            }
            break;
        case 'S' :  // Stop -- inhibit all Behaviors
            _bInhibit = true;
            _controlParams.StopCsvOutput();
            if ( _messageMask & MM_RESPONSES ) {
                Serial.println( F( "Director Stopped" ) );
            }
            break;
        case 'G' :  // Go -- allow Behaviors to behave
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
