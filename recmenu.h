#ifndef __TVGUIDE_RECMENU_H
#define __TVGUIDE_RECMENU_H

// --- cRecMenu  -------------------------------------------------------------

class cRecMenu : public cStyledPixmap {
protected:
    int x, y;
    int width, height;
    int headerHeight, footerHeight;
    int scrollHeight;
    int scrollItemHeight;
    int scrollbarWidth;
    cPixmap *pixmapScrollBar;
    cImage *imgScrollBar;
    int border;
    bool scrollable;
    int numItems;
    int startIndex, stopIndex;
    cRecMenuItem *header;
    cRecMenuItem *footer;
    cList<cRecMenuItem> menuItems;
    void SetWidthPercent(int percentOSDWidth);
    void SetWidthPixel(int pixel);
    int CalculateOptimalWidth(void);
    bool CheckHeight(void);
    void CalculateHeight(void);
    void CreatePixmap(void);
    void SetHeader(cRecMenuItem *header);
    void SetFooter(cRecMenuItem *footer);
    void AddMenuItemScroll(cRecMenuItem *item);
    void AddMenuItem(cRecMenuItem *item, cRecMenuItem *before = NULL);
    cRecMenuItem *GetActiveMenuItem(void);
    bool ActivateNext(void);
    bool ActivatePrev(void);
    void Activate(cRecMenuItem *itemOld, cRecMenuItem *item);
    void ScrollUp(void);
    void ScrollDown(void);
    void JumpBegin(void);
    void JumpEnd(void);
    void DrawScrollBar(void);
    cImage *createScrollbar(int width, int height, tColor clrBgr, tColor clrBlend);
    void Arrange(bool scroll = false);
    virtual cRecMenuItem *GetMenuItem(int number) { return NULL; };
    virtual int GetTotalNumMenuItems(void) { return 0; };
    virtual void CreateMenuItems(void) {};
public:
    cRecMenu(void);
    virtual ~cRecMenu(void);
    void Display(bool scroll = false);
    void Hide(void);
    void Show(void);
    int GetActive(bool withOffset);
    int GetIntValue(int itemNumber);
    time_t GetTimeValue(int itemNumber);
    bool GetBoolValue(int itemNumber);
    cString GetStringValue(int itemNumber);
    const cEvent *GetEventValue(int itemNumber);
    eRecMenuState ProcessKey(eKeys Key);
};
#endif //__TVGUIDE_RECMENU_H