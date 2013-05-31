#include "footer.h"

cFooter::cFooter(cChannelGroups *channelGroups) {
    this->channelGroups = channelGroups;
    currentGroup = -1;
    buttonBorder = 20;
    buttonWidth = (tvguideConfig.osdWidth - tvguideConfig.timeLineWidth - 5*buttonBorder)/4;
    buttonHeight= tvguideConfig.footerHeight - 2*buttonBorder;
    buttonY = (tvguideConfig.footerHeight - buttonHeight)/2;
    
    footer = osdManager.requestPixmap(2, cRect( tvguideConfig.timeLineWidth, 
                                                tvguideConfig.osdHeight - tvguideConfig.footerHeight, 
                                                tvguideConfig.osdWidth - tvguideConfig.timeLineWidth, 
                                                tvguideConfig.footerHeight),
                                         cRect::Null);
    footer->Fill(clrTransparent);
}

cFooter::~cFooter(void) {
    osdManager.releasePixmap(footer);
}

void cFooter::DrawButton(const char *text, tColor color, tColor borderColor, int num) {
    tColor colorTextBack = (tvguideConfig.useBlending==0)?color:clrTransparent;
    int left = num * buttonWidth + (num + 1) * buttonBorder;
    footer->DrawRectangle(cRect(left, buttonY, buttonWidth, buttonHeight), borderColor);
    if (tvguideConfig.useBlending) {
        cImageLoader imgLoader;
        imgLoader.DrawBackground(theme.Color(clrButtonBlend), color, buttonWidth-4, buttonHeight-4);
        footer->DrawImage(cPoint(left+2, buttonY+2), imgLoader.GetImage());    
    } else {
        footer->DrawRectangle(cRect(left, buttonY, buttonWidth, buttonHeight), borderColor); 
        footer->DrawRectangle(cRect(left+2, buttonY+2, buttonWidth-4, buttonHeight-4), color); 
    }
    int textWidth = tvguideConfig.FontButton->Width(text);
    int textHeight = tvguideConfig.FontButton->Height();
    footer->DrawText(cPoint(left + (buttonWidth-textWidth)/2, buttonY + (buttonHeight-textHeight)/2), text, theme.Color(clrFontButtons), colorTextBack, tvguideConfig.FontButton);
}

void cFooter::drawRedButton() {
    cString text(tr("Set Timer"));
    DrawButton(*text, theme.Color(clrButtonRed), theme.Color(clrButtonRedBorder), 0);
}

void cFooter::drawGreenButton() {
    cString text = cString::sprintf("%d %s", tvguideConfig.jumpChannels, tr("Channels back"));
    DrawButton(*text, theme.Color(clrButtonGreen), theme.Color(clrButtonGreenBorder), 1);
}

void cFooter::drawGreenButton(const char *text) {
    std::string cuttedText = CutText(text, buttonWidth-6, tvguideConfig.FontButton);
    DrawButton(cuttedText.c_str(), theme.Color(clrButtonGreen), theme.Color(clrButtonGreenBorder), 1);
}

void cFooter::drawYellowButton() {
    cString text = cString::sprintf("%d %s", tvguideConfig.jumpChannels, tr("Channels forward"));
    DrawButton(*text, theme.Color(clrButtonYellow), theme.Color(clrButtonYellowBorder), 2);
}

void cFooter::drawYellowButton(const char *text) {
    std::string cuttedText = CutText(text, buttonWidth-6, tvguideConfig.FontButton);
    DrawButton(cuttedText.c_str(), theme.Color(clrButtonYellow), theme.Color(clrButtonYellowBorder), 2);
}

void cFooter::drawBlueButton() {
    cString text(tr("Switch to Channel"));
    DrawButton(*text, theme.Color(clrButtonBlue), theme.Color(clrButtonBlueBorder), 3);
}

void cFooter::UpdateGroupButtons(const cChannel *channel) {
    int group = channelGroups->GetGroup(channel);
    if (group != currentGroup) {
        currentGroup = group;
        drawGreenButton(channelGroups->GetPrev(group));
        drawYellowButton(channelGroups->GetNext(group));
    }
}