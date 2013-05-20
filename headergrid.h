#ifndef __TVGUIDE_HEADERGRID_H
#define __TVGUIDE_HEADERGRID_H

// --- cHeaderGrid  -------------------------------------------------------------

class cHeaderGrid : public cGrid {
private:
    cPixmap *pixmapLogo;
public:
	cHeaderGrid(void);
	virtual ~cHeaderGrid(void);
    void createBackground(int num);
    void drawChannel(const cChannel *channel);
    void setPosition(int num);
};

#endif //__TVGUIDE_HEADERGRID_H