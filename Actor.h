/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

#include <CommandDispatcher.h>
#include <CommandSubscriber.h>

class ControlParams;

/// The Actor class is the base class for all participants in the Subsumption chain.
///
/// 
class Actor : public CommandSubscriber
{
protected:

    /// _bEnabled. Enablement means different things to each Actor.  Some will ignore it altogether (Position), while
    /// others will still update some internal state but will not participate otherwise.
    bool            _bEnabled;

    /// Verbosity. _messageMask controls what messages are emitted by various Actors.  This is at the discretion of each Actor, but general guidelines are:
    ///
    /// * 0 = no output
    /// * 1 = responses to commands
    /// * 2 = informational messages
    /// * 4 = debug messages
    ///
    /// default verbosity level is set to 1
    uint16_t         _messageMask;

    /// _pNextActor. pointer to the next actor in the Subsumption chain.
    Subscriber*     _pNextActor;

    /// Print common parameter values, such as verbosity, etc.  Then call PrintSpecificParameterValues()
    void            PrintParameterValues();

public:

    Actor( CommandDispatcher* pCD );
    ~Actor(){}

    // this handles the subscription contract for Dispatcher and Director events
    // the default 0 eventID subscribes to the Director.
    virtual void        SubscribeTo( Publisher* pPub, uint8_t eventID = 0 );

    // derived Actors should override PrintHelp() to provide specific help.
    // they can call Actor::PrintHelp() to display the common commands (Help, Enable, Disable)
    virtual void        PrintHelp( uint8_t eventID );

    // derived Actors should override PrintSpecificParameterValues() to list their respective parameters
    virtual void        PrintSpecificParameterValues();

    // Handle events coming from the Dispatcher or the Director.  we route these to
    // the two methods below, respectively.
    virtual Subscriber* HandleEvent( EventNotification* pEvent );

    // handle Dispatcher events
    virtual void        handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs ) = 0;

    // handle Director events
    virtual void        handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams ) = 0;
};


