#ifndef __TVGUIDE_DETAILVIEW_H
#define __TVGUIDE_DETAILVIEW_H

#include <vdr/osd.h>
#include <vdr/osdbase.h>
#include <vdr/epg.h>
#include "services/tvscraper.h"
#include "styledpixmap.h"
#include "footer.h"

// --- cDetailView  -------------------------------------------------------------

class cEpgGrid;

class cDetailView : public cThread {
private:
    cPixmap *back;
    cStyledPixmap *header;
    cPixmap *headerLogo;
    cPixmap *content;
    cPixmap *pixmapPoster;
    cPixmap *scrollBar;
    cFooter *footer;
    const cEvent *event;
    cImage *imgScrollBar;
    cTextWrapper description;
    cTextWrapper reruns;
    TVScraperGetFullInformation mediaInfo;
    bool hasAdditionalMedia;
    int border;
    int headerWidth;
    int headerHeight;
    int width;
    int contentWidth;
    int contentHeight;
    int contentX;
    int widthPoster;
    int yBanner;
    int yEPGText;
    int yActors;
    int yFanart;
    int yAddInf;
    int yEPGPics;
    int actorThumbWidth;
    int actorThumbHeight;
    int scrollBarWidth;
    bool setContentDrawportHeight();
    int heightContent;
    int heightScrollbar;
    int numEPGPics;
    bool contentScrollable;
    void loadReruns(void);
    int heightEPGPics(void);
    int heightActorPics(void);
    int heightFanartImg(void);
    void drawEPGPictures(int height);
    void drawRecIcon(void);
    void drawPoster(void);
    void drawBanner(int height);
    void drawActors(int height);
    void drawFanart(int height);
    cImage *createScrollbar(int width, int height, tColor clrBgr, tColor clrBlend);
    void scrollUp();
    void scrollDown();
    void pageUp();
    void pageDown();
    void Action(void);
public:
    cDetailView(const cEvent *event, cFooter *footer);
    virtual ~cDetailView(void);
    void setContent();
    void createPixmaps();
    void drawHeader();
    void drawContent();
    void drawScrollbar();
    eOSState ProcessKey(eKeys Key);
};

#endif //__TVGUIDE_DETAILVIEW_H