/*
This file is part of the PubSubsumption library, an implementation of the Subsumption
Architecture based upon a simple Publisher/Subscriber mechanism.

Copyright (C) 2014 Terry Crook

Written by Terry Crook in collaboration with Clayton Dean, and based upon the
Subsumption Architecture as described by David P. Anderson.
*/

#pragma once

#include <CommandDispatcher.h>
#include <Actor.h>

// ControlParams contains the motor control values which are passed through the Subsumption stack
// and end up controlling the motors
class ControlParams 
{
    Actor*      _pTakenBy;  // pointer to the first subsumption layer which takes control

    int         _throttleLeft;
    int         _throttleRight;

    /// CSV output control params
    enum eCsvState { eCsvIdle, eCsvHeadings, eCsvData };

    eCsvState   _csvState;
    char        _csvDelimiter;


public:

    ControlParams() : _throttleLeft( 0 ), _throttleRight( 0 ), _csvState( eCsvIdle ), _csvDelimiter( '\t' )                         {};

    void        ControlledBy( Actor* pActor )           { _pTakenBy = pActor; }
    Actor*      ActorInControl()                        { return _pTakenBy; }

    void        SetThrottles( int left, int right )     { _throttleLeft = left; _throttleRight = right; }
    void        SetLeftThrottle( int left )             { _throttleLeft = left; }
    void        SetRightThrottle( int right )           { _throttleRight = right; }
    int         GetLeftThrottle()                       { return _throttleLeft; }
    int         GetRightThrottle()                      { return _throttleRight; }

    void        SetCsvDelimiter( char delimiter )       { _csvDelimiter = delimiter; }
    char        GetCsvDelimiter()                       { return _csvDelimiter; }
    bool        PrintingCsv()                           { return _csvState != eCsvIdle; }
    bool        PrintingCsvHeadings()                   { return _csvState == eCsvHeadings; }
    bool        PrintingCsvData()                       { return _csvState == eCsvData; }
    void        PrintCsvHeadings()                      { _csvState = eCsvHeadings; }
    void        PrintCsvData()                          { _csvState = eCsvData; }
    void        StopCsvOutput()                         { _csvState = eCsvIdle; }
};


class Director : public Publisher, public Actor
{
    CommandDispatcher* _pCD;

//    Subscriber* _pFirstSubscriber;

    bool            _bEnabled;
    bool            _bInhibit;

    uint16_t        _intervalMS;
    unsigned long   _tickTimeMS;

    // ControlParams object which is passed to all Behaviors through the Publisher's EventNotification.
    ControlParams   _controlParams;

public:
    // interval is the subsumption interval in ms.
    Director( CommandDispatcher* pCD, uint16_t interval );
    ~Director(void);

    /// the Update() function gets called from the Arduino loop() function as frequently as possible.
    /// it checks millis(), and returns if the interval has not elapsed.  When the interval has elapsed,
    /// it sends the subsumption event to the subscribers.
    void Update();

    void PrintHelp( uint8_t eventID );

//    virtual Subscriber* HandleEvent( EventNotification* pEvent );

    virtual void        handleCommandEvent( EventNotification* pEvent, CommandArgs* pArgs );
    virtual void        handleControlEvent( EventNotification* pEvent, ControlParams* pControlParams ) {}  // these would come from the Director
};

