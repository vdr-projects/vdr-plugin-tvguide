#ifndef __TVGUIDE_TIMELINE_H
#define __TVGUIDE_TIMELINE_H

// --- cTimeLine  -------------------------------------------------------------

class cTimeLine  {
private:
    cMyTime *myTime;
    cStyledPixmap *dateViewer;
    cPixmap *timeline;
    cStyledPixmap *clock;
    cImage *createBackgroundImage(int width, int height, tColor clrBgr, tColor clrBlend);
public:
    cTimeLine(cMyTime *myTime);
    virtual ~cTimeLine(void);
    void drawDateViewer();
    void drawTimeline();
    void setTimeline();
    void drawClock();
};

#endif //__TVGUIDE_TIMELINE_H