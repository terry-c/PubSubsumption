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

// CruiseControl
//
// This class is intended as a default behavior.  
// If no other behavior asserts itself, CruiseControl will drive the motors to 
// maintain a specified speed and a straight-ahead course.
//
// Speed is set either by command or waypoint.
// The throttle settings established by CruiseControl are expected to be honored by all other
// behaviors.  That is, CruiseControl attempts to maintain the currently-defined speed limit, 
// so if higher-priority behaviors need to deviate from that, it should be by slowing one or 
// both motors, not by increasing their speed.  There may be exceptions to this.
//
// Speed Limit is set (in IPS) by sending a command (CS) or by suggestion passed down from higher
// priority behaviors (such as Navigator).  This speed is used to calculate a target distance (in
// encoder ticks) which is to be added at each interval to determine whether each motor is running
// at the proper speed.
//
// When CruiseControl gains control after having been subsumed, it samples the current Position
// readings in order to maintain the current heading.  It calculates the expected (target) Position
// readings for the next interval by adding the pre-computed target distance to the current readings.
//
// At subsequent intervals, the current Position readings are subtracted from the target readings
// to compute an error value.  This error value is then used in to compute adjustments using what
// is effectively a PID algorithm.  The error is used directly (with an appropriate coefficient, which
// is set with the CP command) to compute the P (proportional) term.  The I (Integral) term is actually
// computed by comparing the current Position reading with the projected ideal Position reading.  This
// ideal is computed by simply adding the target distance to the previous ideal at each interval, so that
// the errors will accumulate.  A D (Derivative) term could be computed, as well, if it proves useful.
// The commands for setting the I and D coefficients are CI and CD.  All commands take a float argument.

class CruiseControl : public Behavior
{
    float       _idealPositionLeft;
    float       _idealPositionRight;

    float       _prevPositionLeft;
    float       _prevPositionRight;

    float       _prevErrorLeft;
    float       _prevErrorRight;

    int         _throttleLeft;
    int         _throttleRight;

    float       _targetSpeedIPS;

    bool        _bCruising;

    Position*   _pPosition;

    float       _kP;
    float       _kI;
    float       _kD;

public:
    CruiseControl( CommandDispatcher* pCD, Position* pOD );

    void SetCruiseSpeed( float speedIPS )    { _targetSpeedIPS = speedIPS; }

    virtual void    handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs );
    virtual void    handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams );

    virtual void    PrintSpecificParameterValues();
};
