#ifndef __TVGUIDE_RECMENUS_H
#define __TVGUIDE_RECMENUS_H

#define TEXTINPUTLENGTH 256

#include <vector>
#include <functional>
#include <vdr/epg.h>
#include "services/epgsearch.h"
#include "switchtimer.h"
#include "recmanager.h"

typedef enum {
    searchTimerActionRecord = 0,
    searchTimerActionAnnounceViaOSD,
    searchTimerActionSwitchOnly,
    searchTimerActionAnnounceAndSwitch,
    searchTimerActionAnnounceViaMail,
    searchTimerActionInactiveRecord
} searchTimerAction;

// --- cRecMenuMain  ---------------------------------------------------------
class cRecMenuMain : public cRecMenu {
public:
    cRecMenuMain(bool epgSearchAvailable, bool timerActive, bool switchTimerActive);
    virtual ~cRecMenuMain(void) {};
};

/******************************************************************************************
*   Instant Timer Menus
******************************************************************************************/


// --- cRecMenuAskFolder  ---------------------------------------------------------
class cRecMenuAskFolder: public cRecMenu {
private:
    std::vector<std::string> folders;
    eRecMenuState NextAction;
public:
    cRecMenuAskFolder(const cEvent *event, eRecMenuState nextAction);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuAskFolder(void) {};
    std::string GetFolder(void);
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
    int GetTimerConflict(void);
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
    int GetTimerConflictIndex(void);
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
    const cEvent *GetRerunEvent(void);
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
private:
    const cTimer *originalTimer;
    bool timerActive;
    time_t day;
    int start;
    int stop;
    int prio;
    int lifetime;
    char folder[TEXTINPUTLENGTH];
public:
    cRecMenuEditTimer(const cTimer *timer, eRecMenuState nextState);
    const cTimer *GetOriginalTimer(void);
    virtual ~cRecMenuEditTimer(void) {};
    cTimer GetTimer(void);
};

/******************************************************************************************
*   Series Timer Menus
******************************************************************************************/


// --- cRecMenuSeriesTimer ---------------------------------------------------------
class cRecMenuSeriesTimer: public cRecMenu {
    std::string folder;
    bool timerActive;
    int channel;
    time_t tstart;
    int start;
    int stop;
    int dayOfWeek;
    int priority;
    int lifetime;
    void CalculateTimes(const cEvent *event);
public:
    cRecMenuSeriesTimer(const cChannel *initialChannel, const cEvent *event, std::string folder);
    virtual ~cRecMenuSeriesTimer(void) {};
    cTimer *GetTimer(void);
};

// --- cRecMenuConfirmSeriesTimer  ---------------------------------------------------------
class cRecMenuConfirmSeriesTimer: public cRecMenu {
public:
    cRecMenuConfirmSeriesTimer(cTimer *seriesTimer);    
    virtual ~cRecMenuConfirmSeriesTimer(void) {};
};

/******************************************************************************************
*   SearchTimer Menus
******************************************************************************************/

// --- cRecMenuSearchTimer  ---------------------------------------------------------
class cRecMenuSearchTimer: public cRecMenu {
private:
    char searchString[TEXTINPUTLENGTH];
public:
    cRecMenuSearchTimer(const cEvent *event);
    virtual ~cRecMenuSearchTimer(void) {};
    std::string GetSearchString(void) { return searchString; };
};

// --- cRecMenuSearchTimerTemplates  ---------------------------------------------------------
class cRecMenuSearchTimerTemplates: public cRecMenu {
private:
    int numTemplates;
    cTVGuideSearchTimer searchTimer;
    std::vector<TVGuideEPGSearchTemplate> templates;
public:
    cRecMenuSearchTimerTemplates(cTVGuideSearchTimer searchTimer, std::vector<TVGuideEPGSearchTemplate> templates);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuSearchTimerTemplates(void) {};
    cTVGuideSearchTimer GetSearchTimer(void) { return searchTimer; };
    TVGuideEPGSearchTemplate GetTemplate(void);
};


// --- cRecMenuSearchTimers  ---------------------------------------------------------
class cRecMenuSearchTimers: public cRecMenu {
private:
    int numSearchTimers;
    std::vector<cTVGuideSearchTimer> searchTimers;
    void SetMenuItems(void);
public:
    cRecMenuSearchTimers(std::vector<cTVGuideSearchTimer> searchTimers);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    cTVGuideSearchTimer GetSearchTimer(void);
    virtual ~cRecMenuSearchTimers(void);
};

// --- cRecMenuSearchTimerEdit  ---------------------------------------------------------
class cRecMenuSearchTimerEdit: public cRecMenu {
private:
    cTVGuideSearchTimer searchTimer;
    cTVGuideSearchTimer sT;
    std::vector<std::string> searchModes;
    std::vector<std::string> useChannelModes;
    std::vector<std::string> channelGroups;
    std::vector<std::string> compareDateModes;
    std::vector<std::string> searchTimerModes;
    std::vector<std::string> delModes;
    std::vector<std::string> channelgroups;
    std::vector<cRecMenuItem*> mainMenuItems;
    bool init;
    int numMenuItems;
    int useChannelPos;
    int useTimePos;
    int useDayOfWeekPos;
    int avoidRepeatsPos;
    char searchString[TEXTINPUTLENGTH];
    bool timerActive;
    int startChannel;
    int stopChannel;
    int channelgroupIndex;
    char directory[TEXTINPUTLENGTH];
    int SplitChannelGroups(std::vector<std::string> *channelGroups, std::vector<std::string> *channelgroups);
public:
    cRecMenuSearchTimerEdit(cTVGuideSearchTimer searchTimer, std::vector<std::string> channelGroups);
    void CreateMenuItems(void);
    virtual ~cRecMenuSearchTimerEdit(void);
    cTVGuideSearchTimer GetSearchTimer(void);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
};

// --- cRecMenuSearchTimerDeleteConfirm ---------------------------------------------
class cRecMenuSearchTimerDeleteConfirm: public cRecMenu {
private:
    cTVGuideSearchTimer searchTimer;
public:
    cRecMenuSearchTimerDeleteConfirm(cTVGuideSearchTimer searchTimer);
    virtual ~cRecMenuSearchTimerDeleteConfirm(void);
    cTVGuideSearchTimer GetSearchTimer(void);
};

// --- cRecMenuSearchTimerCreateConfirm  ---------------------------------------------------------
class cRecMenuSearchTimerCreateConfirm: public cRecMenu {
private:
public:
    cRecMenuSearchTimerCreateConfirm(bool success);
    virtual ~cRecMenuSearchTimerCreateConfirm(void) {};
};

// --- cRecMenuSearchTimerTemplatesCreate  ---------------------------------------------------------
class cRecMenuSearchTimerTemplatesCreate: public cRecMenu {
private:
    TVGuideEPGSearchTemplate templ;
    cTVGuideSearchTimer searchTimer;
public:
    cRecMenuSearchTimerTemplatesCreate(TVGuideEPGSearchTemplate templ, cTVGuideSearchTimer searchTimer);
    virtual ~cRecMenuSearchTimerTemplatesCreate(void) {};
    cTVGuideSearchTimer GetSearchTimer(void) { return searchTimer; };
    TVGuideEPGSearchTemplate GetTemplate(void) { return templ; };
};

// --- cRecMenuSearchTimerResults ---------------------------------------------------------
class cRecMenuSearchTimerResults: public cRecMenu {
private:
    const cEvent **searchResults;
    int numResults;
    eRecMenuState action2;
public:
    cRecMenuSearchTimerResults(std::string searchString, const cEvent **searchResults, int numResults, std::string templateName = "", eRecMenuState action2 = rmsDisabled);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuSearchTimerResults(void) {
        delete[] searchResults;
    };
    const cEvent *GetEvent(void);
};

// --- cRecMenuSearchTimerNothingFound  ---------------------------------------------------------
class cRecMenuSearchTimerNothingFound: public cRecMenu {
public:
    cRecMenuSearchTimerNothingFound(std::string searchString);
    virtual ~cRecMenuSearchTimerNothingFound(void) {};
};

/******************************************************************************************
*   SwitchTimer Menus
******************************************************************************************/

// --- cRecMenuSwitchTimer  ---------------------------------------------------------
class cRecMenuSwitchTimer: public cRecMenu {
private:
    int switchMinsBefore;
    int switchMode;
public:
    cRecMenuSwitchTimer(void);
    virtual ~cRecMenuSwitchTimer(void) {};
    cSwitchTimer GetSwitchTimer(void);
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

/******************************************************************************************
*   Search Menus
******************************************************************************************/

// --- cRecMenuSearch  ---------------------------------------------------------
class cRecMenuSearch: public cRecMenu {
private:
    char searchString[TEXTINPUTLENGTH];
    int mode;
    int channelNr;
    bool useTitle;
    bool useSubTitle;
    bool useDescription;
public:
    cRecMenuSearch(std::string searchString, bool withOptions);
    virtual ~cRecMenuSearch(void) {};
    Epgsearch_searchresults_v1_0 GetEPGSearchStruct(void);
    std::string GetSearchString(void) { return searchString; };
};

// --- cRecMenuSearchResults  ---------------------------------------------------------
class cRecMenuSearchResults: public cRecMenu {
private:
    std::string searchString;
    const cEvent **searchResults;
    int numResults;
public:
    cRecMenuSearchResults(std::string searchString, const cEvent **searchResults, int numResults);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuSearchResults(void) {
        delete[] searchResults;
    };
    const cEvent *GetEvent(void);
    std::string GetSearchString(void) { return searchString; };
};

// --- cRecMenuSearchConfirmTimer  ---------------------------------------------------------
class cRecMenuSearchConfirmTimer: public cRecMenu {
public:
    cRecMenuSearchConfirmTimer(const cEvent *event, eRecMenuState nextAction);
    virtual ~cRecMenuSearchConfirmTimer(void) {};
};

// --- cRecMenuSearchNothingFound  ---------------------------------------------------------
class cRecMenuSearchNothingFound: public cRecMenu {
public:
    cRecMenuSearchNothingFound(std::string searchString, bool tooShort = false);
    virtual ~cRecMenuSearchNothingFound(void) {};
};

/******************************************************************************************
*   Recording Search Menus
******************************************************************************************/


// --- cRecMenuRecordingSearch  ---------------------------------------------------------
class cRecMenuRecordingSearch: public cRecMenu {
private:
    char searchString[TEXTINPUTLENGTH];
public:
    cRecMenuRecordingSearch(std::string search);
    virtual ~cRecMenuRecordingSearch(void) {};
    std::string GetSearchString(void) { return searchString; };
};

// --- cRecMenuRecordingSearchResults  ---------------------------------------------------------
class cRecMenuRecordingSearchResults: public cRecMenu {
private:
    std::string searchString;
    const cRecording **searchResults;
    int numResults;
public:
    cRecMenuRecordingSearchResults(std::string searchString, const cRecording **searchResults, int numResults);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    virtual ~cRecMenuRecordingSearchResults(void) {
        delete[] searchResults;
    };
    std::string GetSearchString(void) { return searchString; };
};

// --- cRecMenuRecordingSearchNotFound  ---------------------------------------------------------
class cRecMenuRecordingSearchNotFound: public cRecMenu {
public:
    cRecMenuRecordingSearchNotFound(std::string searchString);
    virtual ~cRecMenuRecordingSearchNotFound(void) {};
};

/******************************************************************************************
*   Timeline
******************************************************************************************/

// --- cRecMenuTimeline  ---------------------------------------------------------
class cRecMenuTimeline: public cRecMenu {
private:
    std::vector<const cTimer*> timersToday;
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
    void ClearMenu(void);
public:
    cRecMenuTimeline(cTVGuideTimerConflicts *timerConflicts);
    virtual ~cRecMenuTimeline(void) {};
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    eRecMenuState ProcessKey(eKeys Key);
    const cTimer *GetTimer(void);
};

/******************************************************************************************
*   Favorites
******************************************************************************************/

// --- cRecMenuFavorites  ---------------------------------------------------------
class cRecMenuFavorites: public cRecMenu {
private:
    std::vector<cTVGuideSearchTimer> favorites;
    std::vector<cRecMenuItem*> myMenuItems;
    int numFavorites;
    void CreateFavoritesMenuItems(void);
    std::string NiceTime(int favTime);
public:
    cRecMenuFavorites(std::vector<cTVGuideSearchTimer> favorites);
    cRecMenuItem *GetMenuItem(int number);
    int GetTotalNumMenuItems(void);
    cTVGuideSearchTimer GetFavorite(void);
    virtual ~cRecMenuFavorites(void);
};

#endif //__TVGUIDE_RECMENUS_H
