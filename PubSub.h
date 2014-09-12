/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

#include "CommonDefs.h"

/**

publisher/subscriber

subscriber tells publisher what event it wants
	publisher needs to know:
	1. event ID:  could be an int, char, or enum which identifies which event is desired.  Some 
	   publishers may have only one event, in which case this could be ignored.  Others (such as 
	   the Command Processor) could have many possible events
	2. pointer to subscriber object to be notified
	
	if publisher already has a subscriber for this event, it will return a pointer to that subscriber.  
	The subscriber should cache this pointer so that it can forward any notifications.  
	This allows the publisher to notify any number of subscribers without having to maintain a dynamic container.
	
	
	
publisher sends notifications to subscriber(s) when subscribed events occur
	publisher sends pointer to struct containing:
	1. publisher ID
	2. event ID
	3. other data specific to the publisher or event

    */

class Subscriber;
class Publisher;

struct EventNotification 
{
    Publisher*  pPublisher; // source of this notification
    uint8_t     eventID;    // ID of the event
    void*       pData;      // additional data associated with th   is event
};

class Publisher
{
public:

    // this is the event we will publish
    EventNotification notification;

    // subscribers call this function to subscribe to the given event
    virtual Subscriber* Subscribe( Subscriber* pSub, uint8_t eventID );

    // this is the first subscriber in our chain of subscribers
    Subscriber* _pFirstSubscriber;

protected:

    // send the notification to each of the subscribers.
    // iterate through the subscriber chain, calling HandleEvent for each subscriber
    virtual void publish( Subscriber* pSub, EventNotification* pEvent );

private:
};

class Subscriber
{
protected:
    Subscriber* _pNextSub;   // pointer to the next subscriber in the chain
    char*       _pName;

public:
    // some subscribers may need a custom version of this.  For example, if
    // a subscriber subscribes to multiple publishers, it will need to keep
    // a "next subscriber" pointer for each publisher.
    virtual void SubscribeTo( Publisher* pPub, uint8_t eventID );

    Subscriber(void) { _pNextSub = NULL; _pName = "<Unnamed Subscriber>"; }

    virtual Subscriber* HandleEvent( EventNotification* pEvent ) = 0;

    const char* GetName( void ) { return _pName; }
};
