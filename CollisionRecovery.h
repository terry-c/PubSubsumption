/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

#include <Director.h>
#include <CommandDispatcher.h>


// The CollisionRecovery class implements behaviors which are intended to get the robot
// around obstacles it has crashed into.  It detects collisions through two "bumper" inputs (left and right),
// basically switch closures connected to two input pins.  
//
// The recovery behavior consists of a sequence of steps, or states:
//
//      1. Stop
//      2. Reverse
//      3. Turn away from the collision side
//      4. Forward
//
// Each state has an associated duration and speed, which can be configured from the console.

class CollisionRecovery : public Actor
{
    enum eRecoveryState {
        eNormal,
        eStopped,
        eReversing,
        eTurning,
        eForward,
        eStates
    };

    eRecoveryState  _eState;
    int             _nStateTimer;
    int             _StateTimes[ eStates ];
    int             _StateSpeeds[ eStates ];

    int             _leftMotorSpeed;
    int             _rightMotorSpeed;

    bool            _bSimBumpLeft;
    bool            _bSimBumpRight;

    // pins connected to bumper switches
    uint8_t         _leftPin;
    uint8_t         _rightPin;
    
public:

    CollisionRecovery( CommandDispatcher* pCD, uint8_t leftPin, uint8_t rightPin );
    ~CollisionRecovery() {}

//    virtual Subscriber* HandleEvent( EventNotification* pEvent );
    virtual void    handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs );
    virtual void    handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams );


    virtual void    PrintHelp();
    virtual void    PrintSpecificParameterValues();

};
