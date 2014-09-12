/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

#include <CommandDispatcher.h>
#include <CommandSubscriber.h>


// The WaypointManager maintains a linked list of Waypoint objects,
// supporting appending, inserting, deleting, and modifying (replacing) Waypoints, 
// as well as serialization (load and save) to/from Serial port or flash memory.

class Waypoint
{
public:

    Waypoint( int x, int y, int h ) : _x(x), _y(y), _heading(h) {}

    int _x;
    int _y;
    int _heading;

    Waypoint* _pNextWaypoint;
};

class WaypointManager : public CommandSubscriber
{
    Waypoint* _pFirstWaypoint;
/*
    void    Append( Waypoint* pWaypoint );
    void    Insert( Waypoint* pWaypoint, uint16_t index );
    void    Delete( uint16_t index );
    void    Replace( Waypoint* pWaypoint, uint16_t index );

    void    Load();
    void    Save();

    void    Clear();
*/

public:

    WaypointManager( CommandDispatcher* pCD );
    ~WaypointManager();

    Waypoint*               FirstWaypoint()   { return _pFirstWaypoint; }

    virtual Subscriber*     HandleEvent( EventNotification* pEvent );

    virtual void            PrintHelp( uint8_t eventID );

};