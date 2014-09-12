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

// LEDDriver simulates motors in a simplistic way:
// A motor is represented by two LED's, one to indicate forward motion, the other to indicate backward motion.
// These LED's use PWM to indicate speed using brightness.

// As a CommandHandler, LEDDriver implements the following commands: <<<<< REVISE >>>>>
// The base (noun) command is "L", followed by optional subcommands and arguments
// In normal operation, the "L0" and "L1" commands disable and enable the LEDDriver 
// This is true of all Actor objects.
// For diagnostic and testing purposes (which is what the LEDDriver is for anyway),
// the "L <LeftSpeed> <RightSpeed>" command sets the speed
// of the two "motors".  Reverse direction is indicated by negative speeds.
// For diagnostic and testing purposes,

class MotorDriver : public Actor
{
    uint8_t _pwmPinLF, _dirPinLF,
            _pwmPinRF, _dirPinRF,
            _pwmPinLR, _dirPinLR,
            _pwmPinRR, _dirPinRR;



    Position*       _pPosition;

    int             _throttleLeft;
    int             _throttleRight;

public:
    MotorDriver( 
        uint8_t pwmPinLR, uint8_t dirPinLR,
        uint8_t pwmPinRR, uint8_t dirPinRR,
        uint8_t pwmPinLF, uint8_t dirPinLF,
        uint8_t pwmPinRF, uint8_t dirPinRF,
        
        CommandDispatcher* pCD, Position* pOD);
    void                Update( void );
//    virtual Subscriber* HandleEvent(EventNotification* pEvent);
    virtual void        handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs );
    virtual void        handleControlEvent( EventNotification* pEvent, MotorParams* pMotorParams );

    virtual void        PrintHelp( uint8_t eventID );
};
