#ifndef __TVGUIDE_FOOTER_H
#define __TVGUIDE_FOOTER_H

// --- cFooter  -------------------------------------------------------------

class cFooter  {
private:
	cStyledPixmap *buttonRed;
	cStyledPixmap *buttonGreen;
	cStyledPixmap *buttonYellow;
	cStyledPixmap *buttonBlue;
	int textY;
	int buttonWidth;
public:
	cFooter();
	virtual ~cFooter(void);
	void drawRedButton();
	void drawGreenButton();
	void drawYellowButton();
	void drawBlueButton();
};

#endif //__TVGUIDE_FOOTER_H