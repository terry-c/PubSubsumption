/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

//#include <Encoder.h>

#include <CommandDispatcher.h>
#include <Director.h>

/// The Position class tracks current position.
///
/// The Position class is designed to work with differential steering platforms, with one or more motors on each side.
/// If there are multiple motors on a side, they should be connected to where there may be
/// multiple motors on each side (left and right).  At its simplest level, there will be only one motor on each side,
/// but if there are multiple motors on a side, it is assumed that they operate in tandem, running at the same speed.
/// One option would be to ignore the "extra" motors and just track one motor on each side.  Another option might be
/// to average the motors' odemetry data.
///
/// Position is an Actor, so it participates in the Subsumption chain.  It should be first in the chain, but it will
/// never subsume.  Instead, it takes a snapshot of the encoder positions and performs all the calculations to determine
/// location, orientation, speed, etc.  These values are made available to any subsequent Actor which needs this information.
/// This way, all these calculations are performed in one place, at one time, using the position values captured in the
/// snapshot, so there is no skew caused by sampling at different times.
class Position : public Actor
{
public:

    // leftPosition and rightPosition are 
    // for use by the global interrupt handlers which update these members.
    Position( CommandDispatcher* pCD, Director* pD, 
        uint32_t*& leftPosition, uint32_t*& rightPosition,  // pointers to _currentEncoderPositionLeft and _currentEncoderPositionRight
        float ticksPerRev, float wheelDiameter, float wheelSpacing );

    void PrintHelp( uint8_t eventID );

    /// these are the raw encoder positions, updated directly by the encoder interrupt handlers
    uint32_t    _currentEncoderPositionLeft;
    uint32_t    _currentEncoderPositionRight;

    /// encoder positions are captured here
    uint32_t    _snapshotPositionLeft;
    uint32_t    _snapshotPositionRight;

    /// these are the values used by CruiseControl for PID calculations

    /// these values are used by Navigator for heading control
    float _leftInches;         // distance travelled by left and right wheels
    float _rightInches;
    float _distanceInches;     // distance travelled by the robot (center)
    float _theta;              // current angle (in radians)
    float _xInches;            // x-coordinate
    float _yInches;            // y-coordinate
    float _headingDegrees;     // current heading in degrees


    virtual void        handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs );
    virtual void        handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams );
};
