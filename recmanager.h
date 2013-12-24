#ifndef __TVGUIDE_RECMMANAGER_H
#define __TVGUIDE_RECMMANAGER_H

#include <string>
#include <vector>
#include <vdr/plugin.h>
#include "detailview.h"
#include "recmenu.h"

class TVGuideTimerConflict {
public:
    time_t time;
    time_t timeStart;
    time_t timeStop;
    time_t overlapStart;
    time_t overlapStop;
    int percentPossible;
    int timerID;
    std::vector<int> timerIDs;
    bool timerInvolved(int involvedID);
};

struct TVGuideEPGSearchTemplate {
public:
    std::string name;
    std::string templValue;
};

// --- cRecManager  -------------------------------------------------------------
class cRecManager   {
private:
    cPlugin *epgSearchPlugin;
    bool epgSearchAvailable;
    void DeleteTimer(cTimer *timer);
public:
    cRecManager  (void);
    void SetEPGSearchPlugin(void);
    bool EpgSearchAvailable(void) {return epgSearchAvailable;};
    bool RefreshRemoteTimers(void);
    bool CheckEventForTimer(const cEvent *event);
    cTimer *GetTimerForEvent(const cEvent *event);
    cTimer *createTimer(const cEvent *event, std::string path);
    cTimer *createLocalTimer(const cEvent *event, std::string path);
    cTimer *createRemoteTimer(const cEvent *event, std::string path);
    void SetTimerPath(cTimer *timer, const cEvent *event, std::string path);
    void DeleteTimer(int timerID);
    void DeleteTimer(const cEvent *event);
    void DeleteLocalTimer(const cEvent *event);
    void DeleteRemoteTimer(const cEvent *event);
    void SaveTimer(cTimer *timer, cRecMenu *menu);
    bool IsRecorded(const cEvent *event);
    std::vector<TVGuideTimerConflict> CheckTimerConflict(void);
    cTimer *CreateSeriesTimer(cRecMenu *menu, std::string path);
    std::string BuildEPGSearchString(cString searchString, cRecMenu *menu);
    std::string BuildEPGSearchString(cString searchString, std::string templValue);
    const cEvent **PerformSearchTimerSearch(std::string epgSearchString, int &numResults);
    const cEvent **PerformSearch(cRecMenu *menu, bool withOptions, int &numResults);
    std::vector<TVGuideEPGSearchTemplate> ReadEPGSearchTemplates(void);
    int CreateSearchTimer(std::string epgSearchString);
    void UpdateSearchTimers(void);
    bool CreateSwitchTimer(const cEvent *event, cRecMenu *menu);
    void DeleteSwitchTimer(const cEvent *event);
    cRecording **SearchForRecordings(cString searchString, int &numResults);
    virtual ~cRecManager  (void);
};

#endif //__TVGUIDE_RECMMANAGER_H