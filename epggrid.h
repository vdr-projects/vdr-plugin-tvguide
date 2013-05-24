#ifndef __TVGUIDE_EPGGRID_H
#define __TVGUIDE_EPGGRID_H

// --- cEpgGrid  -------------------------------------------------------------

class cEpgGrid : public cGrid {
private:
	const cEvent *event;
	cTextWrapper *extText;
    cString timeString;
    bool hasTimer;
	void drawText();
	void drawRecIcon();
    time_t Duration(void) { return event->Duration(); };
public:
	cEpgGrid(cChannelColumn *c, const cEvent *event);
	virtual ~cEpgGrid(void);
	void SetViewportHeight();
    void PositionPixmap();
    void setText(void);
	const cEvent *GetEvent() {return event;};
	time_t StartTime() { return event->StartTime(); };
	time_t EndTime() { return event->EndTime(); };
	void setTimer() {hasTimer = true;};
    cString getTimeString(void);
	void debug();
};

#endif //__TVGUIDE_EPGGRID_H