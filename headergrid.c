#include "headergrid.h"

cHeaderGrid::cHeaderGrid(void) : cGrid(NULL) {
    pixmap = NULL;
    pixmapLogo = NULL;
}

cHeaderGrid::~cHeaderGrid(void) {
    osdManager.releasePixmap(pixmapLogo);
}

void cHeaderGrid::createBackground(int num) {
    color = theme.Color(clrHeader);
    colorBlending = theme.Color(clrHeaderBlending);
    int x, y, width, height;
    if (tvguideConfig.displayMode == eVertical) {
        x = tvguideConfig.timeLineWidth + num*tvguideConfig.colWidth;
        y = tvguideConfig.statusHeaderHeight;
        width = tvguideConfig.colWidth;
        height = tvguideConfig.channelHeaderHeight;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        x = 0;
        y = tvguideConfig.statusHeaderHeight + tvguideConfig.timeLineHeight + num*tvguideConfig.rowHeight;
        width = tvguideConfig.channelHeaderWidth;
        height = tvguideConfig.rowHeight;
    }
    pixmap = osdManager.requestPixmap(2, cRect(x, y, width, height));
    pixmapLogo = osdManager.requestPixmap(3, cRect(x, y, width, height));
    if ((!pixmap) || (!pixmapLogo)){
        return;
    }
    pixmapLogo->Fill(clrTransparent);
    drawBackground();
}

void cHeaderGrid::drawChannel(const cChannel *channel) {
    if (tvguideConfig.displayMode == eVertical) {
        drawChannelVertical(channel);
    } else if (tvguideConfig.displayMode == eHorizontal) {
        drawChannelHorizontal(channel);
    }
    drawBorder();
}

void cHeaderGrid::drawChannelHorizontal(const cChannel *channel) {
    int logoWidth = Height() * tvguideConfig.logoWidthRatio / tvguideConfig.logoHeightRatio;
    int logoX = tvguideConfig.displayChannelName?2:(Width()-logoWidth)/2;
    int textX = 5;
    int textY = (Height() - tvguideConfig.FontChannelHeaderHorizontal->Height())/2;
    bool logoFound = false;
    if (!tvguideConfig.hideChannelLogos) {
        cImageLoader imgLoader;
        if (imgLoader.LoadLogo(channel->Name(), logoWidth, Height())) {
            cImage logo = imgLoader.GetImage();
            pixmapLogo->DrawImage(cPoint(logoX, 0), logo);
            logoFound = true;
        }
    }
    bool drawText = false;
    int textWidthMax = Width() - 10;
    if (!logoFound) {
        drawText = true;
    } else  if (tvguideConfig.displayChannelName) {
        drawText = true;
        textX += logoWidth;
        textWidthMax -= logoWidth;
    }
    if (drawText) {
        tColor colorTextBack = (tvguideConfig.useBlending==0)?color:clrTransparent;
        cString strChannel = cString::sprintf("%d %s", channel->Number(), channel->Name());
        strChannel = CutText(*strChannel, textWidthMax, tvguideConfig.FontChannelHeaderHorizontal).c_str();
        pixmap->DrawText(cPoint(textX, textY), *strChannel, theme.Color(clrFontHeader), colorTextBack, tvguideConfig.FontChannelHeaderHorizontal);
    }
}

void cHeaderGrid::drawChannelVertical(const cChannel *channel) {
    int logoWidth = Width()/2 - 15;
    int logoHeight = logoWidth * tvguideConfig.logoHeightRatio / tvguideConfig.logoWidthRatio;
    cTextWrapper tw;
    cString headerText = cString::sprintf("%d - %s", channel->Number(), channel->Name());
    tw.Set(*headerText, tvguideConfig.FontChannelHeader, tvguideConfig.colWidth - 8);
    int lines = tw.Lines();
    int lineHeight = tvguideConfig.FontChannelHeader->Height();
    int yStart = (tvguideConfig.channelHeaderHeight - lines*lineHeight)/2 + 8;
    bool logoFound = false;
    if (!tvguideConfig.hideChannelLogos) {
        cImageLoader imgLoader;
        if (imgLoader.LoadLogo(channel->Name(), logoWidth, logoHeight)) {
                cImage logo = imgLoader.GetImage();
                pixmapLogo->DrawImage(cPoint((Width() - logoWidth)/2, 4), logo);
                logoFound = true;
        }
    }
    bool drawText = false;
    if (!logoFound) {
        drawText = true;
    } else if (tvguideConfig.displayChannelName) {
        drawText = true;
        yStart = logoHeight;
    }
    if (!drawText)
        return;
    tColor colorTextBack = (tvguideConfig.useBlending==0)?color:clrTransparent;
    for (int i=0; i<lines; i++) {
        int textWidth = tvguideConfig.FontChannelHeader->Width(tw.GetLine(i));
        int xText = (tvguideConfig.colWidth - textWidth) / 2;
        if (xText < 0) 
            xText = 0;
        pixmap->DrawText(cPoint(xText, yStart + i*lineHeight), tw.GetLine(i), theme.Color(clrFontHeader), colorTextBack, tvguideConfig.FontChannelHeader);
    }
}

void cHeaderGrid::setPosition(int num) {
    int x, y, width, height;
    if (tvguideConfig.displayMode == eVertical) {
        x = tvguideConfig.timeLineWidth + num*tvguideConfig.colWidth;
        y = tvguideConfig.statusHeaderHeight;
        width = tvguideConfig.colWidth;
        height = tvguideConfig.channelHeaderHeight;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        x = 0;
        y = tvguideConfig.statusHeaderHeight + tvguideConfig.timeLineHeight + num*tvguideConfig.rowHeight;
        width = tvguideConfig.channelHeaderWidth;
        height = tvguideConfig.rowHeight;
    }
    pixmap->SetViewPort(cRect(x, y, width, height));
    pixmapLogo->SetViewPort(cRect(x, y, width, height));
}
