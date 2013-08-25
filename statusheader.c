#include "statusheader.h"

cStatusHeader::cStatusHeader(void) {
    color = theme.Color(clrStatusHeader);
    colorBlending = theme.Color(clrStatusHeaderBlending);
    height = tvguideConfig.statusHeaderHeight;
    if (tvguideConfig.scaleVideo) {
        width = tvguideConfig.osdWidth - height * 16 / 9;
    } else {
        width = tvguideConfig.osdWidth;
    }
    int tvFrameWidth = tvguideConfig.osdWidth - width;
    pixmap = osdManager.requestPixmap(1, cRect(0, 0, width, height));
    pixmapText = osdManager.requestPixmap(2, cRect(0, 0, width, height));
    pixmapText->Fill(clrTransparent);
    
    pixmapTVFrame = osdManager.requestPixmap(1, cRect(width, 0, tvFrameWidth, height));
    pixmapTVFrame->Fill(clrTransparent);
    if (tvguideConfig.decorateVideo) {
        int radius = 16;
	int frame = 10;
        pixmapTVFrame->DrawRectangle(cRect(0, 0, tvFrameWidth, frame), theme.Color(clrBackground));
        pixmapTVFrame->DrawEllipse(cRect(frame,frame,radius,radius), theme.Color(clrBackground), -2);
        pixmapTVFrame->DrawRectangle(cRect(tvFrameWidth - frame, frame, frame, height - 2*frame), theme.Color(clrBackground));		
        pixmapTVFrame->DrawEllipse(cRect(tvFrameWidth - radius - frame, frame, radius, radius), theme.Color(clrBackground), -1);
        pixmapTVFrame->DrawRectangle(cRect(0, frame, frame, height - 2*frame), theme.Color(clrBackground));	
        pixmapTVFrame->DrawEllipse(cRect(frame, height - radius - frame, radius, radius), theme.Color(clrBackground), -3);
        pixmapTVFrame->DrawRectangle(cRect(0, height - frame, tvFrameWidth, frame), theme.Color(clrBackground));
        pixmapTVFrame->DrawEllipse(cRect(tvFrameWidth - radius - frame, height - radius - frame, radius, radius), theme.Color(clrBackground), -4);

    }
    drawBackground();
    drawBorder();
}

cStatusHeader::~cStatusHeader(void) {
    osdManager.releasePixmap(pixmapText);
    osdManager.releasePixmap(pixmapTVFrame);
    if (tvguideConfig.scaleVideo) {
        cRect vidWin = cDevice::PrimaryDevice()->CanScaleVideo(cRect::Null);
        cDevice::PrimaryDevice()->ScaleVideo(vidWin);
    }
}

void cStatusHeader::ScaleVideo(void) {
    if (tvguideConfig.scaleVideo) {
        int width = height * 16 / 9;
        int x = osdManager.Left() + tvguideConfig.osdWidth - width;
        int y = osdManager.Top();
        cRect availableRect(x, y, width, height);
        cRect vidWin = cDevice::PrimaryDevice()->CanScaleVideo(availableRect);
        cDevice::PrimaryDevice()->ScaleVideo(vidWin);
    }
}

void cStatusHeader::DrawInfoText(cGrid *grid) {
    int border = 10;
    int textWidth = width - 2 * border;
    tColor colorTextBack = (tvguideConfig.useBlending==0)?color:clrTransparent;
    pixmapText->Fill(clrTransparent);
    int x = border;
    int y = border;
    if (!grid->isDummy()) {
        const cEvent *event = grid->GetEvent();
        int newX = DrawPoster(event, x, y, height-2*border, border);
        if (newX > 0) {
            textWidth -= (newX - x);
            x += newX;
        }
        cString time = grid->getTimeString();
        cString title("");
        title = cString::sprintf(": %s", event->Title());
        cString header = cString::sprintf("%s%s", *time, *title);
        header = CutText(*header, textWidth, tvguideConfig.FontStatusHeaderLarge).c_str();
        pixmapText->DrawText(cPoint(x,y), *header, theme.Color(clrFont), colorTextBack, tvguideConfig.FontStatusHeaderLarge);
        y += tvguideConfig.FontStatusHeaderLarge->Height() + border;
        int heightText = pixmapText->ViewPort().Height() - y;
        cTextWrapper description;
        description.Set(event->Description(), tvguideConfig.FontStatusHeader, textWidth);
        int lineHeight = tvguideConfig.FontStatusHeader->Height();
        int textLines = description.Lines();
        int maxLines = heightText / lineHeight;
        int lines = min(textLines, maxLines);
        for (int i = 0; i < lines-1; i++) {
            pixmapText->DrawText(cPoint(x,y), description.GetLine(i), theme.Color(clrFont), colorTextBack, tvguideConfig.FontStatusHeader);
            y += lineHeight;
        }
        cString lastLine = description.GetLine(lines-1);
        if (textLines > maxLines) {
            lastLine = cString::sprintf("%s...", *lastLine);
        }
        pixmapText->DrawText(cPoint(x,y), *lastLine, theme.Color(clrFont), colorTextBack, tvguideConfig.FontStatusHeader);
    } else {
        int heightText = pixmapText->ViewPort().Height() - y;
        y += (heightText - tvguideConfig.FontStatusHeaderLarge->Height() - 2*border)/2;
        pixmapText->DrawText(cPoint(x,y), *grid->getText(), theme.Color(clrFont), colorTextBack, tvguideConfig.FontStatusHeaderLarge);
    }
}

int cStatusHeader::DrawPoster(const cEvent *event, int x, int y, int height, int border) {
    bool hasPoster = false;
    TVScraperGetPoster poster;
    int posterWidth = 0;
    int posterHeight = 0;
    if (event) {
        static cPlugin *pTVScraper = cPluginManager::GetPlugin("tvscraper");
        if (pTVScraper) {
            poster.event = event;
            poster.isRecording = false;
            if (pTVScraper->Service("TVScraperGetPoster", &poster)) {
                hasPoster = true;
                int posterWidthOrig = poster.media.width;
                int posterHeightOrig = poster.media.height;
                if ((posterWidthOrig > 10) && (posterHeightOrig > 10)) {
                    posterHeight = height;
                    posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
                }
            }
        }
    }
    if (hasPoster) {
        cImageLoader imgLoader;
        if (imgLoader.LoadPoster(poster.media.path.c_str(), posterWidth, posterHeight)) {
            pixmapText->DrawImage(cPoint(x, y), imgLoader.GetImage());
            return posterWidth + border;
        }
    }
    return 0;
}
