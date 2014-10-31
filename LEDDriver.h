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
/// This is true of all Actor objects.
/// For diagnostic and testing purposes (which is what the LEDDriver is for anyway),
/// the "L <LeftSpeed> <RightSpeed>" command sets the speed
/// of the two "motors".  Reverse direction is indicated by negative speeds.

class LEDDriver : public Actor
{
    uint8_t         ledPinLF, ledPinRF, ledPinLB, ledPinRB;

    Position*       _pPosition;
    float           _ticksPerInch;

    int             _throttleLeft;
    int             _throttleRight;

    float           _leftRatio;
    float           _rightRatio;

    int             _throttleChangeLimit;

public:

    LEDDriver( uint8_t lfpin, uint8_t rfpin, uint8_t lbpin, uint8_t rbpin, CommandDispatcher* pCD, Position* pOD, float ticksPerInch );
    void                Update( void );
//    virtual Subscriber* HandleEvent(EventNotification* pEvent);
    virtual void        handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs );
    virtual void        handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams );

    virtual void        PrintHelp();
};
