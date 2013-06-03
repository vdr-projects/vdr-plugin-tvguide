#ifndef __TVGUIDE_DETAILVIEW_H
#define __TVGUIDE_DETAILVIEW_H

// --- cDetailView  -------------------------------------------------------------

class cEpgGrid;

class cDetailView : public cThread  {
private:
    cGrid *grid;
    cStyledPixmap *header;
    cPixmap *headerLogo;
    cPixmap *headerBack;
    cPixmap *content;
    cPixmap *scrollBar;
    cPixmap *footer;
    const cEvent *event;
    cImage *imgScrollBar;
    int FrameTime;
    int FadeTime;
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
    void drawHeader();
    void drawContent();
    void drawScrollbar();
    int heightEPGPics(void);
    void drawEPGPictures(int height);
    cImage *createScrollbar(int width, int height, tColor clrBgr, tColor clrBlend);
    virtual void Action(void);
public:
    cDetailView(cGrid *grid);
    virtual ~cDetailView(void);
    void createPixmaps();
    void scrollUp();
    void scrollDown();
};

#endif //__TVGUIDE_DETAILVIEW_H