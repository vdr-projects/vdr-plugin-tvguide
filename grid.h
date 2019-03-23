#ifndef __TVGUIDE_GRID_H
#define __TVGUIDE_GRID_H

#include <vdr/tools.h>
#include "styledpixmap.h"

class cChannelColumn;

// --- cEpgGrid  -------------------------------------------------------------

class cGrid : public cListObject, public cStyledPixmap {
protected:
    cTextWrapper *text;
    int viewportHeight;
    int borderWidth;
    void setBackground();
    bool isColor1;
    bool active;
    bool dirty;
    bool hasTimer;
    bool hasSwitchTimer;
    bool intersects(cGrid *neighbor);
    virtual time_t Duration(void) { return 0; };
    virtual void drawText(void) {};
    bool dummy;
public:
    cGrid(cChannelColumn *c);
    virtual ~cGrid(void);
    cChannelColumn *column;
    virtual void SetViewportHeight(void) {};
    virtual void PositionPixmap(void) {};
    virtual void setText(void) {};
    void Draw(void);
    void SetDirty(void) {dirty = true;};
    void SetActive(void) {dirty = true; active = true;};
    void SetInActive(void) {dirty = true; active = false;};
    void SetColor(bool color) {isColor1 = color;};
    bool IsColor1(void) {return isColor1;};
    bool isFirst(void);
    virtual const cEvent *GetEvent(void) { return NULL; };
    bool Match(time_t t);
    virtual time_t StartTime(void) { return 0; };
    virtual time_t EndTime(void) { return 0; };
    virtual void SetStartTime(time_t start) {};
    virtual void SetEndTime(time_t end) {};
    int calcOverlap(cGrid *neighbor);
    virtual void SetTimer(void) {};
    virtual void SetSwitchTimer(void) {};
    virtual cString getText(void) { return cString("");};
    virtual cString getTimeString(void) { return cString("");};
    bool Active(void) { return active; };
    bool HasTimer(void) {return hasTimer;};
    bool HasSwitchTimer(void) {return hasSwitchTimer;};
    bool isDummy(void) { return dummy; };
    virtual void debug() {};
};

#endif //__TVGUIDE_GRID_H
