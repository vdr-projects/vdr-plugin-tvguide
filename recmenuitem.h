#ifndef __TVGUIDE_RECMENUITEM_H
#define __TVGUIDE_RECMENUITEM_H

#define AUTO_ADVANCE_TIMEOUT 1500

enum eRecMenuState {
    rmsConsumed,
    rmsNotConsumed,
    rmsRefresh,
    rmsContinue,
    rmsClose,
    rmsInstantRecord,
    rmsIgnoreTimerConflict,
    rmsDeleteTimerConflictMenu,
    rmsEditTimerConflictMenu,
    rmsSaveTimerConflictMenu,
    rmsTimerConflictShowInfo,
    rmsDeleteTimer,
    rmsDeleteTimerConfirmation,
    rmsEditTimer,
    rmsSaveTimer,
    rmsSearch,
    rmsSearchWithOptions,
    rmsSearchPerform,
    rmsSearchShowInfo,
    rmsSearchRecord,
    rmsSearchRecordConfirm,
    rmsSearchNothingFoundConfirm,
    rmsSeriesTimer,
    rmsSeriesTimerCreate,
    rmsSearchTimer,
    rmsSearchTimerOptions,
    rmsSearchTimerOptionsReload,
    rmsSearchTimerUseTemplate,
    rmsSearchTimerOptionsManually,
    rmsSearchTimerTestManually,
    rmsSearchTimerTestTemplate,
    rmsSearchTimerNothingFoundConfirm,
    rmsSearchTimerCreateManually,
    rmsSearchTimerCreateTemplate,
    rmsSwitchTimer,
    rmsSwitchTimerCreate,
    rmsSwitchTimerDelete,
    rmsRecordingSearch,
    rmsRecordingSearchResult,
    rmsTimerConflict,
    rmsTimerConflicts,
    rmsDisabled,
};

enum eDependend {
    eGreater,
    eLower,
};
// --- cRecMenuItem  -------------------------------------------------------------
class cRecMenuItem : public cListObject, public cStyledPixmap {
protected:
    int x, y;
    int width, height;
    bool selectable;
    bool active;
    bool drawn;
    eRecMenuState action;
    tColor colorText;
    tColor colorTextBack;
    const cFont *font;
    const cFont *fontSmall;
public:
    cRecMenuItem(void);
    virtual ~cRecMenuItem(void);
    void SetGeometry(int x, int y, int width);
    virtual void SetPixmaps(void);
    virtual int GetHeight(void) { return height; };
    virtual int GetWidth(void) { return 0; };
    virtual void CalculateHeight(int textWidth) {};
    void setActive(void) { this->active = true; }
    void setInactive(void) { this->active = false; }
    bool isSelectable(void) { return selectable; }
    bool isActive(void) { return active; }
    virtual void setBackground(void);
    virtual void Draw(void) {};
    virtual void Hide(void) { pixmap->SetLayer(-1);};
    virtual void Show(void) { pixmap->SetLayer(4);};
    virtual int GetIntValue(void) { return -1; };
    virtual time_t GetTimeValue(void) { return 0; };
    virtual bool GetBoolValue(void) { return false; };
    virtual cString GetStringValue(void) { return cString(""); };
    virtual const cEvent *GetEventValue(void) { return NULL; };
    virtual eRecMenuState ProcessKey(eKeys Key) { return rmsNotConsumed; };

};

// --- cRecMenuItemButton  -------------------------------------------------------
class cRecMenuItemButton : public cRecMenuItem {
private:
    cString text;
    bool halfWidth;
public:
    cRecMenuItemButton(const char *text, eRecMenuState action, bool active, bool halfWidth = false);
    virtual ~cRecMenuItemButton(void);
    int GetWidth(void);
    void SetPixmaps(void);
    void Draw(void);
    eRecMenuState ProcessKey(eKeys Key);
};

// --- cRecMenuItemButtonYesNo  -------------------------------------------------------
class cRecMenuItemButtonYesNo : public cRecMenuItem {
private:
    cString textYes;
    cString textNo;
    eRecMenuState actionNo;
    bool yesActive;
    cStyledPixmap *pixmapNo;
    tColor colorTextNo;
public:
    cRecMenuItemButtonYesNo(cString textYes,
                            cString textNo,
                            eRecMenuState actionYes, 
                            eRecMenuState actionNo,
                            bool active);
    virtual ~cRecMenuItemButtonYesNo(void);
    void SetPixmaps(void);
    void setBackground(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
};

// --- cRecMenuItemInfo  -------------------------------------------------------
class cRecMenuItemInfo : public cRecMenuItem {
private:
    cString text;
    cTextWrapper wrapper;
    int border;
public:
    cRecMenuItemInfo(const char *text);
    virtual ~cRecMenuItemInfo(void);
    void setBackground(void);
    void CalculateHeight(int textWidth);
    void Draw(void);
};

// --- cRecMenuItemInt  -------------------------------------------------------
class cRecMenuItemInt : public cRecMenuItem {
private:
    cString text;
    int currentVal;
    int minVal;
    int maxVal;
    cPixmap *pixmapVal;
    bool fresh;
    void DrawValue(void);
public:
    cRecMenuItemInt(cString text,
                    int initialVal,
                    int minVal,
                    int maxVal,
                    bool active);
    virtual ~cRecMenuItemInt(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    void setBackground(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    int GetIntValue(void) { return currentVal; };
};

// --- cRecMenuItemBool  -------------------------------------------------------
class cRecMenuItemBool : public cRecMenuItem {
private:
    cString text;
    bool yes;
    cPixmap *pixmapVal;
    bool refresh;
    void DrawValue(void);
public:
    cRecMenuItemBool(cString text,
                     bool initialVal,
                     bool refresh,
                     bool active);
    virtual ~cRecMenuItemBool(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    bool GetBoolValue(void) { return yes; };
};

// --- cRecMenuItemSelect  -------------------------------------------------------
class cRecMenuItemSelect : public cRecMenuItem {
private:
    cString text;
    int currentVal;
    const char * const *strings;
    int numValues;
    cPixmap *pixmapVal;
    void DrawValue(void);
public:
    cRecMenuItemSelect(cString text,
                       const char * const *Strings,
                       int initialVal,
                       int numValues,
                       bool active);
    virtual ~cRecMenuItemSelect(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    int GetIntValue(void) { return currentVal; };
    cString GetStringValue(void) { return strings[currentVal]; };
};

// --- cRecMenuItemText  -------------------------------------------------------
class cRecMenuItemText : public cRecMenuItem {
private:
    cString title;
    char *value;
    int length;
    const char *allowed;
    int pos, offset;
    bool insert, newchar, uppercase;
    int lengthUtf8;
    uint *valueUtf8;
    uint *allowedUtf8;
    uint *charMapUtf8;
    uint *currentCharUtf8;
    eKeys lastKey;
    cTimeMs autoAdvanceTimeout;
    cPixmap *pixmapVal;
    cStyledPixmap *pixmapKeyboard;
    cPixmap *pixmapKeyboardHighlight;
    cPixmap *pixmapKeyboardIcons;
    int keyboardWidth;
    int gridWidth;
    int gridHeight;
    int keyboardHeight;
    bool keyboardDrawn;
    uint *IsAllowed(uint c);
    void AdvancePos(void);
    uint Inc(uint c, bool Up);
    void Type(uint c);
    void Insert(void);
    void Delete(void);
    void EnterEditMode(void);
    void LeaveEditMode(bool SaveValue = false);
    bool InEditMode(void) { return valueUtf8 != NULL; };    
    void SetText(void);
    void ActivateKeyboard(void);
    void DeactivateKeyboard(void);
    void HighlightSMSKey(int num);
    void ClearSMSKey(void);
    char *GetSMSKeys(int num);
    void DrawValue(char *newValue);
public:
    cRecMenuItemText(cString title,
                     char *initialVal,
                     int length,
                     bool active);
    virtual ~cRecMenuItemText(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    void setBackground(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    cString GetStringValue(void) { return value; };
};


// --- cRecMenuItemTime  -------------------------------------------------------
class cRecMenuItemTime : public cRecMenuItem {
private:
    cString text;
    int value;
    int mm;
    int hh;
    int pos;
    bool fresh;
    cPixmap *pixmapVal;
    void DrawValue(void);
public:
    cRecMenuItemTime(cString text,
                    int initialVal,
                    bool active);
    virtual ~cRecMenuItemTime(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    int GetIntValue(void) { return value; };
};

// --- cRecMenuItemDay  -------------------------------------------------------
class cRecMenuItemDay : public cRecMenuItem {
private:
    cString text;
    time_t currentVal;
    cPixmap *pixmapVal;
    void DrawValue(void);
public:
    cRecMenuItemDay(cString text,
                    time_t initialVal,
                    bool active);
    virtual ~cRecMenuItemDay(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    time_t GetTimeValue(void) { return currentVal; };
};

// --- cRecMenuItemTimer  -------------------------------------------------------
class cRecMenuItemTimer : public cRecMenuItem {
private:
    const cTimer *timer;
    eRecMenuState action2;
    eRecMenuState action3;
    int iconActive;
    cPixmap *pixmapIcons;
    cPixmap *pixmapStatus;
    time_t conflictStart;
    time_t conflictStop;
    time_t overlapStart;
    time_t overlapStop;
    int DrawIcons(void);
    void DrawTimerConflict(void);
public:
    cRecMenuItemTimer(const cTimer *timer, 
                      eRecMenuState action1, 
                      eRecMenuState action2,
                      eRecMenuState action3,
                      time_t conflictStart,
                      time_t conflictStop,
                      time_t overlapStart,
                      time_t overlapStop,
                      bool active);
    virtual ~cRecMenuItemTimer(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
};

// --- cRecMenuItemTimerConflictHeader -------------------------------------------------------

class cRecMenuItemTimerConflictHeader: public cRecMenuItem {
private:
    cPixmap *pixmapStatus;
    time_t conflictStart;
    time_t conflictStop;
    time_t overlapStart;
    time_t overlapStop;
public:
    cRecMenuItemTimerConflictHeader(time_t conflictStart,
                                    time_t conflictStop,
                                    time_t overlapStart,
                                    time_t overlapStop);
    virtual ~cRecMenuItemTimerConflictHeader(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void);
    void setBackground(void);
    void Draw(void);
};

// --- cRecMenuItemEvent  -------------------------------------------------------
class cRecMenuItemEvent : public cRecMenuItem {
private:
    const cEvent *event;
    eRecMenuState action2;
    int iconActive;
    cPixmap *pixmapText;
    cPixmap *pixmapIcons;
    int DrawIcons(void);
public:
    cRecMenuItemEvent(const cEvent *event, 
                      eRecMenuState action1, 
                      eRecMenuState action2, 
                      bool active);
    virtual ~cRecMenuItemEvent(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    const cEvent *GetEventValue(void) { return event; };
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
};

// --- cRecMenuItemChannelChooser -------------------------------------------------------
class cRecMenuItemChannelChooser : public cRecMenuItem {
private:
    cString text;
    cChannel *channel;
    int channelNumber;
    bool initialChannelSet;
    bool fresh;
    cPixmap *pixmapChannel;
    void DrawValue(void);
public:
    cRecMenuItemChannelChooser (cString text,
                                cChannel *initialChannel,
                                bool active);
    virtual ~cRecMenuItemChannelChooser(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    int GetIntValue(void);
};

// --- cRecMenuItemDayChooser -------------------------------------------------------
class cRecMenuItemDayChooser : public cRecMenuItem {
private:
    cString text;
    int weekdays;
    std::string days;
    int daysX;
    int daysY;
    int daysSize;
    int selectedDay;
    cPixmap *pixmapWeekdays;
    cPixmap *pixmapWeekdaysSelect;
    void SetSizes(void);
    void DrawDays(void);
    void DrawHighlight(int day);
    void ToggleDay(void);
    bool WeekDaySet(unsigned day);
public:
    cRecMenuItemDayChooser (cString text,
                            int weekdays,
                            bool active);
    virtual ~cRecMenuItemDayChooser(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    void setBackground(void);
    eRecMenuState ProcessKey(eKeys Key);
    void Draw(void);
    int GetIntValue(void) { return weekdays;} ;
};

// --- cRecMenuItemRecording  -------------------------------------------------------
class cRecMenuItemRecording : public cRecMenuItem {
private:
    cRecording *recording;
    cPixmap *pixmapText;
public:
    cRecMenuItemRecording(cRecording *recording, bool active);
    virtual ~cRecMenuItemRecording(void);
    void SetPixmaps(void);
    void Hide(void);
    void Show(void); 
    void Draw(void);
};

#endif //__TVGUIDE_RECMENUITEM_H