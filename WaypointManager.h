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

    Waypoint( int x = 0, int y = 0, int radius = 0 ) : _x(x), _y(y), _radius(radius) {}

    void Set( int x, int y, int radius ) { _x = x; _y = y; _radius = radius; }

    int         _x;
    int         _y;
    uint8_t     _radius;

//    Waypoint*   _pNextWaypoint;
};

class WaypointManager : public CommandSubscriber
{
//    Waypoint*   _pFirstWaypoint;
    uint16_t    _nextWaypoint;

    Waypoint    _waypoints[10];
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

    Waypoint*               GetWaypoint( uint16_t ixWaypoint )   { return ixWaypoint < _nextWaypoint ? &_waypoints[ ixWaypoint ] : NULL; }
    void                    AppendWaypoint( int x, int y, int radius )       {                _waypoints[ _nextWaypoint++ ].Set( x, y, radius ); }

    virtual Subscriber*     HandleEvent( EventNotification* pEvent );

    virtual void            PrintHelp();

    // Since WaypointManager is not a Behavior, we have to handle this ourselves.
    void                    PrintParameterValues();

};