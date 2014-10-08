/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#include <WaypointManager.h>


WaypointManager::WaypointManager( CommandDispatcher* pCD ) : CommandSubscriber( pCD )
{
    _pName = "Waypoints";

    SubscribeTo( pCD, 'W' );  // WaypointManager commands, of course!

    _nextWaypoint = 0;
}

WaypointManager::~WaypointManager() 
{
}

// we only expect events from the CommandDispatcher
Subscriber* WaypointManager::HandleEvent( EventNotification* pEvent ) 
{
    if ( pEvent && pEvent->eventID == 'W' ) {
        CommandArgs* pArgs = (CommandArgs*) pEvent->pData;
        switch( pArgs->inputBuffer[1] ) {
            case 0 : // no subcommand
                break;
            case '?' : 
                PrintHelp( pEvent->eventID );
                break;
            case 'A' : {// append a waypoint 
                _waypoints[ _nextWaypoint++ ].Set( pArgs->nParams[ 0 ], pArgs->nParams[ 1 ], pArgs->nParams[ 2 ] );
                Serial.println( F( "Waypoint added." ) );
            }
                break;
            case 'I' : // insert a waypoint
                break;
            case 'D' : // delete a waypoint
                break;
            case 'L' : // Load waypoints
                break;
            case 'M' : // modify a waypoint
                break;
            case 'Q' : // query (list waypoints).  Since WaypointManager is not an actor, we have to do this ourselves.
                if ( _nextWaypoint ) {
                    Serial.println( F( "\nDefined waypoints:" ) );
                    Serial.println( F( "x\ty\tradius" ) );
                }
                else {
                    Serial.println( F( "\nNo waypoints defined." ) ) ;
                }

                for ( int ix = 0; ix < _nextWaypoint; ix++ ) {
                    Serial.print( _waypoints[ ix ]._x ); Serial.print( '\t' );
                    Serial.print( _waypoints[ ix ]._y ); Serial.print( '\t' );
                    Serial.println( _waypoints[ ix ]._radius );
                }
                break;
            case 'X' : // clear waypoint list
                _nextWaypoint = 0;

                break;
        }
    }

    return _pNextSub;
}

void WaypointManager::PrintHelp( uint8_t eventID ) 
{
   // we only handle one event, the "W" command:
    Serial.println( F(  "\nWaypoint Manager Control:\n"
                        "  W0: Disable\n"
                        "  W1: Enable\n"
                        "  WA <x> <y> <radius> : Add waypoint\n"
                        "  WI: Insert\n"
                        "  WD: Delete\n"
                        "  WM: Modify\n"
                        "  WL: Load\n"
                        "  WQ: Query\n"
                        "  WX: Clear" 
                        ) );
}
