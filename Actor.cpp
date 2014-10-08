/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include "Actor.h"

Actor::Actor( CommandDispatcher* pCD ) : CommandSubscriber( pCD ), _bEnabled( true ), _messageMask( 1 )
{

}

void Actor::SubscribeTo( Publisher* pPub, uint8_t eventID /* = 0 */ )
{
    if ( pPub ) {
        // subscribing to Director
        if ( eventID == 0 ) {
            _pNextActor = pPub->Subscribe( static_cast<Subscriber*> (this), eventID );
        }
        else {  // subscribing to CommandDispatcher
            // if necessary, we could keep a "next" pointer for each eventID,
            // but it should not be necessary in this case
            _pNextSub = pPub->Subscribe( static_cast<CommandSubscriber*> (this), eventID );
        }
    }
}

// For an Actor, there are two kinds of events:  Control events from the Director, or
// Command events from the Dispatcher.  Here, we distinguish between the two and
// route them accordingly.  Also, we handle the common sub-commands.
Subscriber* Actor::HandleEvent( EventNotification* pEvent ) 
{
    Subscriber* pReturnSub = NULL;

    if ( pEvent->eventID == 0 ) {   // Director event
        if ( _messageMask & MM_ID ) {
            Serial.print( _bEnabled ? '+' : '-' ); Serial.println( _pName );
        }

        handleControlEvent( pEvent, (ControlParams*) pEvent->pData );
        pReturnSub = _pNextActor;
    }
    else {  // CommandDispatcher event
        // since all Actors have some common functionality, we can handle the common stuff here
        pReturnSub = _pNextSub;

        CommandArgs* pArgs = (CommandArgs*) pEvent->pData;
        switch ( pArgs->inputBuffer[1] ) {
            case '?' :
                PrintHelp( pEvent->eventID );
                break;
            case '0' : 
                _bEnabled = false; 
                break;
            case '1' : 
                _bEnabled = true; 
                break;
            case 'Q' :
                PrintParameterValues();
                break;
            case 'V' :
                switch ( pArgs->inputBuffer[2] ) {
                    case 0 : // no command modifier, just set the value
                        _messageMask = pArgs->nParams[ 0 ]; 
                        break;
                    case '+' : // add these bits
                        _messageMask |= pArgs->nParams[ 0 ];
                        break;
                    case '-' : // remove these bits
                        _messageMask &= ~pArgs->nParams[ 0 ];
                        break;
                }
                Serial.print( _pName );
                Serial.print( F( " message mask:\t0x" ) ) ;
                Serial.println( _messageMask, HEX );
                break;
            default: 
                handleCommandEvent( pEvent, pArgs ); 
                break;
        }
    }
    return pReturnSub;
}

void Actor::PrintHelp( uint8_t eventID )
{
    PrintParameterValues();

    Serial.print( "  " ); Serial.print( (char) eventID );
    Serial.println( F( "0: Disable" ) );
    Serial.print( "  " ); Serial.print( (char) eventID );
    Serial.println( F( "1: Enable" ) );
    Serial.print( "  " ); Serial.print( (char) eventID );
    Serial.println( F( "Q: Query Parameter Values" ) );
    Serial.print( "  " ); Serial.print( (char) eventID );
    Serial.println( F( "V[+|-] <mask> : Set verbosity mask, or add/remove bits" ) );
}

void Actor::PrintParameterValues()
{
    Serial.print( '\n' );
    Serial.print( _pName );
    Serial.print( ": " );
    Serial.print( _bEnabled ? F( "Enabled" ) : F( "Disabled" ) );
    Serial.print( F( ", verbosity = " ) );
    Serial.println( _messageMask, HEX );
    PrintSpecificParameterValues();
}

void Actor::PrintSpecificParameterValues()
{
    Serial.print( F( " No parameter query defined for " ) );
    Serial.println( _pName );
}
