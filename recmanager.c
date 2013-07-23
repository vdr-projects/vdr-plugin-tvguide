#include <string>
#include <vector>
#include "recmanager.h"

static int CompareRecording(const void *p1, const void *p2) {
   return (int)((*(cRecording **)p1)->Start() - (*(cRecording **)p2)->Start());
}

bool TVGuideTimerConflict::timerInvolved(int involvedID) {
    int numConflicts = timerIDs.size();
    for (int i=0; i<numConflicts; i++) {
        if (timerIDs[i] == involvedID)
            return true;
    }
    return false;
}

cRecManager::cRecManager(void) {
    epgSearchPlugin = NULL;
    epgSearchAvailable = false;
}

cRecManager::~cRecManager(void) {
}

void cRecManager::SetEPGSearchPlugin(void) {
	epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
	if (epgSearchPlugin) {
		epgSearchAvailable = true;
	}
}

bool cRecManager::RefreshRemoteTimers(void) {
    cString errorMsg;
    if (!pRemoteTimers->Service("RemoteTimers::RefreshTimers-v1.0", &errorMsg)) {
        esyslog("tvguide: %s", *errorMsg);
        return false;
    }
    return true;
}

bool cRecManager::CheckEventForTimer(const cEvent *event) {
    bool hasTimer = false;
    
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_GetMatch_v1_0 rtMatch;
        rtMatch.event = event;
        pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
        if (rtMatch.timerMatch == tmFull)
            hasTimer = true;
    } else
        hasTimer = event->HasTimer();

    return hasTimer;
}

cTimer *cRecManager::GetTimerForEvent(const cEvent *event) {
    cTimer *timer = NULL;
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_GetMatch_v1_0 rtMatch;
        rtMatch.event = event;
        pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
        timer = rtMatch.timer;
    } else
        timer = Timers.GetMatch(event);    
    return timer;
}

cTimer *cRecManager::createTimer(const cEvent *event, std::string path) {
    cTimer *timer = NULL;
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        timer = createRemoteTimer(event, path);
    } else {
        timer = createLocalTimer(event, path);
    }
    return timer;
}

cTimer *cRecManager::createLocalTimer(const cEvent *event, std::string path) {
    cTimer *timer = new cTimer(event);
    cTimer *t = Timers.GetTimer(timer);
    if (t) {
        t->OnOff();
        t->SetEventFromSchedule();
        delete timer;
        timer = t;
        isyslog("timer %s reactivated", *t->ToDescr());
    } else {
        Timers.Add(timer);
        isyslog("timer %s added (active)", *timer->ToDescr());
    }
    SetTimerPath(timer, path);
    Timers.SetModified();
    return timer;
}

cTimer *cRecManager::createRemoteTimer(const cEvent *event, std::string path) {
    cTimer *t = new cTimer(event);
    SetTimerPath(t, path);
    RemoteTimers_Timer_v1_0 rt;
    rt.timer = t;
    pRemoteTimers->Service("RemoteTimers::GetTimer-v1.0", &rt.timer);
    if (rt.timer) {
        rt.timer->OnOff();
        if (!pRemoteTimers->Service("RemoteTimers::ModTimer-v1.0", &rt))
            rt.timer = NULL;
    } else {
        rt.timer = t;
        if (!pRemoteTimers->Service("RemoteTimers::NewTimer-v1.0", &rt))
            isyslog("%s", *rt.errorMsg);
    }
    RefreshRemoteTimers();
    return rt.timer;
}

void cRecManager::SetTimerPath(cTimer *timer, std::string path) {
    if (path.size() > 0) {
        std::replace(path.begin(), path.end(), '/', '~');
        cString newFileName = cString::sprintf("%s~%s", path.c_str(), timer->File());
        timer->SetFile(*newFileName);
    }
}

void cRecManager::DeleteTimer(int timerID) {
    cTimer *t = Timers.Get(timerID);
    if (!t)
        return;
    DeleteTimer(t);
}

void cRecManager::DeleteTimer(const cEvent *event) {
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        DeleteRemoteTimer(event);
    } else {
        DeleteLocalTimer(event);
    }
}

void cRecManager::DeleteLocalTimer(const cEvent *event) {
    cTimer *t = Timers.GetMatch(event);
    if (!t)
        return;
    DeleteTimer(t);
}


void cRecManager::DeleteTimer(cTimer *timer) {
    if (timer->Recording()) {
        timer->Skip();
        cRecordControls::Process(time(NULL));
    }
    isyslog("timer %s deleted", *timer->ToDescr());
    Timers.Del(timer, true);
    Timers.SetModified();
}

void cRecManager::DeleteRemoteTimer(const cEvent *event) {
    RemoteTimers_GetMatch_v1_0 rtMatch;
    rtMatch.event = event;
    pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
    if (rtMatch.timer) {
        RemoteTimers_Timer_v1_0 rt;
        rt.timer = rtMatch.timer;
        isyslog("remotetimer %s deleted", *rt.timer->ToDescr());
        if (!pRemoteTimers->Service("RemoteTimers::DelTimer-v1.0", &rt))
            isyslog("remotetimer error");
        RefreshRemoteTimers();
    }
}

void cRecManager::SaveTimer(cTimer *timer, cRecMenu *menu) {
    if (!timer)
        return;
    
    bool active = menu->GetBoolValue(1);
    int prio = menu->GetIntValue(2);
    int lifetime = menu->GetIntValue(3);
    time_t day = menu->GetTimeValue(4);
    int start = menu->GetIntValue(5);
    int stop = menu->GetIntValue(6);

    timer->SetDay(day);
    timer->SetStart(start);
    timer->SetStop(stop);
    timer->SetPriority(prio);
    timer->SetLifetime(lifetime);
    
    if (timer->HasFlags(tfActive) && !active)
        timer->ClrFlags(tfActive);
    else if (!timer->HasFlags(tfActive) && active)
        timer->SetFlags(tfActive);
    
    timer->SetEventFromSchedule();
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_Timer_v1_0 rt;
        rt.timer = timer;
        if (!pRemoteTimers->Service("RemoteTimers::ModTimer-v1.0", &rt))
            rt.timer = NULL;
        RefreshRemoteTimers();
    } else {
        Timers.SetModified();
    }          
}

bool cRecManager::IsRecorded(const cEvent *event) {
    cTimer *timer = Timers.GetMatch(event);
    if (!timer)
        return false;
    return timer->Recording();
}

std::vector<TVGuideTimerConflict> cRecManager::CheckTimerConflict(void) {
    /* TIMERCONFLICT FORMAT:
    The result list looks like this for example when we have 2 timer conflicts at one time:
    1190232780:152|30|50#152#45:45|10|50#152#45
    '1190232780' is the time of the conflict in seconds since 1970-01-01. 
    It's followed by list of timers that have a conflict at this time:
    '152|30|50#1    int editTimer(cTimer *timer, bool active, int prio, int start, int stop);
    52#45' is the description of the first conflicting timer. Here:
    '152' is VDR's timer id of this timer as returned from VDR's LSTT command
    '30' is the percentage of recording that would be done (0...100)
    '50#152#45' is the list of concurrent timers at this conflict
    '45|10|50#152#45' describes the next conflict
    */
    std::vector<TVGuideTimerConflict> results;
    if (!epgSearchAvailable)
        return results;
    Epgsearch_services_v1_1 *epgSearch = new Epgsearch_services_v1_1;
    if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
        std::list<std::string> conflicts = epgSearch->handler->TimerConflictList();
        int numConflicts = conflicts.size();
        if (numConflicts > 0) {
            for (std::list<std::string>::iterator it=conflicts.begin(); it != conflicts.end(); ++it) {
                TVGuideTimerConflict sConflict;
                splitstring s(it->c_str());
                std::vector<std::string> flds = s.split(':');
                if (flds.size() < 2)
                    continue;
                sConflict.time = atoi(flds[0].c_str());
                splitstring s2(flds[1].c_str());
                std::vector<std::string> flds2 = s2.split('|');
                if (flds2.size() < 3)
                    continue;
                sConflict.timerID = atoi(flds2[0].c_str());
                sConflict.percentPossible = atoi(flds2[1].c_str());
                splitstring s3(flds2[2].c_str());
                std::vector<std::string> flds3 = s3.split('#');
                std::vector<int> timerIDs;
                for (int k = 0; k < flds3.size(); k++) {
                    timerIDs.push_back(atoi(flds3[k].c_str()) - 1);
                }
                sConflict.timerIDs = timerIDs;
                results.push_back(sConflict);
            }
        }
    }
    delete epgSearch;
    
    int numConflicts = results.size();
    time_t startTime = 0;
    time_t endTime = 0;
    for (int i=0; i < numConflicts; i++) {
        cTimeInterval *unionSet = NULL;
        int numTimers = results[i].timerIDs.size();
        for (int j=0; j < numTimers; j++) {
            const cTimer *timer = Timers.Get(results[i].timerIDs[j]);
            if (timer) {
                if (!unionSet) {
                    unionSet = new cTimeInterval(timer->StartTime(), timer->StopTime());
                } else {
                    cTimeInterval *timerInterval = new cTimeInterval(timer->StartTime(), timer->StopTime());
                    cTimeInterval *newUnion = unionSet->Union(timerInterval);
                    delete unionSet;
                    delete timerInterval;
                    unionSet = newUnion;
                }
            }
        }
        results[i].timeStart = unionSet->Start();
        results[i].timeStop = unionSet->Stop();
        delete unionSet;
        
        cTimeInterval *intersect = NULL;
        for (int j=0; j < numTimers; j++) {
            const cTimer *timer = Timers.Get(results[i].timerIDs[j]);
            if (timer) {
                if (!intersect) {
                    intersect = new cTimeInterval(timer->StartTime(), timer->StopTime());
                } else {
                    cTimeInterval *timerInterval = new cTimeInterval(timer->StartTime(), timer->StopTime());
                    cTimeInterval *newIntersect = intersect->Intersect(timerInterval);
                    if (newIntersect) {
                        delete intersect;
                        intersect = newIntersect;
                    }
                    delete timerInterval;
                }
            }
        }
        results[i].overlapStart = intersect->Start();
        results[i].overlapStop = intersect->Stop();
        delete intersect;
    }
    
    return results;
}

cTimer *cRecManager::CreateSeriesTimer(cRecMenu *menu, std::string path) {
    bool active = menu->GetBoolValue(1);
    int channelNumber = menu->GetIntValue(2);
    int start = menu->GetIntValue(3);
    int stop = menu->GetIntValue(4);
    int weekdays = menu->GetIntValue(5);
    time_t tday = menu->GetTimeValue(6);
    int prio = menu->GetIntValue(7);
    int lifetime = menu->GetIntValue(8);

    cChannel *channel = Channels.GetByNumber(channelNumber);
    cTimer *seriesTimer = new cTimer(false, false, channel);
    
    cString fileName = "TITLE EPISODE";
    if (path.size() > 0) {
        std::replace(path.begin(), path.end(), '/', '~');
        fileName = cString::sprintf("%s~%s", path.c_str(), *fileName);
    }
    
    seriesTimer->SetDay(tday);
    seriesTimer->SetStart(start);
    seriesTimer->SetStop(stop);
    seriesTimer->SetPriority(prio);
    seriesTimer->SetLifetime(lifetime);
    seriesTimer->SetWeekDays(weekdays);
    seriesTimer->SetFile(*fileName);
    if (active)
        seriesTimer->SetFlags(tfActive);
    else 
        seriesTimer->SetFlags(tfNone);
    seriesTimer->SetEventFromSchedule();

    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_Timer_v1_0 rt;
        rt.timer = seriesTimer;
        if (!pRemoteTimers->Service("RemoteTimers::NewTimer-v1.0", &rt))
            isyslog("%s", *rt.errorMsg);
        RefreshRemoteTimers();
        seriesTimer = NULL;
    } else {
        Timers.Add(seriesTimer);
        Timers.SetModified();
    }
    return seriesTimer;
}

std::vector<TVGuideEPGSearchTemplate> cRecManager::ReadEPGSearchTemplates(void) {
    cString ConfigDir = cPlugin::ConfigDirectory("epgsearch");
    cString epgsearchConf = "epgsearchtemplates.conf";
    cString fileName = AddDirectory(*ConfigDir,  *epgsearchConf);
    std::vector<TVGuideEPGSearchTemplate> epgTemplates;
    if (access(fileName, F_OK) == 0) {
        FILE *f = fopen(fileName, "r");
        if (f) {
            char *s;
            cReadLine ReadLine;
            while ((s = ReadLine.Read(f)) != NULL) {
                char *p = strchr(s, '#');
                if (p)
                    *p = 0;
                stripspace(s);
                try {
                    if (!isempty(s)) {
                        std::string templ = s;
                        int posID = templ.find_first_of(":");
                        int posName = templ.find_first_of(":", posID+1);
                        std::string name = templ.substr(posID+1, posName - posID - 1);
                        std::string templValue = templ.substr(posName);
                        TVGuideEPGSearchTemplate tmp;
                        tmp.name = name;
                        tmp.templValue = templValue;
                        epgTemplates.push_back(tmp);
                    }
                } catch (...){}
            }
        }
    }
    return epgTemplates;
}

std::string cRecManager::BuildEPGSearchString(cString searchString, std::string templValue) {
     std::string strSearchString = *searchString;
     std::replace(strSearchString.begin(), strSearchString.end(), ':', '|');
     std::stringstream searchTimerString;
     searchTimerString << "0:";
     searchTimerString << strSearchString;
     searchTimerString << templValue;
     return searchTimerString.str();
}

std::string cRecManager::BuildEPGSearchString(cString searchString, cRecMenu *menu) {
    std::string strSearchString = *searchString;
    std::replace(strSearchString.begin(), strSearchString.end(), ':', '|');
    int searchMode = menu->GetIntValue(0);
    bool useTitle = menu->GetBoolValue(1);
    bool useSubTitle = menu->GetBoolValue(2);
    bool useDescription = menu->GetBoolValue(3);
    bool limitChannels = menu->GetBoolValue(4);
    int startChannel = -1;
    int stopChannel = -1;
    if (limitChannels) {
        startChannel = menu->GetIntValue(5);
        stopChannel = menu->GetIntValue(6);
    }
    int after = 0;
    int before = 0;
    bool limitTime = (limitChannels)?menu->GetBoolValue(7):menu->GetBoolValue(5);
    if (limitTime) {
        after = (limitChannels)?menu->GetIntValue(8):menu->GetIntValue(6);
        before = (limitChannels)?menu->GetIntValue(9):menu->GetIntValue(7);
    }

    std::stringstream searchTimerString;
    //1 - unique search timer id
    searchTimerString << "0:";
    //2 - the search term
    searchTimerString << strSearchString;
    //3 - use time? 0/1
    //4 - start time in HHMM
    //5 - stop time in HHMM
    if (limitTime) {
        searchTimerString << ":1:" << after << ":" << before << ":";
    } else {
        searchTimerString << ":0:::";
    }
    //6 - use channel? 0 = no,  1 = Interval, 2 = Channel group, 3 = FTA only
    //7 - if 'use channel' = 1 then channel id[|channel id] in VDR format,
    //    one entry or min/max entry separated with |, if 'use channel' = 2
    //    then the channel group name
    if (limitChannels) {
        searchTimerString << "1:";
        cChannel *startChan = Channels.GetByNumber(startChannel);
        cChannel *stopChan = Channels.GetByNumber(stopChannel);
        searchTimerString << *(startChan->GetChannelID().ToString());
        searchTimerString << "|";
        searchTimerString << *(stopChan->GetChannelID().ToString()) << ":";
    } else {
        searchTimerString << "0::";
    }
    //8 - match case? 0/1
    searchTimerString << ":0";    
    /*9 - search mode:
        0 - the whole term must appear as substring
        1 - all single terms (delimiters are blank,',', ';', '|' or '~')
            must exist as substrings.
        2 - at least one term (delimiters are blank, ',', ';', '|' or '~')
            must exist as substring.
        3 - matches exactly
        4 - regular expression */   
    searchTimerString << searchMode << ":";
    //10 - use title? 0/1
    if (useTitle)
        searchTimerString << "1:";
    else
        searchTimerString << "0:";
    //11 - use subtitle? 0/1
    if (useSubTitle)
        searchTimerString << "1:";
    else
        searchTimerString << "0:";
    // 12 - use description? 0/1
    if (useDescription)
        searchTimerString << "1:";
    else
        searchTimerString << "0:";
    //13 - use duration? 0/1
    //14 - min duration in hhmm
    //15 - max duration in hhmm
    searchTimerString << "0:::";
    //16 - use as search timer? 0/1
    searchTimerString << "1:";
    //17 - use day of week? 0/1
    //18 - day of week (0 = Sunday, 1 = Monday...;
    //     -1 Sunday, -2 Monday, -4 Tuesday, ...; -7 Sun, Mon, Tue)
    searchTimerString << "0::";
    //19 - use series recording? 0/1
    searchTimerString << "1:";
    //20 - directory for recording
    searchTimerString << ":";
    //21 - priority of recording
    //22 - lifetime of recording
    searchTimerString << "99:99:";
    //23 - time margin for start in minutes
    //24 - time margin for stop in minutes
    searchTimerString << "5:5:";
    //25 - use VPS? 0/1
    searchTimerString << "0:";
    /*26 - action:
         0 = create a timer
         1 = announce only via OSD (no timer)
         2 = switch only (no timer)
         3 = announce via OSD and switch (no timer)
         4 = announce via mail*/
    searchTimerString << "0:";
    /*27 - use extended EPG info? 0/1
    28 - extended EPG info values. This entry has the following format
         (delimiter is '|' for each category, '#' separates id and value):
         1 - the id of the extended EPG info category as specified in
             epgsearchcats.conf
         2 - the value of the extended EPG info category
             (a ':' will be translated to "!^colon^!", e.g. in "16:9") */
    searchTimerString << "0::";
    /*29 - avoid repeats? 0/1
    30 - allowed repeats
    31 - compare title when testing for a repeat? 0/1
    32 - compare subtitle when testing for a repeat? 0/1/2
         0 - no
         1 - yes
         2 - yes, if present
    33 - compare description when testing for a repeat? 0/1
    34 - compare extended EPG info when testing for a repeat?
         This entry is a bit field of the category IDs.
    35 - accepts repeats only within x days */
    searchTimerString << "1:1:1:2:1:::";
    /*36 - delete a recording automatically after x days
    37 - but keep this number of recordings anyway
    38 - minutes before switch (if action = 2)
    39 - pause if x recordings already exist
    40 - blacklist usage mode (0 none, 1 selection, 2 all)
    41 - selected blacklist IDs separated with '|'
    42 - fuzzy tolerance value for fuzzy searching
    43 - use this search in favorites menu (0 no, 1 yes)
    44 - id of a menu search template
    45 - auto deletion mode (0 don't delete search timer, 1 delete after given
         count of recordings, 2 delete after given days after first recording)
    46 - count of recordings after which to delete the search timer
    47 - count of days after the first recording after which to delete the search
         timer
    48 - first day where the search timer is active (see parameter 16)
    49 - last day where the search timer is active (see parameter 16)
    50 - ignore missing EPG categories? 0/1
    51 - unmute sound if off when used as switch timer
    52 - percentage of match when comparing the summary of two events (with 'avoid repeats')
    53 - HEX representation of the content descriptors, each descriptor ID is represented with 2 chars
    54 - compare date when testing for a repeat? (0=no, 1=same day, 2=same week, 3=same month) */
    searchTimerString << "0::::0:::0::0:::::::::0";

    //esyslog("tvguide: epgsearch String: %s", searchTimerString.str().c_str());
    
    return searchTimerString.str();
}

const cEvent **cRecManager::PerformSearchTimerSearch(std::string epgSearchString, int &numResults) {
    if (!epgSearchAvailable)
        return NULL;
    const cEvent **searchResults = NULL;
    Epgsearch_services_v1_1 *epgSearch = new Epgsearch_services_v1_1;
    if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
        std::list<std::string> results = epgSearch->handler->QuerySearch(epgSearchString);
        numResults = results.size();
        if (numResults > 0) {
            searchResults = new const cEvent *[numResults];
            cSchedulesLock schedulesLock;
            const cSchedules *schedules;
            schedules = cSchedules::Schedules(schedulesLock);
            const cEvent *event = NULL;
            int index=0;
            for (std::list<std::string>::iterator it=results.begin(); it != results.end(); ++it) {
                try {
                    splitstring s(it->c_str());
                    std::vector<std::string> flds = s.split(':', 1);
                    int eventID = atoi(flds[1].c_str());
                    std::string channelID = flds[7];
                    tChannelID chanID = tChannelID::FromString(channelID.c_str());
                    cChannel *channel = Channels.GetByChannelID(chanID);
                    if (channel) {
                        const cSchedule *Schedule = NULL;
                        Schedule = schedules->GetSchedule(channel);
                        event = Schedule->GetEvent(eventID);
                        if (event) {
                            searchResults[index] = event;                        
                        } else
                            return NULL;
                    } else
                        return NULL;
                    index++;
                } catch (...){}
            }
        }
    }
    return searchResults;
}

const cEvent **cRecManager::PerformSearch(cRecMenu *menu, bool withOptions, int &numResults) {
    if (epgSearchAvailable) {
        cString searchString = menu->GetStringValue(1);
        Epgsearch_searchresults_v1_0 data;
        data.query = (char *)*searchString;
        int mode = 0;
        int channelNr = 0;
        bool useTitle = true;
        bool useSubTitle = true;
        bool useDescription = false;
        if (withOptions) {
            mode = menu->GetIntValue(2);
            channelNr = menu->GetIntValue(3);
            useTitle = menu->GetBoolValue(4);
            useSubTitle = menu->GetBoolValue(5);
            useDescription = menu->GetBoolValue(6);
        }
        data.mode = mode;
        data.channelNr = channelNr;
        data.useTitle = useTitle;
        data.useSubTitle = useSubTitle;
        data.useDescription = useDescription;

        if (epgSearchPlugin->Service("Epgsearch-searchresults-v1.0", &data)) {
            cList<Epgsearch_searchresults_v1_0::cServiceSearchResult> *list = data.pResultList;
            if (!list)
                return NULL;
            int numElements = list->Count();
            const cEvent **searchResults = NULL;
            if (numElements > 0) {
                searchResults = new const cEvent *[numElements];
                numResults = numElements;
                int index = 0;
                for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = list->First(); r ; r = list->Next(r)) {
                    searchResults[index] = r->event;
                    index++;
                }
            }
            delete list;
            return searchResults;
        }
    }
    return NULL;
}

int cRecManager::CreateSearchTimer(std::string epgSearchString) {
    int timerID = -1;
    if (!epgSearchAvailable)
        return timerID;
    Epgsearch_services_v1_1 *epgSearch = new Epgsearch_services_v1_1;
    if (epgSearchPlugin->Service("Epgsearch-services-v1.1", epgSearch)) {
        timerID = epgSearch->handler->AddSearchTimer(epgSearchString);
    }
    return timerID;
}

void cRecManager::UpdateSearchTimers(void) {
    if (epgSearchAvailable) {
        Epgsearch_updatesearchtimers_v1_0 data;
        data.showMessage = false;
        epgSearchPlugin->Service("Epgsearch-updatesearchtimers-v1.0", &data);
    }
}

// announceOnly: 0 = switch, 1 = announce only, 2 = ask for switch
bool cRecManager::CreateSwitchTimer(const cEvent *event, cRecMenu *menu) {
    int switchMinsBefore = menu->GetIntValue(1);
    int announceOnly = menu->GetIntValue(2);
    if (epgSearchAvailable) {
        Epgsearch_switchtimer_v1_0 data;
        data.event = event;
        data.mode = 1;
        data.switchMinsBefore = switchMinsBefore;
        data.announceOnly = announceOnly;
        data.success = false;
        epgSearchPlugin->Service("Epgsearch-switchtimer-v1.0", &data);
        cSwitchTimer *t = new cSwitchTimer(event);
        SwitchTimers.Add(t);
        return data.success;
    }
    return false;
}

void cRecManager::DeleteSwitchTimer(const cEvent *event) {
    SwitchTimers.DeleteSwitchTimer(event);
    if (epgSearchAvailable) {
        Epgsearch_switchtimer_v1_0 data;
        data.event = event;
        data.mode = 2;
        data.switchMinsBefore = 0;
        data.announceOnly = 0;
        data.success = false;
        epgSearchPlugin->Service("Epgsearch-switchtimer-v1.0", &data);
    }
}

cRecording **cRecManager::SearchForRecordings(cString searchString, int &numResults) {
    
    cRecording **matchingRecordings = NULL;
    int num = 0;
    numResults = 0;
    
    for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording)) {
        std::string s1 = recording->Name();
        std::string s2 = *searchString;
        if (s1.empty() || s2.empty()) continue;
        
        // tolerance for fuzzy searching: 90% of the shorter text length, but at least 1
        int tolerance = std::max(1, (int)std::min(s1.size(), s2.size()) / 10); 

        bool match = FindIgnoreCase(s1, s2) >= 0 || FindIgnoreCase(s2, s1) >= 0;
        
        if (!match) {
            AFUZZY af = { NULL, NULL, NULL, NULL, NULL, NULL, { 0 }, { 0 }, 0, 0, 0, 0, 0, 0 };
            if (s1.size() > 32) s1 = s1.substr(0, 32);
            afuzzy_init(s1.c_str(), tolerance, 0, &af);
            /* Checking substring */
            int res = afuzzy_checkSUB(s2.c_str(), &af);
            afuzzy_free(&af);
            match = (res > 0);
        }
        
        if (!match) {
            AFUZZY af = { NULL, NULL, NULL, NULL, NULL, NULL, { 0 }, { 0 }, 0, 0, 0, 0, 0, 0 };
            if (s2.size() > 32) s2 = s2.substr(0, 32);
            afuzzy_init(s2.c_str(), tolerance, 0, &af);
            /* Checking substring */
            int res = afuzzy_checkSUB(s1.c_str(), &af);
            afuzzy_free(&af);
            match = (res > 0);
        }
        
        if (match) {
            matchingRecordings = (cRecording **)realloc(matchingRecordings, (num + 1) * sizeof(cRecording *));
            matchingRecordings[num++] = recording;
        }
    }
    if (num > 0) {
        qsort(matchingRecordings, num, sizeof(cRecording *), CompareRecording);
        numResults = num;
        return matchingRecordings;
    }
    return NULL;
}