#ifndef __TVGUIDE_FOOTER_H
#define __TVGUIDE_FOOTER_H

// --- cFooter  -------------------------------------------------------------

class cFooter  {
private:
	cPixmap *footer;
	int buttonWidth;
    int buttonHeight;
    int buttonY;
    int buttonBorder;
    void DrawButton(const char *text, tColor color, tColor borderColor, int num);
public:
	cFooter();
	virtual ~cFooter(void);
	void drawRedButton();
	void drawGreenButton();
	void drawYellowButton();
	void drawBlueButton();
};

#endif //__TVGUIDE_FOOTER_H