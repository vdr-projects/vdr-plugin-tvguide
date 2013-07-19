#ifndef __TVGUIDE_RECMENUMANAGER_H
#define __TVGUIDE_RECMENUMANAGER_H

// --- cRecMenuManager  -------------------------------------------------------------
class cRecMenuManager {
private:
    bool active;
    cRecMenu *activeMenu;
    cRecMenu *activeMenuBuffer;
    const cEvent *event;
    cRecManager *recManager;
    std::vector<TVGuideTimerConflict> conflictList;
    std::vector<TVGuideEPGSearchTemplate> epgSearchTemplates;
    bool instantRecord;
    bool folderChoosen;
    int currentConflict;
    int templateID;
    bool searchWithOptions;
    cTimer *timer;
    cString recFolder;
    cString searchString;
    cDetailView *detailView;
    cPixmap *pixmapBackground;
    bool detailViewActive;
    void SetBackground(void);
    void DeleteBackground(void);
    bool displayTimerConflict(cTimer *timer);
    bool displayTimerConflict(int timerID);
    bool createSearchTimer(std::string epgSearchString);
public:
    cRecMenuManager(void);
    virtual ~cRecMenuManager(void);
    bool isActive(void) { return active; };
    void Start(const cEvent *event);
    void Close(void);
    eOSState StateMachine(eRecMenuState nextState);
    eOSState ProcessKey(eKeys Key);
};

#endif //__TVGUIDE_RECMENUMANAGER_H