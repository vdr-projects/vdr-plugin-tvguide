#ifndef __TVGUIDE_DETAILVIEW_H
#define __TVGUIDE_DETAILVIEW_H

// --- cDetailView  -------------------------------------------------------------

class cEpgGrid;

class cDetailView {
private:
    cStyledPixmap *header;
    cPixmap *headerLogo;
    cPixmap *headerBack;
    cPixmap *content;
    cPixmap *scrollBar;
    cPixmap *footer;
    const cEvent *event;
    cImage *imgScrollBar;
    cTextWrapper description;
    cTextWrapper reruns;
    int borderWidth;
    int headerHeight;
    int scrollBarWidth;
    bool setContentDrawportHeight();
    int heightContent;
    int heightScrollbar;
    int numEPGPics;
    bool contentScrollable;
    void loadReruns(void);
    int heightEPGPics(void);
    void drawEPGPictures(int height);
    cImage *createScrollbar(int width, int height, tColor clrBgr, tColor clrBlend);
public:
    cDetailView(const cEvent *event);
    virtual ~cDetailView(void);
    void createPixmaps();
    void drawHeader();
    void drawContent();
    void drawScrollbar();
    void scrollUp();
    void scrollDown();
    eOSState ProcessKey(eKeys Key);
};

#endif //__TVGUIDE_DETAILVIEW_H