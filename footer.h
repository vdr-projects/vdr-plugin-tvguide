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
    cChannelGroups *channelGroups;
    int currentGroup;
    void DrawButton(const char *text, tColor color, tColor borderColor, int num);
public:
    cFooter(cChannelGroups *channelGroups);
    virtual ~cFooter(void);
    void drawRedButton();
    void drawGreenButton();
    void drawYellowButton();
    void drawGreenButton(const char *text);
    void drawYellowButton(const char *text);
    void drawBlueButton();
    void UpdateGroupButtons(const cChannel *channel);
};

#endif //__TVGUIDE_FOOTER_H