#ifndef __TVGUIDE_RECMENUMANAGER_H
#define __TVGUIDE_RECMENUMANAGER_H

#include "recmenu.h"
#include "recmanager.h"
#include "services/epgsearch.h"
#include "footer.h"

// --- cRecMenuManager  -------------------------------------------------------------
class cRecMenuManager {
private:
    cFooter *footer;
    bool active;
    cRecMenu *activeMenu;
    cRecMenu *activeMenuBuffer;
    cRecMenu *activeMenuBuffer2;
    const cEvent *event;
    const cEvent *displayEvent;
    cRecManager *recManager;
    cTVGuideTimerConflicts *timerConflicts;
    cDetailView *detailView;
    cPixmap *pixmapBackground;
    bool detailViewActive;
    void SetBackground(void);
    void DeleteBackground(void);
    void DisplaySearchTimerList(void);
    bool DisplayTimerConflict(const cTimer *timer);
    bool DisplayTimerConflict(int timerID);
    void DisplayDetailedView(const cEvent *ev);
    void DisplayFavoriteResults(std::string header, const cEvent **result, int numResults);
    eOSState StateMachine(eRecMenuState nextState);
public:
    cRecMenuManager(void);
    virtual ~cRecMenuManager(void);
    void SetFooter(cFooter *footer) { this->footer = footer; };
    bool isActive(void) { return active; };
    void Start(const cEvent *event);
    void StartFavorites(void);
    void Close(void);
    eOSState ProcessKey(eKeys Key);
};

#endif //__TVGUIDE_RECMENUMANAGER_H
