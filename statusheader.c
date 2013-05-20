#include "statusheader.h"

cStatusHeader::cStatusHeader(void) {
    color = theme.Color(clrStatusHeader);
    colorBlending = theme.Color(clrStatusHeaderBlending);
    int height = tvguideConfig.statusHeaderHeight;
    int width;
    if (tvguideConfig.scaleVideo) {
        width = tvguideConfig.osdWidth - height * 16 / 9;
    } else {
        width = tvguideConfig.osdWidth;
    }
    pixmap = osdManager.requestPixmap(1, cRect(0, 0, width, height));
    pixmapText = osdManager.requestPixmap(2, cRect(0, 0, width, height));
    pixmapText->Fill(clrTransparent);
    drawBackground();
    drawBorder();
}

cStatusHeader::~cStatusHeader(void) {
    if (tvguideConfig.scaleVideo) {
        cRect vidWin = cDevice::PrimaryDevice()->CanScaleVideo(cRect::Null);
        cDevice::PrimaryDevice()->ScaleVideo(vidWin);
    }
}

void cStatusHeader::ScaleVideo(void) {
    if (tvguideConfig.scaleVideo) {
        int height = tvguideConfig.statusHeaderHeight;
        int width = height * 16 / 9;
        int x = tvguideConfig.osdWidth - width;
        int y = 0;
        cRect availableRect(x, y, width, height);
        cRect vidWin = cDevice::PrimaryDevice()->CanScaleVideo(availableRect);
        cDevice::PrimaryDevice()->ScaleVideo(vidWin);
    }
}

void cStatusHeader::DrawInfoText(cGrid *grid) {
    int border = 5;
    pixmapText->Fill(clrTransparent);
    int x = border;
    int y = border;
    if (!grid->isDummy()) {
        cString time = grid->getTimeString();
        cString title("");
        const cEvent *event = grid->GetEvent();
        title = cString::sprintf(": %s", event->Title());
        cString header = cString::sprintf("%s%s", *time, *title);
        header = CutText(*header, pixmapText->ViewPort().Width() - 2 * border, tvguideConfig.FontStatusHeaderLarge).c_str();
        pixmapText->DrawText(cPoint(x,y), *header, theme.Color(clrFont), clrTransparent, tvguideConfig.FontStatusHeaderLarge);
        y += tvguideConfig.FontStatusHeaderLarge->Height() + border;
        int heightText = pixmapText->ViewPort().Height() - y;
        cTextWrapper description;
        description.Set(event->Description(), tvguideConfig.FontStatusHeader, pixmapText->ViewPort().Width() - 2 * border);
        int lineHeight = tvguideConfig.FontStatusHeader->Height();
        int textLines = description.Lines();
        int maxLines = heightText / lineHeight;
        int lines = min(textLines, maxLines);
        for (int i = 0; i < lines-1; i++) {
            pixmapText->DrawText(cPoint(x,y), description.GetLine(i), theme.Color(clrFont), clrTransparent, tvguideConfig.FontStatusHeader);
            y += lineHeight;
        }
        cString lastLine = description.GetLine(lines-1);
        if (textLines > maxLines) {
            lastLine = cString::sprintf("%s...", *lastLine);
        }
        pixmapText->DrawText(cPoint(x,y), *lastLine, theme.Color(clrFont), clrTransparent, tvguideConfig.FontStatusHeader);
    } else {
        int heightText = pixmapText->ViewPort().Height() - y;
        y += (heightText - tvguideConfig.FontStatusHeaderLarge->Height() - 2*border)/2;
        pixmapText->DrawText(cPoint(x,y), *grid->getText(), theme.Color(clrFont), clrTransparent, tvguideConfig.FontStatusHeaderLarge);
    }
}