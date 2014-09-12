/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

#include <CommandDispatcher.h>
#include <Director.h>
#include <Position.h>

// Navigator class
//
// The Navigator keeps track of the current position and plots a course from this position to the next Waypoint.
// It has a direct connection to the motors for odometry (encoder tick) data.
// It can be controlled from the console.
// It is a Actor so it participates in the Subsumption architecture, generally as a low priority.

class Navigator : public Actor
{

    enum eNavigatorState {
        eNormal,    // normal operation
        eManual,    // manual operation
        eManualEnd,  // return to normal at next tick
        eStartTurn, // begin turn behavior
        eTurning    // continue turn behavior
    };


    // Subscribers have to maintain a pointer to the next subscriber in their event chain (if any).
    // The Subscriber base class has one pointer for this purpose, but since we subscribe to events
    // from two Publishers ( CommandDispatcher and Director ), we need an extra Subscriber
    // pointer to hold the Director chain.
//    Subscriber* _pNextActor;

    Position* _pPosition;

    int _leftMotorSpeed;
    int _rightMotorSpeed;
    eNavigatorState _eState;

    int _targetHeading;
    int _turnRadius;



public:

    Navigator( CommandDispatcher* pCD, Position* pOd );

//    virtual Subscriber* HandleEvent( EventNotification* pEvent );
    virtual void    handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs );
    virtual void    handleControlEvent( EventNotification* pEvent, MotorParams* pMotorParams );

    virtual void    PrintHelp( uint8_t eventID );
};
