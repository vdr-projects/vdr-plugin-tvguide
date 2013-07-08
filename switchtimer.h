#ifndef __TVGUIDE_SWITCHTIMER_H
#define __TVGUIDE_SWITCHTIMER_H

#include <vdr/plugin.h>

class cSwitchTimer : public cListObject
{
public:
    tEventID eventID;
    time_t startTime;
    tChannelID channelID;

    cSwitchTimer(void);
    cSwitchTimer(const cEvent* Event);
    bool Parse(const char *s);
};

class cSwitchTimers : public cConfig<cSwitchTimer>, public cMutex
{
public:
    cSwitchTimers(void) {}
    ~cSwitchTimers(void) {}
    bool EventInSwitchList(const cEvent* event);
    bool ChannelInSwitchList(const cChannel* channel);
    void DeleteSwitchTimer(const cEvent *event);
};

extern cSwitchTimers SwitchTimers;

#endif //__TVGUIDE_SWITCHTIMER_H