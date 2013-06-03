#include "channelgroup.h"

cChannelGroup::cChannelGroup(const char *name) {
    channelStart = 0;
    channelStop = 0;
    this->name = name;
}

cChannelGroup::~cChannelGroup(void) {
}

void cChannelGroup::Dump(void) {
    esyslog("tvguide: Group %s, startChannel %d, stopChannel %d", name, channelStart, channelStop);
}

// --- cChannelGroupGrid  -------------------------------------------------------------

cChannelGroupGrid::cChannelGroupGrid(const char *name) {
    this->name = name;
}

cChannelGroupGrid::~cChannelGroupGrid(void) {
}

void cChannelGroupGrid::SetBackground() {
    if (isColor1) {
        color = theme.Color(clrGrid1);
        colorBlending = theme.Color(clrGrid1Blending);
    } else {
        color = theme.Color(clrGrid2);
        colorBlending = theme.Color(clrGrid2Blending);
    }
}

void cChannelGroupGrid::SetGeometry(int start, int end) {
    int x, y, width, height;
    if (tvguideConfig.displayMode == eVertical) {
        x = tvguideConfig.timeLineWidth + start*tvguideConfig.colWidth;
        y = tvguideConfig.statusHeaderHeight;
        width = (end - start + 1) * tvguideConfig.colWidth;
        height = tvguideConfig.channelGroupsHeight;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        x = 0;
        y = tvguideConfig.statusHeaderHeight +  tvguideConfig.timeLineHeight + start*tvguideConfig.rowHeight;
        width = tvguideConfig.channelGroupsWidth;
        height = (end - start + 1) * tvguideConfig.rowHeight;
    }
    pixmap = osdManager.requestPixmap(1, cRect(x, y, width, height));
}

void cChannelGroupGrid::Draw(void) {
    drawBackground();
    drawBorder();
    tColor colorText = theme.Color(clrFont);
    tColor colorTextBack = (tvguideConfig.useBlending==0)?color:clrTransparent;
    if (tvguideConfig.displayMode == eVertical) {
        int textY = (Height() - tvguideConfig.FontChannelGroups->Height()) / 2;
        cString text = CutText(name, Width() - 4, tvguideConfig.FontChannelGroups).c_str();
        int textWidth = tvguideConfig.FontChannelGroups->Width(*text);
        int x = (Width() - textWidth) / 2;
        pixmap->DrawText(cPoint(x, textY), *text, colorText, colorTextBack, tvguideConfig.FontChannelGroups);
    } else if (tvguideConfig.displayMode == eHorizontal) {
	std::string nameUpper = name;
        std::transform(nameUpper.begin(), nameUpper.end(),nameUpper.begin(), ::toupper);
        int numChars = nameUpper.length();
        int charHeight = tvguideConfig.FontChannelGroupsHorizontal->Height();
        int textHeight = numChars * charHeight;
        int y = 5;
        if ((textHeight +5) < Height()) {
            y = (Height() - textHeight) / 2;
        }
        for (int i=0; i < numChars; i++) {
            if (((y + 2*charHeight) > Height()) && ((i+1)<numChars)) {
                int x = (Width() - tvguideConfig.FontChannelGroupsHorizontal->Width("...")) / 2;
                pixmap->DrawText(cPoint(x, y), "...", colorText, colorTextBack, tvguideConfig.FontChannelGroupsHorizontal);
                break;
            }
            cString currentChar = cString::sprintf("%c", nameUpper.at(i));
            int x = (Width() - tvguideConfig.FontChannelGroupsHorizontal->Width(*currentChar)) / 2;
            pixmap->DrawText(cPoint(x, y), *currentChar, colorText, colorTextBack, tvguideConfig.FontChannelGroupsHorizontal);
            y += tvguideConfig.FontChannelGroupsHorizontal->Height();
        }
    }
}