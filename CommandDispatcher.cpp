/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

//#include <Arduino.h>
#include "CommandDispatcher.h"

CommandDispatcher::CommandDispatcher() 
{
    notification.pPublisher = this;
    notification.pData = &args;

    memset(args.nParams, 0, sizeof(args.nParams) );
    memset(args.fParams, 0, sizeof(args.fParams) );
    memset(subscribers, 0, sizeof(subscribers) );
    memset( args.inputBuffer, 0, sizeof( args.inputBuffer ) );
    bufIx = 0;
}

CommandDispatcher::~CommandDispatcher() {}

#define TokenDelimiters " ,\t"

/// Update is called as frequently as possible to check whether input has been received from the console.
/// Received characters are accumulated into a command buffer.  When a CR is received, the buffer is parsed
/// into a command and arguments.  If the buffer overflows before a CR is received, remaining characters are
/// discarded and an error message is sent back to the console.
///
/// Commands consist of a single alphabetic character followed by up to four numeric arguments.  The
/// arguments can be either integers or floats.
///
/// The parser parses the arguments into two arrays, one of ints, the other of floats.  These arrays
/// are contained in a struct which is passed to the Subscriber linked to the command.  The CommandDispatcher
/// knows nothing about the commands; this knowledge is contained in the Subscribers.
void CommandDispatcher::Update()
{
    if ( Serial.available() > 0 ) {
        char ch = toUpperCase( Serial.read() );
        if ( '\r' == ch ) {
            // process command completion
            
            // parse out the command character
            char* pCh = strtok( args.inputBuffer, TokenDelimiters );
            char cmdChar = pCh[0];  // command is the first character of the first token

            // parse out the remaining arguments
            int argIx = 0;
            while ( pCh && argIx < MaxArgs ) {
                pCh = strtok( NULL, TokenDelimiters );
                args.fParams[ argIx ] = atof( pCh );
                args.nParams[ argIx ] = atoi( pCh );
                argIx++;
            }

            switch ( cmdChar ) {
                case '?' :
                    // if just a ?, or invalid command, list commands
                    if ( ( strlen( args.inputBuffer ) == 1 ) || dispatchCommand( args.inputBuffer[1], eHelpDetail ) == NULL ) {
                        Serial.println(F("\nAvailable Objects:"));
                        for ( char cmd = 'A'; cmd <= 'Z'; cmd++ ) {
                            dispatchCommand( cmd, eHelpSummary );
                        }
                    }
                    break;
                case '*' : // broadcast
                    Serial.print( F( "\nBroadcasting command: " ) );
                    Serial.println( args.inputBuffer + 1 );
                    for ( char cmd = 'A'; cmd <= 'Z'; cmd++ ) {
                        dispatchCommand( cmd, eNotify );
                    }
                    break;
                default:
                    dispatchCommand( cmdChar, eNotify );
                    break;
            }

            // reset the buffer
            memset( args.inputBuffer, 0, sizeof( args.inputBuffer ) );
            bufIx = 0;
        }
        else {
            if ( bufIx >= 32 ) {
                // buffer overflow
            }
            else {
                args.inputBuffer[ bufIx++ ] = ch;
            }
        }
    }
}

/// Subscribe is inherited from the Publisher base class.  This associates a Subscriber with a specified
/// command letter.  The return value is a pointer to the current Subscriber (if any) for that event.  The
/// Subscriber is expected to cache this pointer and return it when its HandleEvent() method is called.  The
/// CommandDispatcher then uses this pointer to send notifications to other Subscribers interested
/// in this same event.  This way, the CommandDispatcher can notify any number of Subscribers of the same event
/// without the need for a dynamic container.  
Subscriber* CommandDispatcher::Subscribe( Subscriber* pSub, uint8_t eventID )
{
    Subscriber* pCurrentSubscriber = NULL;

    // range-check the eventID.  Valid events are 'A' through 'Z', and '*'.
    if ( (char) eventID < 'A' || (char) eventID > 'Z' ) {
        // consider returning failure code somehow, or not.
    }
    else {
        pCurrentSubscriber = subscribers[ eventID - 'A' ];
        subscribers[ eventID - 'A' ] = pSub;
    }
    return pCurrentSubscriber;
}
/// dispatch the given event to the associated subscriber
/// if there is no associated subscriber, return false
/// "dispatch" can mean one of three things, depending upon the value of eAction:
///      eNotify         notify subscriber of event
///      eHelpSummary    send subscriber's help summary string to output
///      eHelpDetail     send subscriber's help detail string to output

/// Help summary is intended to be used in a command list in response to the '?' command
/// Help detail is intended to be used for command-specific ?* command.
/// This means that the summary action should just quietly return false if the specified 
/// command is not associated with a subscriber, while the detail action should print
/// an appropriate message in that case.
/// 
Subscriber* CommandDispatcher::dispatchCommand( char cmdChar, eDispatchAction eAction ) 
{
    // look up the command in the Subscriber table
    Subscriber* pSubscriber = NULL;
    int cmdIx = cmdChar - 'A';
    if ( cmdIx < 0 || cmdIx > 25 ) {
        // command out of range
        Serial.println(F("Invalid command"));
    }
    else {
        pSubscriber = subscribers[ cmdIx ];
    }

    // if there's a subscriber for this event, send the notification
    // or print the help string
    if ( pSubscriber ) {
        switch ( eAction ) {
        case eNotify :
            notification.eventID = cmdChar;
            publish( pSubscriber, &notification );
            break;
        case eHelpSummary :
            Serial.print( F( "  " ) );
            Serial.print( cmdChar );
            Serial.print( F( " - " ) );
            Serial.println( pSubscriber->GetName() );
            break;
        }
    }
    else {
        // no subscriber for this command
        if ( eAction == eHelpDetail ) {
            Serial.println( F("No handler for this command." ) );
        }
    }
    return pSubscriber;
}