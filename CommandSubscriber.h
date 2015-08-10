/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

#include <CommandDispatcher.h>

/// The CommandSubscriber class is a base class for any class which needs to receive CommandDispatcher events.
///
class CommandSubscriber : public Subscriber
{
    /// The single-character command associated with this CommandSubscriber
    char    _charCommand;

public:

    CommandSubscriber( CommandDispatcher* pCD );
};
