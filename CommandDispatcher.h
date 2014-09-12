/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

//#include "CommonDefs.h"
#include <PubSub.h>

#define MaxArgs 4

class CommandArgs
{
public:
    /// In addition to the common elements inherited from EventNotification, 
    /// the CommandDispatcher also passes the command line buffer, plus two 
    /// arrays which are populated from the command line buffer arguments.
    char    inputBuffer[32];

    int     nParams[MaxArgs];
    float   fParams[MaxArgs];
};


class CommandDispatcher : public Publisher
{

    uint8_t bufIx;

    // the subscriber array contains pointers to the Subscribers which have requested notifications of command
    // events.  This array is indexed by the commands themselves, with the first element corresponding to 'A'.
    //
    // When a command is parsed, the CommandDispatcher looks up the corresponding Subscriber and sends the
    // command parameters to that Subscriber.
    //
    // If the element is NULL, it means there is no subscriber for that command letter, so an error message is
    // sent to the console.
    Subscriber* subscribers[26];

    // this is the event notification object which is passed to Subscribers
    CommandArgs args;

    enum eDispatchAction { eNotify, eHelpSummary, eHelpDetail };

    Subscriber* dispatchCommand( char cmdChar, eDispatchAction eAction );

public:

    CommandDispatcher();

    ~CommandDispatcher();

    // Update is called as frequently as possible to check whether input has been received from the console.
    // Received characters are accumulated into a command buffer.  When a CR is received, the buffer is parsed
    // into a command and arguments.  If the buffer overflows before a CR is received, remaining characters are
    // discarded and an error message is sent back to the console.
    //
    // Commands consist of a single alphabetic character followed by up to four numeric arguments.  The
    // arguments can be either integers or floats.
    //
    // The parser parses the arguments into two arrays, one of ints, the other of floats.  These arrays
    // are contained in a struct which is passed to the Subscriber linked to the command.  The CommandDispatcher
    // knows nothing about the commands; this knowledge is contained in the Subscribers.
    void Update();

    // Subscribe is inherited from the Publisher base class.  This associates a Subscriber with a specified
    // command letter.  The return value is a pointer to the current Subscriber (if any) for that event.  The
    // Subscriber should cache this pointer and use it to forward notifications to other Subscribers interested
    // in this same event.  This way, the CommandDispatcher can notify any number of Subscribers of the same event
    // without the need for a dynamic container.  
    virtual Subscriber* Subscribe( Subscriber* pSub, uint8_t eventID );

};
