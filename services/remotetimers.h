// RemoteTimers services
struct RemoteTimers_Event_v1_0 {
    //in
    const cEvent *event;
    //out
    cTimer *timer;
    cString errorMsg;
};

struct RemoteTimers_GetMatch_v1_0 {
    //in
    const cEvent *event;
    //out
    cTimer *timer;
    int timerMatch;
    int timerType;
    bool isRemote;
};

struct RemoteTimers_Timer_v1_0 {
    //in+out
    cTimer *timer;
    //out
    cString errorMsg;
};