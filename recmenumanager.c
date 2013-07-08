#include "recmenumanager.h"

cRecMenuManager::cRecMenuManager(void) {
    active = false;
    activeMenu = NULL;
    activeMenuBuffer = NULL;
    recManager = new cRecManager();
    recManager->SetEPGSearchPlugin();
    instantRecord = false;
    currentConflict = -1;
    templateID = -1;
    timer = NULL;
    searchWithOptions = false;
    detailViewActive = false;
}

cRecMenuManager::~cRecMenuManager(void) {
    if (activeMenu) {
        active = false;
        delete activeMenu;
        activeMenu = NULL;
    }
    delete recManager;
}

void cRecMenuManager::Start(const cEvent *event) {
    active = true;
    activeMenuBuffer = NULL;
    instantRecord = false;
    currentConflict = -1;
    templateID = -1;
    timer = NULL;
    searchWithOptions = false;
    detailViewActive = false;
    SetBackground();
    this->event = event;
    activeMenu = new cRecMenuMain(recManager->EpgSearchAvailable(), event->HasTimer(), SwitchTimers.EventInSwitchList(event));
    activeMenu->Display();
    osdManager.flush();
}

void cRecMenuManager::Close(void) {
    event = NULL;
    active = false;
    if (activeMenu) {
        delete activeMenu;
        activeMenu = NULL;
    }
    DeleteBackground();
}

void cRecMenuManager::SetBackground(void) {
    int backgroundWidth = tvguideConfig.osdWidth;
    int backgroundHeight = tvguideConfig.osdHeight;
    pixmapBackground = osdManager.requestPixmap(3, cRect(0, 0, backgroundWidth, backgroundHeight));
    pixmapBackground->Fill(theme.Color(clrRecMenuBackground));
    if (tvguideConfig.scaleVideo) {
        int tvHeight = tvguideConfig.statusHeaderHeight;
        int tvWidth = tvHeight * 16 / 9;
        int tvX = tvguideConfig.osdWidth - tvWidth;
        pixmapBackground->DrawRectangle(cRect(tvX, 0, tvWidth, tvHeight), clrTransparent);
    }
}

void cRecMenuManager::DeleteBackground(void) {
    osdManager.releasePixmap(pixmapBackground);
}

eOSState cRecMenuManager::StateMachine(eRecMenuState nextState) {
    eOSState state = osContinue;
    switch (nextState) {
        /* 
         * --------- INSTANT RECORDING ---------------------------
        */
        case rmsInstantRecord: {
        //Creating timer for active Event
        //if no conflict, confirm and exit
            instantRecord = true;
            delete activeMenu;
            cTimer *timer = recManager->createTimer(event);
            if (!displayTimerConflict(timer)) {
                activeMenu = new cRecMenuConfirmTimer(event);
                activeMenu->Display();
            }
            break; }
        case rmsIgnoreTimerConflict:
        //Confirming created Timer
            if (instantRecord) {
                delete activeMenu;
                activeMenu = new cRecMenuConfirmTimer(event);
                activeMenu->Display();
            } else {
                state = osEnd;
                Close();
            }
            break;
        case rmsTimerConflictShowInfo: {
            int timerIndex = activeMenu->GetActive(true);
            int timerID = conflictList[currentConflict].timerIDs[timerIndex];
            cTimer *t = Timers.Get(timerID);
            if (t) {
                const cEvent *ev = t->Event();
                if (ev) {
                    activeMenu->Hide();
                    detailView = new cDetailView(ev);
                    detailView->drawHeader();
                    detailView->drawContent();
                    detailView->drawScrollbar();
                    detailViewActive = true;
                }
            }
            break;}
        case rmsDeleteTimerConflictMenu: {
        //delete timer out of current timer conflict
        //active menu: cRecMenuTimerConflict
            int timerIndex = activeMenu->GetActive(true);
            int timerID = conflictList[currentConflict].timerIDs[timerIndex];
            recManager->DeleteTimer(timerID);
            delete activeMenu;
            if (!displayTimerConflict(timerID)) {
                activeMenu = new cRecMenuConfirmTimer(event);
                activeMenu->Display();
            }
            break; }
        case rmsEditTimerConflictMenu: {
        //edit timer out of current timer conflict
        //active menu: cRecMenuTimerConflict
            int activeItem = activeMenu->GetActive(true);
            int timerID = conflictList[currentConflict].timerIDs[activeItem];
            timer = Timers.Get(timerID);
            if (timer) {
                delete activeMenu;
                activeMenu = new cRecMenuEditTimer(timer, rmsSaveTimerConflictMenu);
                activeMenu->Display();
            }
            break; }
        case rmsSaveTimerConflictMenu: {
        //save timer from current timer conflict
            recManager->SaveTimer(timer, activeMenu);
            delete activeMenu;
            if (!displayTimerConflict(timer)) {
                activeMenu = new cRecMenuConfirmTimer(event);
                activeMenu->Display();
            }
            break; }
        case rmsDeleteTimer:
        //delete timer for active event
            delete activeMenu;
            if (recManager->IsRecorded(event)) {
                activeMenu = new cRecMenuAskDeleteTimer(event);
            } else {
                recManager->DeleteTimer(event);
                activeMenu = new cRecMenuConfirmDeleteTimer(event);
            }
            activeMenu->Display();
            break;
        case rmsDeleteTimerConfirmation:
        //delete running timer for active event
            recManager->DeleteTimer(event);
            delete activeMenu;
            activeMenu = new cRecMenuConfirmDeleteTimer(event);
            activeMenu->Display();
            break;
        case rmsEditTimer: {
        //edit timer for active event
            timer = Timers.GetMatch(event);
            if (timer) {
                delete activeMenu;
                activeMenu = new cRecMenuEditTimer(timer, rmsSaveTimer);
                activeMenu->Display();
            }
            break; }
        case rmsSaveTimer: {
        //save timer for active event
            recManager->SaveTimer(timer, activeMenu);
            state = osEnd;
            Close();
            break; }
        /* 
         * --------- SERIES TIMER ---------------------------------
        */
        case rmsSeriesTimer: {
            delete activeMenu;
            cChannel *channel = Channels.GetByChannelID(event->ChannelID());
            activeMenu = new cRecMenuSeriesTimer(channel, event);
            activeMenu->Display();
            break; }
        case rmsSeriesTimerCreate: {
            cTimer *seriesTimer = recManager->CreateSeriesTimer(activeMenu);
            delete activeMenu;
            activeMenu = new cRecMenuConfirmSeriesTimer(seriesTimer);
            activeMenu->Display();
            break; }
        /* 
         * --------- SEARCH TIMER ---------------------------------
        */
        case rmsSearchTimer:
            delete activeMenu;
            activeMenu = new cRecMenuSearchTimer(event);
            activeMenu->Display();
            break;
        case rmsSearchTimerOptions: {
            searchString = *activeMenu->GetStringValue(1);
            delete activeMenu;
            if (isempty(*searchString)) {
                activeMenu = new cRecMenuSearchTimer(event);
            } else {
                epgSearchTemplates = recManager->ReadEPGSearchTemplates();
                int numTemplates = epgSearchTemplates.size();
                if (numTemplates > 0) {
                    activeMenu = new cRecMenuSearchTimerTemplates(searchString, epgSearchTemplates);
                } else {
                    activeMenu = new cRecMenuSearchTimerOptions(searchString);
                }
            }
            activeMenu->Display();
            break; }
        case rmsSearchTimerOptionsReload: {
            int numTemplates = epgSearchTemplates.size();
            delete activeMenu;
            activeMenu = new cRecMenuSearchTimerTemplates(searchString, epgSearchTemplates);
            activeMenu->Display();
            break; }
        case rmsSearchTimerUseTemplate: {
            templateID = activeMenu->GetActive(true) - 1;
            delete activeMenu;
            activeMenu = new cRecMenuSearchTimerTemplatesCreate(searchString, epgSearchTemplates[templateID].name.c_str());
            activeMenu->Display();
            break; }
        case rmsSearchTimerOptionsManually:
            delete activeMenu;
            activeMenu = new cRecMenuSearchTimerOptions(searchString);
            activeMenu->Display();
            break;
        case rmsSearchTimerTestTemplate: {
            std::string epgSearchString = recManager->BuildEPGSearchString(searchString, epgSearchTemplates[templateID].templValue);
            int numSearchResults = 0;
            const cEvent **searchResult = recManager->PerformSearchTimerSearch(epgSearchString, numSearchResults);
            if (searchResult) {
                activeMenuBuffer = activeMenu;
                activeMenuBuffer->Hide();
                activeMenu = new cRecMenuSearchTimerResults(searchString, searchResult, numSearchResults, epgSearchTemplates[templateID].name);
                activeMenu->Display();
            } else {
               activeMenuBuffer = activeMenu;
               activeMenuBuffer->Hide();
               activeMenu = new cRecMenuSearchTimerNothingFound(searchString, epgSearchTemplates[templateID].name);
               activeMenu->Display();
            }
            break; }
        case rmsSearchTimerTestManually: {
            std::string epgSearchString = recManager->BuildEPGSearchString(searchString, activeMenu);
            int numSearchResults = 0;
            const cEvent **searchResult = recManager->PerformSearchTimerSearch(epgSearchString, numSearchResults);
            if (searchResult) {
                activeMenuBuffer = activeMenu;
                activeMenuBuffer->Hide();
                activeMenu = new cRecMenuSearchTimerResults(searchString, searchResult, numSearchResults, "");
                activeMenu->Display();
            } else {
               activeMenuBuffer = activeMenu;
               activeMenuBuffer->Hide();
               activeMenu = new cRecMenuSearchTimerNothingFound(searchString, "");
               activeMenu->Display();
            }
            break; }
        case rmsSearchTimerNothingFoundConfirm:
            delete activeMenu;
            activeMenu = activeMenuBuffer;
            activeMenuBuffer = NULL;
            activeMenu->Show();
            break;
        case rmsSearchTimerCreateManually:
        case rmsSearchTimerCreateTemplate: {
            std::string epgSearchString;
            if (nextState == rmsSearchTimerCreateManually) {
                epgSearchString = recManager->BuildEPGSearchString(searchString, activeMenu);
            } else if (nextState = rmsSearchTimerCreateTemplate) {
                epgSearchString = recManager->BuildEPGSearchString(searchString, epgSearchTemplates[templateID].templValue);
            }
            bool success = createSearchTimer(epgSearchString);
            delete activeMenu;
            activeMenu = new cRecMenuSearchTimerCreateConfirm(success);
            activeMenu->Display();
            break; }
        /* 
         * --------- SWITCH TIMER ---------------------------------
        */
        case rmsSwitchTimer:
            delete activeMenu;
            activeMenu = new cRecMenuSwitchTimer();
            activeMenu->Display();
            break;
        case rmsSwitchTimerCreate: {
            bool success = recManager->CreateSwitchTimer(event, activeMenu);
            delete activeMenu;
            activeMenu = new cRecMenuSwitchTimerConfirm(success);
            activeMenu->Display();
            break; }
        case rmsSwitchTimerDelete: 
            recManager->DeleteSwitchTimer(event);
            delete activeMenu;
            activeMenu = new cRecMenuSwitchTimerDelete();
            activeMenu->Display();
            break;
        /* 
         * --------- RECORDINGS SEARCH ---------------------------------
        */
        case rmsRecordingSearch:
            delete activeMenu;
            activeMenu = new cRecMenuRecordingSearch(event);
            activeMenu->Display();
            break;
        case rmsRecordingSearchResult:  {
            searchString = activeMenu->GetStringValue(1);
            delete activeMenu;
            if (isempty(*searchString)) {
                activeMenu = new cRecMenuRecordingSearch(event);
            } else {
                int numSearchResults = 0;
                cRecording **searchResult = recManager->SearchForRecordings(searchString, numSearchResults);
                if (numSearchResults == 0) {
                    activeMenu = new cRecMenuRecordingSearchNotFound(searchString);
                } else {
                    activeMenu = new cRecMenuRecordingSearchResults(searchString, searchResult, numSearchResults);
                }
            }
            activeMenu->Display();
            break; }
        /* 
         * --------- SEARCH ---------------------------------
        */
        case rmsSearch:
            delete activeMenu;
            activeMenu = new cRecMenuSearch(event);
            activeMenu->Display();
            searchWithOptions = false;
            break;
        case rmsSearchWithOptions: {
            cString searchString = activeMenu->GetStringValue(1);
            delete activeMenu;
            if (isempty(*searchString)) {
                activeMenu = new cRecMenuSearch(event);
            } else {
                activeMenu = new cRecMenuSearch(event, *searchString);
                searchWithOptions = true;
            }
            activeMenu->Display();
            break; }
        case rmsSearchPerform: {
            cString searchString = activeMenu->GetStringValue(1);
            if (isempty(*searchString)) {
                delete activeMenu;
                activeMenu = new cRecMenuSearch(event);
            } else {
                int numSearchResults = 0;
                const cEvent **searchResult = recManager->PerformSearch(activeMenu, searchWithOptions, numSearchResults);
                if (searchResult) {
                    delete activeMenu;
                    activeMenu = new cRecMenuSearchResults(searchString, searchResult, numSearchResults);
                } else {
                    activeMenuBuffer = activeMenu;
                    activeMenuBuffer->Hide();
                    activeMenu = new cRecMenuSearchNothingFound(searchString);
                }
            }
            activeMenu->Display();
            break; }
        case rmsSearchNothingFoundConfirm:
            delete activeMenu;
            activeMenu = activeMenuBuffer;
            activeMenuBuffer = NULL;
            activeMenu->Show();
            break;
        case rmsSearchShowInfo: {
            const cEvent *ev = activeMenu->GetEventValue(activeMenu->GetActive(false));
            if (ev) {
                activeMenu->Hide();
                detailView = new cDetailView(ev);
                detailView->drawHeader();
                detailView->drawContent();
                detailView->drawScrollbar();
                detailViewActive = true;
            }
            break;}
        case rmsSearchRecord: {
            const cEvent *ev = activeMenu->GetEventValue(activeMenu->GetActive(false));
            cTimer *timer = recManager->createTimer(ev);
            activeMenuBuffer = activeMenu;
            activeMenuBuffer->Hide();
            activeMenu = new cRecMenuSearchConfirmTimer(ev);
            activeMenu->Display();
            break;}
        case rmsSearchRecordConfirm:
            delete activeMenu;
            activeMenu = activeMenuBuffer;
            activeMenuBuffer = NULL;
            activeMenu->Show();
            break;
        /* 
         * --------- CHECK FOR TIMER CONFLICTS ---------------------------------
        */
        case rmsTimerConflicts: {
        //Show timer conflict
        //active menu: cRecMenuTimerConflicts
            conflictList = recManager->CheckTimerConflict();
            delete activeMenu;
            int numConflicts = conflictList.size();
            if (numConflicts > 0) {
                activeMenu = new cRecMenuTimerConflicts(conflictList);
            } else {
                activeMenu = new cRecMenuNoTimerConflict();
            }
            activeMenu->Display();
            break; }
        case rmsTimerConflict:
        //Show timer conflict
        //active menu: cRecMenuTimerConflicts
            currentConflict = activeMenu->GetActive(true);
            delete activeMenu;
            activeMenu = new cRecMenuTimerConflict(conflictList[currentConflict]);
            activeMenu->Display();
            break;

        /* 
         * --------- COMMON ---------------------------------
        */
        case rmsClose: {
            if (activeMenuBuffer == NULL) {
                state = osEnd;
                Close();
            } else {
                delete activeMenu;
                activeMenu = activeMenuBuffer;
                activeMenuBuffer = NULL;
                activeMenu->Show();
                state = osContinue;
            }
            break; }
        default:        
            break;
    }
    return state;
}

bool cRecMenuManager::displayTimerConflict(cTimer *timer) {
    int timerID = 0;
    for (cTimer *t = Timers.First(); t; t = Timers.Next(t)) {
        if (t == timer)
            return displayTimerConflict(timerID);
        timerID++;
    }
    return false;
}

bool cRecMenuManager::displayTimerConflict(int timerID) {
    conflictList = recManager->CheckTimerConflict();
    int numConflicts = conflictList.size();
    int showTimerConflict = -1;
    if (numConflicts > 0) {
        for (int i=0; i<numConflicts; i++) {
            if (conflictList[i].timerInvolved(timerID)) {
                showTimerConflict = i;
                break;
            }
        }
    }
    if (showTimerConflict > -1) {
        currentConflict = showTimerConflict;
        activeMenu = new cRecMenuTimerConflict(conflictList[currentConflict]);
        activeMenu->Display();
        return true;
    }
    return false;
}

bool cRecMenuManager::createSearchTimer(std::string epgSearchString) {
    int newTimerID = recManager->CreateSearchTimer(epgSearchString);
    bool success = false;
    if (newTimerID > -1) {
        recManager->UpdateSearchTimers();
        success = true;
    }
    return success;
}

eOSState cRecMenuManager::ProcessKey(eKeys Key) {
    eOSState state = osContinue;
    eRecMenuState nextState = rmsContinue;
    if (!activeMenu)
        return state;
    if (detailViewActive) {
        state = detailView->ProcessKey(Key);
        if (state == osEnd) {
            delete detailView;
            detailView = NULL;
            detailViewActive = false;
            activeMenu->Show();
            state = osContinue;
        }
    } else {
        nextState = activeMenu->ProcessKey(Key);
        if ((nextState == rmsClose) || ((nextState == rmsNotConsumed)&&(Key == kBack))){
            if (activeMenuBuffer == NULL) {
                state = osEnd;
                Close();
            } else {
                delete activeMenu;
                activeMenu = activeMenuBuffer;
                activeMenuBuffer = NULL;
                activeMenu->Show();
                state = osContinue;
                osdManager.flush();
            }
            return state;
        }
        state = StateMachine(nextState);
    }
    osdManager.flush();
    return state;
}