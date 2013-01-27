#ifndef __TVGUIDE_DETAILVIEW_H
#define __TVGUIDE_DETAILVIEW_H

// --- cDetailView  -------------------------------------------------------------

class cEpgGrid;

class cDetailView : public cThread  {
private:
	cEpgGrid *grid;
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
	int borderWidth;
	int headerHeight;
	bool setContentDrawportHeight();
	int heightContent;
	int heightScrollbar;
	bool contentScrollable;
	virtual void Action(void);
	void drawHeader();
	void drawContent();
	void drawScrollbar();
	cImage *createScrollbar(int width, int height, tColor clrBgr, tColor clrBlend);
public:
	cDetailView(cEpgGrid *grid);
	virtual ~cDetailView(void);
	void createPixmaps();
	void scrollUp();
	void scrollDown();
};

#endif //__TVGUIDE_DETAILVIEW_H