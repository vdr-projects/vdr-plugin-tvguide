#ifndef __TVGUIDE_SEARCHTIMER_H
#define __TVGUIDE_SEARCHTIMER_H


class cTVGuideSearchTimer {
private:
    std::string strTimer;
    int         ID;
    std::string searchString;
    int         useTime;
    int         startTime;
    int      	stopTime;
    int         useChannel;
#if VDRVERSNUM >= 20301
    const cChannel    *channelMin;
    const cChannel    *channelMax;
#else
    cChannel 	*channelMin;
    cChannel 	*channelMax;
#endif
    std::string channelGroup;
    int      	useCase;
    int      	mode;
    int         useTitle;
    int         useSubtitle;
    int         useDescription;
    int         useDuration;
    int         minDuration;
    int         maxDuration;
    int         useAsSearchTimer;
    int         useDayOfWeek;
    int         dayOfWeek;
    int         useEpisode;
    std::string directory;
    int         priority;
    int         lifetime;
    int         marginStart;
    int         marginStop;
    int         useVPS;
    int         action;
    int         useExtEPGInfo;
    std::string extEPGInfoValues;
    int         avoidRepeats;
    int         allowedRepeats;
    int         compareTitle;
    int         compareSubtitle;
    int         compareSummary;
    unsigned long catvaluesAvoidRepeat;
    int         repeatsWithinDays;
    int         delAfterDays;
    int         recordingsKeep;
    int         switchMinsBefore;
    int         pauseOnNrRecordings;
    int         blacklistMode;
    std::string blacklists;
    int         fuzzyTolerance;
    int         useInFavorites;
    int         menuTemplate;
    int         delMode;
    int         delAfterCountRecs;
    int         delAfterDaysOfFirstRec;
    int         useAsSearchTimerFrom;
    int         useAsSearchTimerTil;
    int         ignoreMissingEPGCats;
    int         unmuteSoundOnSwitch;
    int         compareSummaryMatchInPercent;
    std::string contentsFilter;
    int         compareDate;
public:
    cTVGuideSearchTimer(void);
    virtual ~cTVGuideSearchTimer(void);
    bool operator < (const cTVGuideSearchTimer& other) const;
    void SetEPGSearchString(std::string strTimer) { this->strTimer = strTimer; };
    void SetTemplate(std::string tmpl);
    bool Parse(bool readTemplate = false);
    std::string BuildSearchString(void);
    int GetID(void) { return ID; };
    //GETTER
    std::string GetSearchString(void) const { return searchString; };
    bool IsActive(void);
    bool UseTitle(void) { return useTitle; };
    bool UseSubtitle(void) { return useSubtitle; };
    bool UseDescription(void) { return useDescription; };
    int SearchMode(void) { return mode; };
    int UseChannel(void) { return useChannel; };
    int StartChannel(void) { return (channelMin)?channelMin->Number():0; };
    int StopChannel(void) { return (channelMax)?channelMax->Number():0; };
    std::string ChannelGroup(void) { return channelGroup; };
    bool UseTime(void) { return useTime; };
    int StartTime(void) { return startTime; };
    int StopTime(void) { return stopTime; };
    bool UseDayOfWeek(void) { return useDayOfWeek; };
    int DayOfWeek(void);
    int UseEpisode(void) { return useEpisode; };
    std::string Directory(void) { return directory; };
    int Priority(void) { return priority; };
    int Lifetime(void) { return lifetime; };
    int MarginStart(void) { return marginStart; };
    int MarginStop(void) { return marginStop; };
    bool UseVPS(void) { return useVPS; };
    bool AvoidRepeats(void) { return avoidRepeats; };
    int AllowedRepeats(void) { return allowedRepeats; };
    bool CompareTitle(void) { return compareTitle; };
    bool CompareSubtitle(void) { return compareSubtitle; };
    bool CompareSummary(void) { return compareSummary; };
    bool UseInFavorites(void) { return useInFavorites; };
    //SETTER
    void SetSearchString(std::string searchString) { this->searchString = searchString; };
    void SetActive(bool active) { useAsSearchTimer = active; };
    void SetSearchMode(int searchMode) { mode = searchMode; };
    void SetUseTitle(bool useTitle) { this->useTitle = useTitle; };
    void SetUseSubtitle(bool useSubtitle) { this->useSubtitle = useSubtitle; };
    void SetUseDesription(bool useDescription) { this->useDescription = useDescription; };
    void SetUseChannel(int useChannel) { this->useChannel = useChannel; };
    void SetStartChannel(int startChannel);
    void SetStopChannel(int stopChannel);
    void SetChannelGroup(std::string channelGroup) { this->channelGroup = channelGroup; };
    void SetUseTime(bool useTime) { this->useTime = useTime; };
    void SetStartTime(int startTime) { this->startTime = startTime; };
    void SetStopTime(int stopTime) { this->stopTime = stopTime; };
    void SetUseDayOfWeek(bool useDayOfWeek) { this->useDayOfWeek = useDayOfWeek; };
    void SetDayOfWeek(int VDRDayOfWeek);
    void SetUseEpisode(int useEpisode) { this->useEpisode = useEpisode; };
    void SetDirectory(std::string directory) { this-> directory = directory; };
    void SetPriority(int priority) { this->priority = priority; };
    void SetLifetime(int lifetime) { this->lifetime = lifetime; };
    void SetMarginStart(int marginStart) { this->marginStart = marginStart; };
    void SetMarginStop(int marginStop) { this->marginStop = marginStop; };
    void SetUseVPS(bool useVPS) { this->useVPS = useVPS; };
    void SetAvoidRepeats(bool avoidRepeats) { this->avoidRepeats = avoidRepeats; };
    void SetAllowedRepeats(int allowedRepeats) { this->allowedRepeats = allowedRepeats; };
    void SetCompareTitle(bool compareTitle) { this->compareTitle = compareTitle; };
    void SetCompareSubtitle(bool compareSubtitle) { this->compareSubtitle = compareSubtitle; };
    void SetCompareSummary(bool compareSummary) { this->compareSummary = compareSummary; };
    void SetUseInFavorites(bool useInFavorites) { this->useInFavorites = useInFavorites; };
    //COMMON
    int GetNumTimers(void);
    int GetNumRecordings(void);
    void GetSearchModes(std::vector<std::string> *searchModes);
    void GetUseChannelModes(std::vector<std::string> *useChannelModes);
    void Dump(void);
};

#endif //__TVGUIDE_SEARCHTIMER_H
