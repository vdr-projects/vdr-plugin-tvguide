#ifndef __TVGUIDE_EPGGRID_H
#define __TVGUIDE_EPGGRID_H

// --- cEpgGrid  -------------------------------------------------------------

class cEpgGrid : public cListObject, public cStyledPixmap {
private:
	const cEvent *event;
	cTextWrapper *text;
	cTextWrapper *extText;
	int viewportHeight;
	int borderWidth;
	void drawText();
	void setBackground();
	bool isColor1;
	bool active;
	bool dirty;
	bool intersects(cEpgGrid *neighbor);
	bool hasTimer;
	void DrawRecIcon();
public:
	cEpgGrid(cChannelColumn *c, const cEvent *event);
	virtual ~cEpgGrid(void);
	cChannelColumn *column;
	void SetViewportHeight();
	void PositionPixmap();
	void setText();
	void Draw();
	void SetDirty() {dirty = true;};
	void SetActive() {dirty = true; active = true;};
	void SetInActive() {dirty = true; active = false;};
	void SetColor(bool color) {isColor1 = color;};
	bool IsColor1() {return isColor1;};
	int GetViewportHeight() {return viewportHeight;};
	const cEvent *GetEvent() {return event;};
	bool isActiveInitial(time_t t);
	time_t StartTime() { return event->StartTime(); };
	time_t EndTime() { return event->EndTime(); };
	int calcOverlap(cEpgGrid *neighbor);
	void setTimer() {hasTimer = true;};
	void debug();
};

#endif //__TVGUIDE_EPGGRID_H