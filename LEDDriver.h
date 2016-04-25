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

/// LEDDriver simulates motors in a simplistic way:
/// A motor is represented by two LED's, one to indicate forward motion, the other to indicate backward motion.
/// These LED's use PWM to indicate speed using brightness.
/// /todo
/// As a CommandHandler, LEDDriver implements the following commands: /todo <<<<< REVISE >>>>>
/// The base (noun) command is "L", followed by optional subcommands and arguments
/// In normal operation, the "L0" and "L1" commands disable and enable the LEDDriver 
/// This is true of all Behavior objects.
/// For diagnostic and testing purposes (which is what the LEDDriver is for anyway),
/// the "L <LeftSpeed> <RightSpeed>" command sets the speed
/// of the two "motors".  Reverse direction is indicated by negative speeds.

class LEDDriver : public Behavior
{
    uint8_t         _ledPwmPinLeft, _ledPwmPinRight, _ledDirPinLeft, _ledDirPinRight;

    Position*       _pPosition;
    float           _ticksPerInch;

    int             _throttleLeft;
    int             _throttleRight;

    float           _leftRatio;
    float           _rightRatio;

    int             _throttleChangeLimit;

    void            SetLED( int speed, int pwmPin, int dirPin );


public:

    LEDDriver( uint8_t pwmPinLeft, uint8_t pwmPinRight, uint8_t dirPinLeft, uint8_t dirPinRight, CommandDispatcher* pCD, Position* pOD, float ticksPerInch );
    void                Update( void );
    virtual void        handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs );
    virtual void        handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams );
};
