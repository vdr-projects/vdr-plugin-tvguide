#include "imageloader.h"
#include "timeline.h"

cTimeLine::cTimeLine(cMyTime *myTime) {
    this->myTime = myTime;
    if (tvguideConfig.displayMode == eVertical) {
        dateViewer = new cStyledPixmap(osdManager.requestPixmap(1, cRect(0, 
                                                                         tvguideConfig.statusHeaderHeight, 
                                                                         tvguideConfig.timeLineWidth, 
                                                                         tvguideConfig.channelHeaderHeight + tvguideConfig.channelGroupsHeight)));
        timeline = osdManager.requestPixmap(2, cRect(0, 
                                                     tvguideConfig.statusHeaderHeight + tvguideConfig.channelHeaderHeight + tvguideConfig.channelGroupsHeight, 
                                                     tvguideConfig.timeLineWidth, 
                                                     tvguideConfig.osdHeight - tvguideConfig.statusHeaderHeight - tvguideConfig.channelHeaderHeight - tvguideConfig.channelGroupsHeight - tvguideConfig.footerHeight)
                                             , cRect(0,
                                                     0, 
                                                     tvguideConfig.timeLineWidth, 
                                                     1440*tvguideConfig.minutePixel));
        timelineBack = osdManager.requestPixmap(1, cRect(0, 
                                                     tvguideConfig.statusHeaderHeight + tvguideConfig.channelHeaderHeight + tvguideConfig.channelGroupsHeight, 
                                                     tvguideConfig.timeLineWidth, 
                                                     tvguideConfig.osdHeight - tvguideConfig.statusHeaderHeight - tvguideConfig.channelHeaderHeight - tvguideConfig.channelGroupsHeight - tvguideConfig.footerHeight));
    } else if (tvguideConfig.displayMode == eHorizontal) {
        dateViewer = new cStyledPixmap(osdManager.requestPixmap(1, cRect(0, 
                                                                         tvguideConfig.statusHeaderHeight, 
                                                                         tvguideConfig.channelHeaderWidth + tvguideConfig.channelGroupsWidth,
                                                                         tvguideConfig.timeLineHeight-2)));
        timeline = osdManager.requestPixmap(2, cRect(tvguideConfig.channelHeaderWidth + tvguideConfig.channelGroupsWidth, 
                                                     tvguideConfig.statusHeaderHeight, 
                                                     tvguideConfig.osdWidth - tvguideConfig.channelHeaderWidth - tvguideConfig.channelGroupsWidth,
                                                     tvguideConfig.timeLineHeight)
                                             , cRect(0,
                                                     0, 
                                                     1440*tvguideConfig.minutePixel, 
                                                     tvguideConfig.timeLineWidth));
        timelineBack = osdManager.requestPixmap(1, cRect(tvguideConfig.channelHeaderWidth + tvguideConfig.channelGroupsWidth, 
                                                     tvguideConfig.statusHeaderHeight, 
                                                     tvguideConfig.osdWidth - tvguideConfig.channelHeaderWidth - tvguideConfig.channelGroupsWidth,
                                                     tvguideConfig.timeLineHeight));
    }
    clock = new cStyledPixmap(osdManager.requestPixmap(1, cRect(0, 
                                                                tvguideConfig.osdHeight- tvguideConfig.footerHeight, 
                                                                tvguideConfig.timeLineWidth, 
                                                                tvguideConfig.footerHeight-9)));
}

cTimeLine::~cTimeLine(void) {
    delete dateViewer;
    osdManager.releasePixmap(timeline);
    osdManager.releasePixmap(timelineBack);
    if (clock)
        delete clock;
}

void cTimeLine::drawDateViewer() {
    cString weekDay = myTime->GetWeekday();
    cString date = myTime->GetDate();   
    dateViewer->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
    dateViewer->drawBackground();
    dateViewer->drawBorder();
    tColor colorFontBack = (tvguideConfig.useBlending==0)?theme.Color(clrHeader):clrTransparent;

    if (tvguideConfig.displayMode == eVertical) {
        int textHeight = tvguideConfig.FontTimeLineWeekday->Height();
        int weekdayWidth = tvguideConfig.FontTimeLineWeekday->Width(*weekDay);
        int dateWidth = tvguideConfig.FontTimeLineDate->Width(*date);
        dateViewer->DrawText(cPoint((tvguideConfig.timeLineWidth-weekdayWidth)/2, (tvguideConfig.channelHeaderHeight + tvguideConfig.channelGroupsHeight -2*textHeight)/2), *weekDay, theme.Color(clrFontHeader), colorFontBack, tvguideConfig.FontTimeLineWeekday);
        dateViewer->DrawText(cPoint((tvguideConfig.timeLineWidth-dateWidth)/2, (tvguideConfig.channelHeaderHeight + tvguideConfig.channelGroupsHeight -2*textHeight)/2 + textHeight + 5), *date, theme.Color(clrFontHeader), colorFontBack, tvguideConfig.FontTimeLineDate);
    } else if (tvguideConfig.displayMode == eHorizontal) {
        cString strDate = cString::sprintf("%s %s", *weekDay, *date);
        int x = (dateViewer->Width() - tvguideConfig.FontTimeLineDateHorizontal->Width(*strDate))/2;
        int y = (dateViewer->Height() - tvguideConfig.FontTimeLineDateHorizontal->Height())/2;
        dateViewer->DrawText(cPoint(x, y), *strDate, theme.Color(clrFontHeader), colorFontBack, tvguideConfig.FontTimeLineDateHorizontal);
    }
}

void cTimeLine::drawTimeline() {
    timelineBack->Fill(clrBlack);
    timeline->SetTile(true);
    timeline->Fill(theme.Color(clrBackground));
    tColor colorFont, colorBackground;
    
    int imgWidth, imgHeight;
    if (tvguideConfig.displayMode == eVertical) {
        imgWidth = tvguideConfig.timeLineWidth-4;
        imgHeight = tvguideConfig.minutePixel*30;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        imgWidth = tvguideConfig.minutePixel*30;
        imgHeight = tvguideConfig.timeLineHeight-4;
    }
    const cImage *img1 = createBackgroundImage(imgWidth, imgHeight, theme.Color(clrTimeline1), theme.Color(clrTimeline1Blending));
    const cImage *img2 = createBackgroundImage(imgWidth, imgHeight, theme.Color(clrTimeline2), theme.Color(clrTimeline2Blending));
    const cImage *img = NULL;
    
    int textWidth, posX, posY;
    char timetext[10];
    
    for (int i=0; i<48; i++) {
        if (i%2==0) {
            img = img1;
            colorFont = theme.Color(clrTimeline2);
            colorBackground = (tvguideConfig.useBlending==0)?theme.Color(clrTimeline1):clrTransparent;
            if (tvguideConfig.timeFormat == e12Hours) {
                if (i == 0)
                    sprintf(timetext, "12:00 PM");
                else if (i/2 < 13)
                    sprintf(timetext, "%d:00 AM", i/2);
                else 
                    sprintf(timetext, "%d:00 PM", i/2-12);
            } else {
                sprintf(timetext, "%d:00", i/2);
            }
        } else {
            img = img2;
            colorFont = theme.Color(clrTimeline1);
            colorBackground = (tvguideConfig.useBlending==0)?theme.Color(clrTimeline2):clrTransparent;
            if (tvguideConfig.timeFormat == e12Hours) {
                if (i == 1)
                    sprintf(timetext, "12:30 PM");
                else if (i/2 < 13)
                    sprintf(timetext, "%d:30 AM", i/2);
                else 
                    sprintf(timetext, "%d:30 PM", i/2-12);
            } else {
                sprintf(timetext, "%d:30", i/2);
            }
        }
        if (tvguideConfig.displayMode == eVertical) {
            posY = i*tvguideConfig.minutePixel*30;
            timeline->DrawImage(cPoint(2, posY), *img);
            decorateTile(0, posY, imgWidth+2, imgHeight);
            textWidth = tvguideConfig.FontTimeLineTime->Width(timetext);
            timeline->DrawText(cPoint((tvguideConfig.timeLineWidth-textWidth)/2, posY + 5), timetext, colorFont, colorBackground, tvguideConfig.FontTimeLineTime);
        } else if (tvguideConfig.displayMode == eHorizontal) {
            posX = i*tvguideConfig.minutePixel*30;
            timeline->DrawImage(cPoint(posX, 2), *img);
            decorateTile(posX, 0, imgWidth, imgHeight+2);
            timeline->DrawText(cPoint(posX + 15, (dateViewer->Height() - tvguideConfig.FontTimeLineTimeHorizontal->Height())/2), timetext, colorFont, colorBackground, tvguideConfig.FontTimeLineTimeHorizontal);
       }
    }
    setTimeline();
    delete img1;
    delete img2;
}

void cTimeLine::decorateTile(int posX, int posY, int tileWidth, int tileHeight) {
    timeline->DrawRectangle(cRect(posX,posY,tileWidth,2), theme.Color(clrBackground));          //top
    timeline->DrawRectangle(cRect(posX,posY,2,tileHeight), theme.Color(clrBackground));         //left
    timeline->DrawRectangle(cRect(posX,posY + tileHeight-2,tileWidth,2), theme.Color(clrBackground));   //bottom
    timeline->DrawRectangle(cRect(posX + tileWidth-2,posY,2,tileHeight), theme.Color(clrBackground));   //right

    timeline->DrawRectangle(cRect(2+posX,posY+2,tileWidth-4,1), theme.Color(clrBorder));            //top
    timeline->DrawRectangle(cRect(2+posX,posY+2,1,tileHeight-4), theme.Color(clrBorder));           //left
    timeline->DrawRectangle(cRect(2+posX,posY+tileHeight-3,tileWidth-4,1), theme.Color(clrBorder));     //bottom
    timeline->DrawRectangle(cRect(posX+tileWidth-3,posY+2,1,tileHeight-4), theme.Color(clrBorder));     //right
    
    if (tvguideConfig.roundedCorners) {
        int borderRadius = 12;
        drawRoundedCorners(posX, posY, tileWidth, tileHeight, borderRadius);
    }
}

void cTimeLine::drawRoundedCorners(int posX, int posY, int width, int height, int radius) {
    timeline->DrawEllipse(cRect(posX+2,posY+2,radius,radius), theme.Color(clrBorder), -2);
    timeline->DrawEllipse(cRect(posX+1,posY+1,radius,radius), theme.Color(clrBackground), -2);

    timeline->DrawEllipse(cRect(posX+width-radius - 2,posY+2,radius,radius), theme.Color(clrBorder), -1);
    timeline->DrawEllipse(cRect(posX+width-radius - 1,posY+1,radius,radius), theme.Color(clrBackground), -1);
    
    if( height > 2*radius) {
        timeline->DrawEllipse(cRect(posX+2,posY+height-radius - 2,radius,radius), theme.Color(clrBorder), -3);
        timeline->DrawEllipse(cRect(posX+1,posY+height-radius - 1,radius,radius), theme.Color(clrBackground), -3);
        
        timeline->DrawEllipse(cRect(posX+width-radius - 2,posY+height-radius - 2,radius,radius), theme.Color(clrBorder), -4);
        timeline->DrawEllipse(cRect(posX+width-radius - 1,posY+height-radius - 1,radius,radius), theme.Color(clrBackground), -4);
    }
}

cImage *cTimeLine::createBackgroundImage(int width, int height, tColor clrBgr, tColor clrBlend) {
    cImage *image = NULL;
    if (tvguideConfig.useBlending == 1) {
        image = new cImage(cSize(width, height));
        image->Fill(clrBgr);
        int stepY = 0.5*height / 64;
        int alpha = 0x00;
        tColor clr;
        for (int i = 0; i<64; i++) {
            clr = AlphaBlend(clrBgr, clrBlend, alpha);
            for (int y = i*stepY; y < (i+1)*stepY; y++) {
                for (int x=0; x<width; x++) {
                    image->SetPixel(cPoint(x,y), clr);
                }
            }
            alpha += 0x04;
        }
    } else  if (tvguideConfig.useBlending == 2) {
        cImageLoader imgLoader;
        if (imgLoader.DrawBackground(clrBgr, clrBlend, width, height)) {
            image = new cImage(imgLoader.GetImage());
        }
    } else {
        image = new cImage(cSize(width, height));
        image->Fill(clrBgr);
    }
    return image;
}

void cTimeLine::setTimeline() {
    int offset = myTime->GetTimelineOffset();
    int xNew, yNew;
    if (tvguideConfig.displayMode == eVertical) {
        xNew = 0;
        yNew = -offset*tvguideConfig.minutePixel;
    } else if (tvguideConfig.displayMode == eHorizontal) {
        xNew = -offset*tvguideConfig.minutePixel;
        yNew = 0;
    }
    timeline->SetDrawPortPoint(cPoint(xNew, yNew));
}

void cTimeLine::drawClock() {
    cString currentTime = myTime->GetCurrentTime();
    int textHeight = tvguideConfig.FontTimeLineTime->Height();
    int clockWidth = tvguideConfig.FontTimeLineTime->Width(*currentTime);
    tColor colorFontBack = (tvguideConfig.useBlending==0)?theme.Color(clrHeader):clrTransparent;
    clock->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
    clock->drawBackground();
    clock->drawBorder();
    clock->DrawText(cPoint((tvguideConfig.timeLineWidth-clockWidth)/2, (tvguideConfig.footerHeight-textHeight)/2), *currentTime, theme.Color(clrFontHeader), colorFontBack, tvguideConfig.FontTimeLineTime);
}