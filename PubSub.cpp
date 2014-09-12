/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <PubSub.h>

void Publisher::publish( Subscriber* pSub, EventNotification* pEvent )
{
    Subscriber* pSubscriber = pSub;

    // HandleEvent() returns a pointer to the next chained Subscriber, if any.
    while ( pSubscriber ) {
        pSubscriber = pSubscriber->HandleEvent( pEvent );
    }
}


// this is a generalized version of Subscribe(), usable by publishers
// which have only a single event, so need only know a single subscriber.
Subscriber* Publisher::Subscribe( Subscriber* pSub, uint8_t eventID )
{
    Subscriber* pNextSubscriber = _pFirstSubscriber;
    _pFirstSubscriber = pSub;
    return pNextSubscriber;
}


void Subscriber::SubscribeTo( Publisher* pPub, uint8_t eventID )
{
    if ( pPub ) {
        _pNextSub = pPub->Subscribe( this, eventID );
    }
}
