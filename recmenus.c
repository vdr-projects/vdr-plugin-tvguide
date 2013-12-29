#include "services/remotetimers.h"
#include "recmenus.h"

// --- cRecMenuMain  ---------------------------------------------------------
cRecMenuMain::cRecMenuMain(bool epgSearchAvailable, bool timerActive, bool switchTimerActive) {
    eRecMenuState action;
    if (!timerActive) {
        action = (tvguideConfig.recMenuAskFolder)
                  ?rmsInstantRecordFolder
                  :rmsInstantRecord;
        AddMenuItem(new cRecMenuItemButton(tr("Instant Record"), action, true));
    } else {
        AddMenuItem(new cRecMenuItemButton(tr("Delete Timer"), rmsDeleteTimer, true));
        AddMenuItem(new cRecMenuItemButton(tr("Edit Timer"), rmsEditTimer, false));
    }
    action = (tvguideConfig.recMenuAskFolder)
              ?rmsSeriesTimerFolder
              :rmsSeriesTimer;
    AddMenuItem(new cRecMenuItemButton(tr("Create Series Timer"), action, false));
    if (epgSearchAvailable) {
        AddMenuItem(new cRecMenuItemButton(tr("Create Search Timer"), rmsSearchTimer, false));
        if (!switchTimerActive) {
            AddMenuItem(new cRecMenuItemButton(tr("Create Switch Timer"), rmsSwitchTimer, false));
        } else {
            AddMenuItem(new cRecMenuItemButton(tr("Delete Switch Timer"), rmsSwitchTimerDelete, false));
        }
    }
    if (epgSearchAvailable) {
        AddMenuItem(new cRecMenuItemButton(tr("Search"), rmsSearch, false));
    }
    AddMenuItem(new cRecMenuItemButton(tr("Search in Recordings"), rmsRecordingSearch, false));
    if (epgSearchAvailable) {
        AddMenuItem(new cRecMenuItemButton(tr("Check for Timer Conflicts"), rmsTimerConflicts, false));
    }
    int menuWidth = CalculateOptimalWidth() + 4 * border;
    SetWidthPixel(menuWidth);
            
    CalculateHeight();
    CreatePixmap();
    Arrange();
};

// --- cRecMenuAskFolder  ---------------------------------------------------------
cRecMenuAskFolder::cRecMenuAskFolder(const cEvent *event, eRecMenuState nextAction) {
    SetWidthPercent(80);
    NextAction = nextAction;
    cString message = tr("Set Folder for");
    cString headerText = cString::sprintf("%s\n\"%s\"", *message, event->Title());
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*headerText);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    AddMenuItemScroll(new cRecMenuItemButton(tr("root video folder"), nextAction, true, false, true));
    
    readFolders(NULL, "");
    int numFolders = folders.size();
    for (int i=0; i < numFolders; i++) {
        AddMenuItemScroll(new cRecMenuItemButton(*folders[i], nextAction, false, false, true));
        if (!CheckHeight())
            break;
    }
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

void cRecMenuAskFolder::readFolders(cList<cNestedItem> *rootFolders, cString path) {
    cList<cNestedItem> *foldersLevel = NULL;
    if (rootFolders) {
        foldersLevel = rootFolders;
    } else {
        foldersLevel = &Folders;
    }
    for (cNestedItem *folder = foldersLevel->First(); folder; folder = foldersLevel->Next(folder)) {
        cString strFolder = cString::sprintf("%s%s", *path, folder->Text());
        folders.push_back(strFolder);
        cList<cNestedItem> *subItems = folder->SubItems();
        if (subItems) {
            cString newPath = cString::sprintf("%s%s/", *path, folder->Text());
            readFolders(subItems, newPath);
        }
    }
}

cRecMenuItem *cRecMenuAskFolder::GetMenuItem(int number) { 
    if (number == 0) {
        cRecMenuItem *result = new cRecMenuItemButton(tr("root video folder"), rmsInstantRecord, false, false, true);
        return result;
    } else if ((number > 0) && (number < folders.size()+1)) {
        cRecMenuItem *result = new cRecMenuItemButton(*folders[number-1], rmsInstantRecord, false, false, true);
        return result;
    }
    return NULL;
}

int cRecMenuAskFolder::GetTotalNumMenuItems(void) { 
    return folders.size()+1;
}

// --- cRecMenuConfirmTimer  ---------------------------------------------------------
cRecMenuConfirmTimer::cRecMenuConfirmTimer(const cEvent *event) {
    SetWidthPercent(50);
    cString channelName = Channels.GetByChannelID(event->ChannelID())->Name();
    cString message;
    bool eventHasTimer = false;
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_GetMatch_v1_0 rtMatch;
        rtMatch.event = event;
        pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
        if (rtMatch.timerMatch == tmFull) {
            eventHasTimer = true;                
        }
    } else {
        eventHasTimer = event->HasTimer();
    }
    if (eventHasTimer) {
        message = tr("Timer created");
    } else {
        message = tr("Timer NOT created");
    }
    cString text = cString::sprintf("%s\n%s\n%s %s - %s\n%s", 
                                    *message, 
                                    *channelName,
                                    *event->GetDateString(),
                                    *event->GetTimeString(),
                                    *event->GetEndTimeString(),
                                    event->Title()
                                );
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuConfirmDeleteTimer  ---------------------------------------------------------
cRecMenuConfirmDeleteTimer::cRecMenuConfirmDeleteTimer(const cEvent *event) {
    SetWidthPercent(50);
    cString channelName = Channels.GetByChannelID(event->ChannelID())->Name();
    cString text = cString::sprintf("%s\n%s\n%s %s - %s\n%s", 
                                    tr("Timer deleted"), 
                                    *channelName,
                                    *event->GetDateString(),
                                    *event->GetTimeString(),
                                    *event->GetEndTimeString(),
                                    event->Title()
                                );
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 4 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true, true));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuAskDeleteTimer ---------------------------------------------------------
cRecMenuAskDeleteTimer::cRecMenuAskDeleteTimer(const cEvent *event) {
    SetWidthPercent(50);
    cString channelName = Channels.GetByChannelID(event->ChannelID())->Name();
    cString text = cString::sprintf("%s \"%s, %s\" %s", 
                                    tr("Timer"),
                                    *channelName,
                                    event->Title(),
                                    tr("still recording - really delete?"));
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 4 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Yes"), tr("No"), rmsDeleteTimerConfirmation, rmsClose, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuTimerConflicts  ---------------------------------------------------------
cRecMenuTimerConflicts::cRecMenuTimerConflicts(cTVGuideTimerConflicts *conflicts) {
    int numConflicts = conflicts->NumConflicts();
    
    cString text;
    if (numConflicts == 1) {
        text = cString::sprintf("%s %s %s", tr("One"), tr("Timer Conflict"), tr("detected"));
    } else {
        text = cString::sprintf("%d %s %s", conflicts->NumConflicts(), tr("Timer Conflicts"), tr("detected"));
    }
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    
    for (int i=0; i<numConflicts; i++) {
        cTVGuideTimerConflict *conflict = conflicts->GetConflict(i);
        if (!conflict)
            continue;
        cString dateTime = DayDateTime(conflict->time);
        int numTimers = conflict->timerIDs.size();
        cString textConflict = cString::sprintf("%s: %s (%d %s)", tr("Show conflict"), *dateTime, numTimers, tr("timers involved"));
        bool isActive = (i==0)?true:false;
        AddMenuItem(new cRecMenuItemButton(*textConflict, rmsTimerConflict, isActive));
    }
    
    SetWidthPixel(CalculateOptimalWidth() + 4*border);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("Ignore Conflicts"), rmsIgnoreTimerConflict, false));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuTimerConflict  ---------------------------------------------------------
cRecMenuTimerConflict::cRecMenuTimerConflict(cTVGuideTimerConflict *conflict) {
    SetWidthPercent(95);
    this->conflict = conflict;
    SetHeader(new cRecMenuItemTimerConflictHeader(conflict->timeStart, 
                                                  conflict->timeStop, 
                                                  conflict->overlapStart, 
                                                  conflict->overlapStop));
    SetFooter(new cRecMenuItemButton(tr("Ignore Conflict"), rmsIgnoreTimerConflict, false, true));
    int i=0;
    for(std::vector<int>::iterator it = conflict->timerIDs.begin(); it != conflict->timerIDs.end(); it++) {
        const cTimer *timer = Timers.Get(*it);
        if (timer) {
            AddMenuItemScroll(new cRecMenuItemTimer(  timer,
                                                      rmsTimerConflictShowInfo,
                                                      rmsDeleteTimerConflictMenu, 
                                                      rmsEditTimerConflictMenu,
                                                      rmsSearchRerunsTimerConflictMenu,
                                                      conflict->timeStart, 
                                                      conflict->timeStop, 
                                                      conflict->overlapStart, 
                                                      conflict->overlapStop, 
                                                      (!i)?true:false)
            );
            i++;
        }
        if (!CheckHeight())
            break;                
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}
    
cRecMenuItem *cRecMenuTimerConflict::GetMenuItem(int number) { 
    if ((number >= 0) && (number < conflict->timerIDs.size())) {
        const cTimer *timer = Timers.Get(conflict->timerIDs[number]);
        cRecMenuItem *result = new cRecMenuItemTimer( timer,
                                                      rmsTimerConflictShowInfo,
                                                      rmsDeleteTimerConflictMenu, 
                                                      rmsEditTimerConflictMenu,
                                                      rmsSearchRerunsTimerConflictMenu,
                                                      conflict->timeStart, 
                                                      conflict->timeStop, 
                                                      conflict->overlapStart, 
                                                      conflict->overlapStop, 
                                                      false);
        return result;
    }
    return NULL;
}

int cRecMenuTimerConflict::GetTotalNumMenuItems(void) { 
    return conflict->timerIDs.size(); 
}

// --- cRecMenuNoTimerConflict ---------------------------------------------------------
cRecMenuNoTimerConflict::cRecMenuNoTimerConflict(void) {
    SetWidthPercent(50);
    cString text = tr("No Timer Conflicts found");
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 4 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("Close"), rmsClose, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuRerunResults ---------------------------------------------------------
cRecMenuRerunResults::cRecMenuRerunResults(const cEvent *original, const cEvent **reruns, int numReruns) {
    this->reruns = reruns;
    this->numReruns = numReruns;
    SetWidthPercent(70);
    this->numReruns = numReruns;
    cString message1 = tr("reruns for");
    cString message2 = tr("rerun for");
    cString message3 = tr("found");
    cString infoText = cString::sprintf("%d %s:\n\"%s\" %s", numReruns, (numReruns>1)?(*message1):(*message2), original->Title(), *message3);
    cRecMenuItem *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    cRecMenuItem *button = new cRecMenuItemButton(tr("Ignore reruns"), rmsTimerConflictIgnoreReruns, false);
    SetFooter(button);
    
    if (reruns && (numReruns > 0)) {
        for (int i=0; i<numReruns; i++) {
            AddMenuItemScroll(new cRecMenuItemEvent(reruns[i], rmsSearchShowInfo, rmsTimerConflictRecordRerun, (i==0)?true:false));
            if (!CheckHeight())
                break;
        }
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cRecMenuItem *cRecMenuRerunResults::GetMenuItem(int number) {
   if ((number >= 0) && (number < numReruns)) {
        cRecMenuItem *result = new cRecMenuItemEvent(reruns[number], rmsSearchShowInfo, rmsTimerConflictRecordRerun, false);
        return result;
    }
    return NULL;
}

int cRecMenuRerunResults::GetTotalNumMenuItems(void) {
    return numReruns;
}

// --- cRecMenuNoRerunsFound  ---------------------------------------------------------
cRecMenuNoRerunsFound::cRecMenuNoRerunsFound(cString searchString) {
    SetWidthPercent(50);
    cString message = tr("No reruns found for Event");
    cString text = cString::sprintf("%s\n\"%s\"", 
                                    *message, 
                                    *searchString);
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsTimerConflictIgnoreReruns, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuConfirmRerunUsed  ---------------------------------------------------------
cRecMenuConfirmRerunUsed::cRecMenuConfirmRerunUsed(const cEvent *original, const cEvent *replace) {
    SetWidthPercent(70);
    cString channelOrig = Channels.GetByChannelID(original->ChannelID())->Name();
    cString channelReplace = Channels.GetByChannelID(replace->ChannelID())->Name();
    cString message1 = tr("Timer for");
    cString message2 = tr("replaced by rerun");
    cString text = cString::sprintf("%s\n\"%s\", %s %s, %s\n%s\n\"%s\", %s %s, %s", 
                                    *message1, 
                                    original->Title(),
                                    *original->GetDateString(),
                                    *original->GetTimeString(),
                                    *channelOrig,
                                    *message2,
                                    replace->Title(),
                                    *replace->GetDateString(),
                                    *replace->GetTimeString(),
                                    *channelReplace);
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsTimerConflicts, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuEditTimer  ---------------------------------------------------------
cRecMenuEditTimer::cRecMenuEditTimer(const cTimer *timer, eRecMenuState nextState) {
    SetWidthPercent(60);
    if (timer) {
        cString title("");
        cString channelName("");
        if (timer->Event())
            title = timer->Event()->Title();
        if (timer->Channel())
            channelName = timer->Channel()->Name();
        cString infoText = cString::sprintf("%s:\n %s, %s", tr("Edit Timer"), *title, *channelName);
        cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
        infoItem->CalculateHeight(width - 2 * border);
        AddMenuItem(infoItem);
                    
        bool timerActive = false;
        if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
            RemoteTimers_GetMatch_v1_0 rtMatch;
            rtMatch.event = timer->Event();
            pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
            if (rtMatch.timer) {
                if (rtMatch.timerMatch == tmFull)
                    timerActive = true;
            }
        } else
            timerActive = timer->HasFlags(tfActive);

        time_t day = timer->Day();
        int start = timer->Start();
        int stop = timer->Stop();
        int prio = timer->Priority();
        int lifetime = timer->Lifetime();
        
        AddMenuItem(new cRecMenuItemBool(tr("Timer Active"), timerActive, false, true));
        AddMenuItem(new cRecMenuItemInt(tr("Priority"), prio, 0, MAXPRIORITY, false));
        AddMenuItem(new cRecMenuItemInt(tr("Lifetime"), lifetime, 0, MAXLIFETIME, false));
        AddMenuItem(new cRecMenuItemDay(tr("Day"), day, false));
        AddMenuItem(new cRecMenuItemTime(tr("Timer start time"), start, false));
        AddMenuItem(new cRecMenuItemTime(tr("Timer stop time"), stop, false));
        AddMenuItem(new cRecMenuItemButtonYesNo(tr("Save"), tr("Cancel"), nextState, rmsClose, false));
        CalculateHeight();
        CreatePixmap();
        Arrange();
    }
}

// --- cRecMenuSeriesTimer ---------------------------------------------------------
cRecMenuSeriesTimer::cRecMenuSeriesTimer(cChannel *initialChannel, const cEvent *event) {
    SetWidthPercent(70);
    if (initialChannel) {
        cString title = tr("Create Series Timer based on");
        cString infoText = cString::sprintf("%s:\n%s", *title, event->Title());
        cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
        infoItem->CalculateHeight(width - 2 * border);
        AddMenuItem(infoItem);
        AddMenuItem(new cRecMenuItemBool(tr("Timer Active"), true, false, true));
        AddMenuItem(new cRecMenuItemChannelChooser(tr("Channel"), initialChannel, false));
        
        time_t tstart = event->StartTime();
        tstart -= Setup.MarginStart * 60;
        time_t tstop = tstart + event->Duration();
        tstop  += Setup.MarginStop * 60;
        
        struct tm tm_r;
        struct tm *time = localtime_r(&tstart, &tm_r);
        int start = time->tm_hour * 100 + time->tm_min;
        time = localtime_r(&tstop, &tm_r);
        int stop = time->tm_hour * 100 + time->tm_min;
        if (stop >= 2400)
            stop -= 2400;
        
        AddMenuItem(new cRecMenuItemTime(tr("Series Timer start time"), start, false));
        AddMenuItem(new cRecMenuItemTime(tr("Series Timer stop time"), stop, false));
        AddMenuItem(new cRecMenuItemDayChooser(tr("Days to record"), 127, false));
        AddMenuItem(new cRecMenuItemDay(tr("Day to start"), tstart, false));
        AddMenuItem(new cRecMenuItemInt(tr("Priority"), MAXPRIORITY, 0, MAXPRIORITY, false));
        AddMenuItem(new cRecMenuItemInt(tr("Lifetime"), MAXLIFETIME, 0, MAXLIFETIME, false));

        AddMenuItem(new cRecMenuItemButtonYesNo(tr("Create Timer"), tr("Cancel"), rmsSeriesTimerCreate, rmsClose, false));

        CalculateHeight();
        CreatePixmap();
        Arrange();
    }
}

// --- cRecMenuConfirmSeriesTimer  ---------------------------------------------------------
cRecMenuConfirmSeriesTimer::cRecMenuConfirmSeriesTimer(cTimer *seriesTimer) {
    SetWidthPercent(50);
    cString message = tr("Series Timer created");
    cString infoText = message;
    if (seriesTimer) {
        cString days = cTimer::PrintDay(seriesTimer->Day(), seriesTimer->WeekDays(), true);
        infoText = cString::sprintf("%s\n%s, %s: %s, %s: %s", *message, *days, tr("Start"), *TimeString(seriesTimer->StartTime()), tr("Stop"), *TimeString(seriesTimer->StopTime()));
    }
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSearchTimer  ---------------------------------------------------------
cRecMenuSearchTimer::cRecMenuSearchTimer(const cEvent *event) {
    SetWidthPercent(70);
    cString message = tr("Configure Search Timer based on");
    cString infoText = cString::sprintf("%s:\n\"%s\"", *message, event->Title());
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    strn0cpy(initialText, event->Title(), sizeof(initialText));
    AddMenuItem(new cRecMenuItemText(tr("Search Expression:"), initialText, sizeof(initialText), false));
    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Continue"), tr("Cancel"), rmsSearchTimerOptions, rmsClose, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSearchTimerTemplates  ---------------------------------------------------------
cRecMenuSearchTimerTemplates::cRecMenuSearchTimerTemplates(cString searchString, std::vector<TVGuideEPGSearchTemplate> templates) {
    this->templates = templates;
    SetWidthPercent(70);
    cString message = tr("Configure Search Timer for Search String");
    cString infoText = cString::sprintf("%s:\n%s", *message, *searchString);
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    AddMenuItemScroll(new cRecMenuItemButton(tr("Manually configure Options"), rmsSearchTimerOptionsManually, true));
    
    numTemplates = templates.size();
    for (int i=0; i<numTemplates; i++) {
        cString buttonText = cString::sprintf("%s \"%s\"", tr("Use Template"), templates[i].name.c_str());
        AddMenuItemScroll(new cRecMenuItemButton(*buttonText, rmsSearchTimerUseTemplate, false));
        if (!CheckHeight())
            break;
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}
    
cRecMenuItem *cRecMenuSearchTimerTemplates::GetMenuItem(int number) {
    if (number == 0) {
        return new cRecMenuItemButton(tr("Manually configure Options"), rmsSearchTimerOptionsManually, false);
    } else if ((number > 0) && (number < numTemplates + 1)) {
        cString buttonText = cString::sprintf("%s \"%s\"", tr("Use Template"), templates[number-1].name.c_str());
        cRecMenuItem *templ = new cRecMenuItemButton(*buttonText, rmsSearchTimerUseTemplate, false);
        return templ;
    }
    return NULL;
}

int cRecMenuSearchTimerTemplates::GetTotalNumMenuItems(void) { 
    return numTemplates + 1;
}

// --- cRecMenuSearchTimerTemplatesCreate  ---------------------------------------------------------
cRecMenuSearchTimerTemplatesCreate::cRecMenuSearchTimerTemplatesCreate(cString searchString, cString tmplName) {
    SetWidthPercent(70);
    
    cString message1 = tr("Creating Search Timer");
    cString message2 = tr("Search Term");
    cString message3 = tr("Using Template");
    
    cString infoText = cString::sprintf("%s\n%s: \"%s\"\n%s \"%s\"", *message1, *message2, *searchString, *message3, *tmplName);
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);

    AddMenuItem(new cRecMenuItemButton(tr("Display Results for Search Timer"), rmsSearchTimerTestTemplate, true));
    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Create Search Timer"), tr("Use other Template"), rmsSearchTimerCreateTemplate, rmsSearchTimerOptionsReload, false));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSearchTimerOptions  ---------------------------------------------------------
cRecMenuSearchTimerOptions::cRecMenuSearchTimerOptions(cString searchString) {
    this->searchString = searchString;
    CreateMenuItems();
}

void cRecMenuSearchTimerOptions::CreateMenuItems(void) {
    bool showChannelSelectors = false;
    bool showTimeSelectors = false;
    if (menuItems.Count() > 0) {
        showChannelSelectors = GetBoolValue(4);
        if (GetIntValue(5)>-1) {
            showTimeSelectors = GetBoolValue(7);
        } else {
            showTimeSelectors = GetBoolValue(5);
        }
        osdManager.releasePixmap(pixmap);
        delete header;
        menuItems.Clear();
    }
    SetWidthPercent(70);
    cString message = tr("Configure Search Timer Options for Search String");
    cString infoText = cString::sprintf("%s:\n%s", *message, *searchString);
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    searchModes[0] = tr("whole term must appear");
    searchModes[1] = tr("all terms must exist");
    searchModes[2] = tr("one term must exist");
    searchModes[3] = tr("exact match");
    searchModes[4] = tr("regular expression");
    AddMenuItem(new cRecMenuItemSelect(tr("Search Mode"), searchModes, 0, 5, false));
    AddMenuItem(new cRecMenuItemBool(tr("Use Title"), true, false, false));
    AddMenuItem(new cRecMenuItemBool(tr("Use Subtitle"), true, false, false));
    AddMenuItem(new cRecMenuItemBool(tr("Use Description"), false, false, false));

    AddMenuItem(new cRecMenuItemBool(tr("Limit Channels"), showChannelSelectors, true, false));
    if (showChannelSelectors) {
        cChannel *startChannel = NULL;
        for (startChannel = Channels.First(); startChannel; startChannel = Channels.Next(startChannel)) {
            if (!startChannel->GroupSep())
                break;
        }
        AddMenuItem(new cRecMenuItemChannelChooser(tr("Start Channel"), startChannel, false));
        AddMenuItem(new cRecMenuItemChannelChooser(tr("Stop Channel"), startChannel, false));
    }
    
    AddMenuItem(new cRecMenuItemBool(tr("Use Time"), showTimeSelectors, true, false));
    if (showTimeSelectors) {
        AddMenuItem(new cRecMenuItemTime(tr("Start after"), 0, false));
        AddMenuItem(new cRecMenuItemTime(tr("Start before"), 2359, false));
    }
    
    AddMenuItem(new cRecMenuItemButton(tr("Display Results for Search Timer"), rmsSearchTimerTestManually, true));
    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Create Search Timer"), tr("Cancel"), rmsSearchTimerCreateManually, rmsClose, false));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSearchTimerResults ---------------------------------------------------------
cRecMenuSearchTimerResults::cRecMenuSearchTimerResults(cString searchString, const cEvent **searchResults, int numResults, std::string templateName) {
    this->searchResults = searchResults;
    SetWidthPercent(70);
    cString message1 = tr("search results for Search Timer");
    cString message2 = tr("search result for Search Timer");
    this->numResults = numResults;
    cString message3 = tr("Using Template");
    cString infoText;
    if (templateName.size() > 0) {
        infoText = cString::sprintf("%d %s:\n\"%s\"\n%s \"%s\"", numResults, (numResults>1)?(*message1):(*message2), *searchString, *message3, templateName.c_str());
    } else {
        infoText = cString::sprintf("%d %s:\n\"%s\"", numResults, (numResults>1)?(*message1):(*message2), *searchString);
    }
    cRecMenuItem *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    cRecMenuItem *buttons = new cRecMenuItemButton(tr("Close"), rmsClose, false);
    SetFooter(buttons);
    if (searchResults && (numResults > 0)) {
        for (int i=0; i<numResults; i++) {
            AddMenuItemScroll(new cRecMenuItemEvent(searchResults[i], rmsSearchShowInfo, rmsDisabled, (i==0)?true:false));
            if (!CheckHeight())
                break;
        }
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}
    
cRecMenuItem *cRecMenuSearchTimerResults::GetMenuItem(int number) { 
    if ((number >= 0) && (number < numResults)) {
        cRecMenuItem *result = new cRecMenuItemEvent(searchResults[number], rmsSearchShowInfo, rmsDisabled, false);
        return result;
    }
    return NULL;
}

int cRecMenuSearchTimerResults::GetTotalNumMenuItems(void) { 
    return numResults;
}

// --- cRecMenuSearchTimerNothingFound  ---------------------------------------------------------
cRecMenuSearchTimerNothingFound::cRecMenuSearchTimerNothingFound(cString searchString, std::string templateName) {
    SetWidthPercent(50);
    cString message = tr("Nothing found for Search String");
    cString message2 = tr("Using Template");
    cString text;
    if (templateName.size() > 0) {
       text = cString::sprintf("%s\n\"%s\"\n%s \"%s\"", 
                               *message, 
                               *searchString,
                               *message2,
                               templateName.c_str());
    } else {
       text = cString::sprintf("%s\n\"%s\"", 
                               *message, 
                               *searchString);
    }
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsSearchTimerNothingFoundConfirm, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSearchTimerCreateConfirm  ---------------------------------------------------------
cRecMenuSearchTimerCreateConfirm::cRecMenuSearchTimerCreateConfirm(bool success) {
    SetWidthPercent(50);
    
    cString message1a = tr("Search Timer sucessfully created.");
    cString message1b = tr ("Search Timer update initialised");
    cString message1 = cString::sprintf("%s\n%s", *message1a, *message1b);
    cString message2 = tr("Search Timer NOT sucessfully created");
    cString infoText = success?message1:message2;
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);

    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSwitchTimer  ---------------------------------------------------------
cRecMenuSwitchTimer::cRecMenuSwitchTimer(void) {
    SetWidthPercent(60);
    
    cString infoText = tr("Configure Options for Switchtimer");
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);

    AddMenuItem(new cRecMenuItemInt(tr("Minutes before switching"), 2, 0, 10, false));
    switchModes[0] = tr("switch");
    switchModes[1] = tr("announce only");
    switchModes[2] = tr("ask for switch");
    AddMenuItem(new cRecMenuItemSelect(tr("Switch Mode"), switchModes, 0, 3, false));
    
    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Create"), tr("Cancel"), rmsSwitchTimerCreate, rmsClose, true));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSwitchTimerConfirm  ---------------------------------------------------------
cRecMenuSwitchTimerConfirm::cRecMenuSwitchTimerConfirm(bool success) {
    SetWidthPercent(50);
    
    cString message1 = tr("Switch Timer sucessfully created");
    cString message2 = tr("Switch Timer NOT sucessfully created");
    cString infoText = success?message1:message2;
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);

    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSwitchTimerDelete  ---------------------------------------------------------
cRecMenuSwitchTimerDelete::cRecMenuSwitchTimerDelete(void) {
    SetWidthPercent(50);
    
    cString infoText = tr("Switch Timer deleted");
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);

    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true));
    
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSearch  ---------------------------------------------------------
cRecMenuSearch::cRecMenuSearch(const cEvent *event) {
    SetWidthPercent(60);
    cString infoText = tr("Search");
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    strn0cpy(initialText, event->Title(), sizeof(initialText));
    AddMenuItem(new cRecMenuItemText(tr("Search Expression:"), initialText, sizeof(initialText), false));
    AddMenuItem(new cRecMenuItemButton(tr("Show Search Options"), rmsSearchWithOptions, false));
    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Perform Search"), tr("Cancel"), rmsSearchPerform, rmsClose, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}
    
cRecMenuSearch::cRecMenuSearch(const cEvent *event, const char *searchString) {
    SetWidthPercent(60);
    cString infoText = tr("Search");
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    strn0cpy(initialText, searchString, sizeof(initialText));
    AddMenuItem(new cRecMenuItemText(tr("Search Expression:"), initialText, sizeof(initialText), false));
    searchModes[0] = tr("whole term must appear");
    searchModes[1] = tr("all terms must exist");
    searchModes[2] = tr("one term must exist");
    searchModes[3] = tr("exact match");
    searchModes[4] = tr("regular expression");
    AddMenuItem(new cRecMenuItemSelect(tr("Search Mode"), searchModes, 0, 5, false));
    AddMenuItem(new cRecMenuItemChannelChooser(tr("Channel to Search"), NULL, false));
    AddMenuItem(new cRecMenuItemBool(tr("Search in title"), true, false, false));
    AddMenuItem(new cRecMenuItemBool(tr("Search in Subtitle"), true, false, false));
    AddMenuItem(new cRecMenuItemBool(tr("Search in Description"), false, false, false));

    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Perform Search"), tr("Cancel"), rmsSearchPerform, rmsClose, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSearchResults  ---------------------------------------------------------
cRecMenuSearchResults::cRecMenuSearchResults(cString searchString, const cEvent **searchResults, int numResults) {
    this->searchResults = searchResults;
    SetWidthPercent(70);
    this->numResults = numResults;
    cString message1 = tr("search results for");
    cString message2 = tr("search result for");
    cString infoText = cString::sprintf("%d %s:\n\"%s\"", numResults, (numResults>1)?(*message1):(*message2), *searchString);
    cRecMenuItem *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    cRecMenuItem *buttons = new cRecMenuItemButtonYesNo(tr("Adapt Search"), tr("Close"), rmsSearch, rmsClose, false);
    SetFooter(buttons);
    if (searchResults && (numResults > 0)) {
        for (int i=0; i<numResults; i++) {
            AddMenuItemScroll(new cRecMenuItemEvent(searchResults[i], rmsSearchShowInfo, rmsSearchRecord, (i==0)?true:false));
            if (!CheckHeight())
                break;
        }
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cRecMenuItem *cRecMenuSearchResults::GetMenuItem(int number) { 
    if ((number >= 0) && (number < numResults)) {
        cRecMenuItem *result = new cRecMenuItemEvent(searchResults[number], rmsSearchShowInfo, rmsSearchRecord, false);
        return result;
    }
    return NULL;
}

int cRecMenuSearchResults::GetTotalNumMenuItems(void) { 
    return numResults;
}

// --- cRecMenuSearchConfirmTimer  ---------------------------------------------------------
cRecMenuSearchConfirmTimer::cRecMenuSearchConfirmTimer(const cEvent *event) {
    SetWidthPercent(50);
    cString channelName = Channels.GetByChannelID(event->ChannelID())->Name();
    cString message = tr("Timer created");
    cString text = cString::sprintf("%s\n%s\n%s %s - %s\n%s", 
                                    *message, 
                                    *channelName,
                                    *event->GetDateString(),
                                    *event->GetTimeString(),
                                    *event->GetEndTimeString(),
                                    event->Title()
                                );
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsSearchRecordConfirm, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuSearchNothingFound  ---------------------------------------------------------
cRecMenuSearchNothingFound::cRecMenuSearchNothingFound(cString searchString) {
    SetWidthPercent(50);
    cString message = tr("Nothing found for Search String");
    cString text = cString::sprintf("%s\n\"%s\"", 
                                    *message, 
                                    *searchString);
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsSearchNothingFoundConfirm, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuRecordingSearch  ---------------------------------------------------------
cRecMenuRecordingSearch::cRecMenuRecordingSearch(const cEvent *event) {
    SetWidthPercent(60);
    cString infoText = tr("Search in Recordings");
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    strn0cpy(initialText, event->Title(), sizeof(initialText));
    AddMenuItem(new cRecMenuItemText(tr("Search Expression:"), initialText, sizeof(initialText), false));
    AddMenuItem(new cRecMenuItemButtonYesNo(tr("Perform Search"), tr("Cancel"), rmsRecordingSearchResult, rmsClose, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

// --- cRecMenuRecordingSearchResults  ---------------------------------------------------------
cRecMenuRecordingSearchResults::cRecMenuRecordingSearchResults(cString searchString, cRecording **searchResults, int numResults) {
    this->searchResults = searchResults;
    SetWidthPercent(80);
    cString message1 = tr("Found");
    cString message2 = tr("recording");
    cString message3 = tr("recordings");
    cString message4 = tr("for");
    this->numResults = numResults;
    cString infoText = cString::sprintf("%s %d %s %s:\n\"%s\"", *message1, numResults, (numResults>1)?(*message3):(*message2), *message4, *searchString);
    cRecMenuItem *infoItem = new cRecMenuItemInfo(*infoText);
    infoItem->CalculateHeight(width - 2 * border);
    SetHeader(infoItem);
    
    cRecMenuItem *buttons = new cRecMenuItemButtonYesNo(tr("Adapt Search"), tr("Close"), rmsRecordingSearch, rmsClose, false);
    SetFooter(buttons);
    if (searchResults && (numResults > 0)) {
        for (int i=0; i<numResults; i++) {
            AddMenuItemScroll(new cRecMenuItemRecording(searchResults[i], (i==0)?true:false));
            if (!CheckHeight())
                break;
        }
    }
    CalculateHeight();
    CreatePixmap();
    Arrange();
}

cRecMenuItem *cRecMenuRecordingSearchResults::GetMenuItem(int number) { 
    if ((number >= 0) && (number < numResults)) {
        cRecMenuItem *result = new cRecMenuItemRecording(searchResults[number], false);
        return result;
    }
    return NULL;
}

int cRecMenuRecordingSearchResults::GetTotalNumMenuItems(void) { 
    return numResults;
}

// --- cRecMenuRecordingSearchNotFound  ---------------------------------------------------------
cRecMenuRecordingSearchNotFound::cRecMenuRecordingSearchNotFound(cString searchString) {
    SetWidthPercent(50);
    cString message = tr("No recordings found for");
    cString text = cString::sprintf("%s\n\"%s\"", 
                                    *message, 
                                    *searchString);
    cRecMenuItemInfo *infoItem = new cRecMenuItemInfo(*text);
    infoItem->CalculateHeight(width - 2 * border);
    AddMenuItem(infoItem);
    AddMenuItem(new cRecMenuItemButton(tr("OK"), rmsClose, true, true));
    CalculateHeight();
    CreatePixmap();
    Arrange();
}