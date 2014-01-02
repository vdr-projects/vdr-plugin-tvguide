#ifndef __TVGUIDE_RECMENUS_H
#define __TVGUIDE_RECMENUS_H

#define TEXTINPUTLENGTH 80

#include <vector>
#include <vdr/epg.h>
#include "recmanager.h"

// --- cRecMenuMain  ---------------------------------------------------------
class cRecMenuMain : public cRecMenu {
public:
    cRecMenuMain(bool epgSearchAvailable, bool timerActive, bool switchTimerActive);
    virtual ~cRecMenuMain(void) {};
};

// --- cRecMenuAskFolder  ---------------------------------------------------------
class cRecMenuAskFolder: public cRecMenu {
private:
    std::vector<cString> folders;
    eRecMenuState NextAction;
    void readFolders(cList<cNestedItem> *rootFolders, cString path);
public:
    cRecMenuAskFolder(const cEvent *event, eRecMenuState nextAction);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuAskFolder(void) {};
};

// --- cRecMenuConfirmTimer  ---------------------------------------------------------
class cRecMenuConfirmTimer: public cRecMenu {
public:
    cRecMenuConfirmTimer(const cEvent *event);
    virtual ~cRecMenuConfirmTimer(void) {};
};

// --- cRecMenuConfirmDeleteTimer  ---------------------------------------------------------
class cRecMenuConfirmDeleteTimer: public cRecMenu {
public:
    cRecMenuConfirmDeleteTimer(const cEvent *event);
    virtual ~cRecMenuConfirmDeleteTimer(void) {};
};

// --- cRecMenuAskDeleteTimer ---------------------------------------------------------
class cRecMenuAskDeleteTimer: public cRecMenu {
public:
    cRecMenuAskDeleteTimer(const cEvent *event);
    virtual ~cRecMenuAskDeleteTimer(void) {};
};

// --- cRecMenuTimerConflicts  ---------------------------------------------------------
class cRecMenuTimerConflicts: public cRecMenu {
public:
    cRecMenuTimerConflicts(cTVGuideTimerConflicts *conflicts);    
    virtual ~cRecMenuTimerConflicts(void) {};
};

// --- cRecMenuTimerConflict  ---------------------------------------------------------
class cRecMenuTimerConflict: public cRecMenu {
private:
    cTVGuideTimerConflict *conflict;
public:
    cRecMenuTimerConflict(cTVGuideTimerConflict *conflict);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuTimerConflict(void) {};
};

// --- cRecMenuNoTimerConflict ---------------------------------------------------------
class cRecMenuNoTimerConflict: public cRecMenu {
public:
    cRecMenuNoTimerConflict(void);   
    virtual ~cRecMenuNoTimerConflict(void) {};
};

// --- cRecMenuRerunResults  ---------------------------------------------------------
class cRecMenuRerunResults: public cRecMenu {
private:
    const cEvent **reruns;
    int numReruns;
public:
    cRecMenuRerunResults(const cEvent *original, const cEvent **reruns, int numReruns);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuRerunResults(void) {
        delete[] reruns;
    };
};

// --- cRecMenuNoRerunsFound  ---------------------------------------------------------
class cRecMenuNoRerunsFound: public cRecMenu {
public:
    cRecMenuNoRerunsFound(cString searchString);
    virtual ~cRecMenuNoRerunsFound(void) {};
};

// --- cRecMenuConfirmRerunUsed  ---------------------------------------------------------
class cRecMenuConfirmRerunUsed: public cRecMenu {
public:
    cRecMenuConfirmRerunUsed(const cEvent *original, const cEvent *replace);
    virtual ~cRecMenuConfirmRerunUsed(void) {};
};

// --- cRecMenuEditTimer  ---------------------------------------------------------
class cRecMenuEditTimer: public cRecMenu {
public:
    cRecMenuEditTimer(const cTimer *timer, eRecMenuState nextState);
    virtual ~cRecMenuEditTimer(void) {};
};


// --- cRecMenuSeriesTimer ---------------------------------------------------------
class cRecMenuSeriesTimer: public cRecMenu {
public:
    cRecMenuSeriesTimer(cChannel *initialChannel, const cEvent *event);
    virtual ~cRecMenuSeriesTimer(void) {};
};

// --- cRecMenuConfirmSeriesTimer  ---------------------------------------------------------
class cRecMenuConfirmSeriesTimer: public cRecMenu {
public:
    cRecMenuConfirmSeriesTimer(cTimer *seriesTimer);    
    virtual ~cRecMenuConfirmSeriesTimer(void) {};
};

// --- cRecMenuSearchTimer  ---------------------------------------------------------
class cRecMenuSearchTimer: public cRecMenu {
private:
    char initialText[TEXTINPUTLENGTH];
public:
    cRecMenuSearchTimer(const cEvent *event);
    virtual ~cRecMenuSearchTimer(void) {};
};

// --- cRecMenuSearchTimerTemplates  ---------------------------------------------------------
class cRecMenuSearchTimerTemplates: public cRecMenu {
private:
    int numTemplates;
    std::vector<TVGuideEPGSearchTemplate> templates;
public:
    cRecMenuSearchTimerTemplates(cString searchString, std::vector<TVGuideEPGSearchTemplate> templates);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuSearchTimerTemplates(void) {};
};

// --- cRecMenuSearchTimerTemplatesCreate  ---------------------------------------------------------
class cRecMenuSearchTimerTemplatesCreate: public cRecMenu {
private:
public:
    cRecMenuSearchTimerTemplatesCreate(cString searchString, cString tmplName);
    virtual ~cRecMenuSearchTimerTemplatesCreate(void) {};
};

// --- cRecMenuSearchTimerOptions  ---------------------------------------------------------
class cRecMenuSearchTimerOptions: public cRecMenu {
private:
    const char * searchModes[5];
    cString searchString;
public:
    cRecMenuSearchTimerOptions(cString searchString);
    void CreateMenuItems(void);
    virtual ~cRecMenuSearchTimerOptions(void) {};
};

// --- cRecMenuSearchTimerResults ---------------------------------------------------------
class cRecMenuSearchTimerResults: public cRecMenu {
private:
    const cEvent **searchResults;
    int numResults;
public:
    cRecMenuSearchTimerResults(cString searchString, const cEvent **searchResults, int numResults, std::string templateName);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuSearchTimerResults(void) {
        delete[] searchResults;
    };
};

// --- cRecMenuSearchTimerNothingFound  ---------------------------------------------------------
class cRecMenuSearchTimerNothingFound: public cRecMenu {
public:
    cRecMenuSearchTimerNothingFound(cString searchString, std::string templateName);
    virtual ~cRecMenuSearchTimerNothingFound(void) {};
};

// --- cRecMenuSearchTimerCreateConfirm  ---------------------------------------------------------
class cRecMenuSearchTimerCreateConfirm: public cRecMenu {
private:
public:
    cRecMenuSearchTimerCreateConfirm(bool success);
    virtual ~cRecMenuSearchTimerCreateConfirm(void) {};
};

// --- cRecMenuSwitchTimer  ---------------------------------------------------------
class cRecMenuSwitchTimer: public cRecMenu {
private:
    const char *switchModes[3];
public:
    cRecMenuSwitchTimer(void);
    virtual ~cRecMenuSwitchTimer(void) {};
};

// --- cRecMenuSwitchTimerConfirm  ---------------------------------------------------------
class cRecMenuSwitchTimerConfirm: public cRecMenu {
private:
public:
    cRecMenuSwitchTimerConfirm(bool success);
    virtual ~cRecMenuSwitchTimerConfirm(void) {};
};

// --- cRecMenuSwitchTimerDelete  ---------------------------------------------------------
class cRecMenuSwitchTimerDelete: public cRecMenu {
private:
public:
    cRecMenuSwitchTimerDelete(void);
    virtual ~cRecMenuSwitchTimerDelete(void) {};
};


// --- cRecMenuSearch  ---------------------------------------------------------
class cRecMenuSearch: public cRecMenu {
private:
    char initialText[TEXTINPUTLENGTH];
    const char * searchModes[5];
public:
    cRecMenuSearch(const cEvent *event);
    cRecMenuSearch(const cEvent *event, const char *searchString);
    virtual ~cRecMenuSearch(void) {};
};

// --- cRecMenuSearchResults  ---------------------------------------------------------
class cRecMenuSearchResults: public cRecMenu {
private:
    const cEvent **searchResults;
    int numResults;
public:
    cRecMenuSearchResults(cString searchString, const cEvent **searchResults, int numResults);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuSearchResults(void) {
        delete[] searchResults;
    };
};

// --- cRecMenuSearchConfirmTimer  ---------------------------------------------------------
class cRecMenuSearchConfirmTimer: public cRecMenu {
public:
    cRecMenuSearchConfirmTimer(const cEvent *event);
    virtual ~cRecMenuSearchConfirmTimer(void) {};
};

// --- cRecMenuSearchNothingFound  ---------------------------------------------------------
class cRecMenuSearchNothingFound: public cRecMenu {
public:
    cRecMenuSearchNothingFound(cString searchString);
    virtual ~cRecMenuSearchNothingFound(void) {};
};

// --- cRecMenuRecordingSearch  ---------------------------------------------------------
class cRecMenuRecordingSearch: public cRecMenu {
private:
    char initialText[TEXTINPUTLENGTH];
public:
    cRecMenuRecordingSearch(const cEvent *event);
    virtual ~cRecMenuRecordingSearch(void) {};
};

// --- cRecMenuRecordingSearchResults  ---------------------------------------------------------
class cRecMenuRecordingSearchResults: public cRecMenu {
private:
    cRecording **searchResults;
    int numResults;
public:
    cRecMenuRecordingSearchResults(cString searchString, cRecording **searchResults, int numResults);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuRecordingSearchResults(void) {
        delete[] searchResults;
    };
};

// --- cRecMenuRecordingSearchNotFound  ---------------------------------------------------------
class cRecMenuRecordingSearchNotFound: public cRecMenu {
public:
    cRecMenuRecordingSearchNotFound(cString searchString);
    virtual ~cRecMenuRecordingSearchNotFound(void) {};
};

// --- cRecMenuTimeline  ---------------------------------------------------------
class cRecMenuTimeline: public cRecMenu {
private:
    std::vector<cTimer*> timersToday;
    int numTimersToday;
    time_t today;
    time_t timeStart;
    time_t timeStop;
    cTVGuideTimerConflicts *timerConflicts;
    std::vector<cTVGuideTimerConflict*> conflictsToday;
    cRecMenuItemTimelineHeader *header;
    void SetStartStop(void);
    void GetTimersForDay(void);
    void SetTimers(void);
    void PrevDay(void);
    void NextDay(void);
    void ClearMenuItems(void);
public:
    cRecMenuTimeline(cTVGuideTimerConflicts *timerConflicts);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuTimeline(void) {
    };
    eRecMenuState ProcessKey(eKeys Key);
};

#endif //__TVGUIDE_RECMENUS_H