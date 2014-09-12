/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

#include <CommandDispatcher.h>
#include <Actor.h>

// MotorParams contains the motor control values which are passed through the Subsumption stack
// and end up controlling the motors
class MotorParams 
{
public:

    //float   LeftMotorSpeed;
    //float   RightMotorSpeed;

    int     _throttleLeft;
    int     _throttleRight;

    Actor*  _pTakenBy;  // pointer to the first subsumption layer which takes control
};


class Director : public Publisher, public Actor
{
    CommandDispatcher* _pCD;

//    Subscriber* _pFirstSubscriber;

    bool            _bEnabled;
    bool            _bInhibit;

    uint16_t   _intervalMS;
    unsigned long   _tickTimeMS;

    // MotorParams object which is passed to all Behaviors through the Publisher's EventNotification.
    MotorParams    motorParams;

public:
    // interval is the subsumption interval in ms.
    Director( CommandDispatcher* pCD, uint16_t interval );
    ~Director(void);

    /// the Update() function gets called from the Arduino loop() function as frequently as possible.
    /// it checks millis(), and returns if the interval has not elapsed.  When the interval has elapsed,
    /// it sends the subsumption event to the subscribers.
    void Update();

    void PrintHelp( uint8_t eventID );

//    virtual Subscriber* HandleEvent( EventNotification* pEvent );

    virtual void        handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs );
    virtual void        handleControlEvent( EventNotification* pEvent, MotorParams* pMotorParams ) {}  // these would come from the Director
};

