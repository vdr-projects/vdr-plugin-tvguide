#include "timeline.h"

cTimeLine::cTimeLine(cMyTime *myTime) {
	this->myTime = myTime;
    if (tvguideConfig.displayMode == eVertical) {
        dateViewer = new cStyledPixmap(osdManager.requestPixmap(3, cRect(0, 
                                                                         tvguideConfig.statusHeaderHeight, 
                                                                         tvguideConfig.timeLineWidth, 
                                                                         tvguideConfig.channelHeaderHeight)));
        timeline = osdManager.requestPixmap(2, cRect(0, 
                                                     tvguideConfig.statusHeaderHeight + tvguideConfig.channelHeaderHeight, 
                                                     tvguideConfig.timeLineWidth, 
                                                     tvguideConfig.osdHeight - tvguideConfig.statusHeaderHeight - tvguideConfig.channelHeaderHeight - tvguideConfig.footerHeight)
                                             , cRect(0,
                                                     0, 
                                                     tvguideConfig.timeLineWidth, 
                                                     1440*tvguideConfig.minutePixel));
    } else if (tvguideConfig.displayMode == eHorizontal) {
        dateViewer = new cStyledPixmap(osdManager.requestPixmap(3, cRect(0, 
                                                                         tvguideConfig.statusHeaderHeight, 
                                                                         tvguideConfig.channelHeaderWidth,
                                                                         tvguideConfig.timeLineHeight)));
        timeline = osdManager.requestPixmap(2, cRect(tvguideConfig.channelHeaderWidth, 
                                                     tvguideConfig.statusHeaderHeight, 
                                                     tvguideConfig.osdWidth - tvguideConfig.channelHeaderWidth,
                                                     tvguideConfig.timeLineHeight)
                                             , cRect(0,
                                                     0, 
                                                     1440*tvguideConfig.minutePixel, 
                                                     tvguideConfig.timeLineWidth));
    }
    clock = new cStyledPixmap(osdManager.requestPixmap(3, cRect(0, 
                                                                tvguideConfig.osdHeight- tvguideConfig.footerHeight, 
                                                                tvguideConfig.timeLineWidth, 
                                                                tvguideConfig.footerHeight-9)));
}

cTimeLine::~cTimeLine(void) {
	delete dateViewer;
	osdManager.releasePixmap(timeline);
    if (clock)
        delete clock;
}

void cTimeLine::drawDateViewer() {
	cString weekDay = myTime->GetWeekday();
	cString date = myTime->GetDate();	
	dateViewer->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
	dateViewer->drawBackground();
	dateViewer->drawBorder();

    if (tvguideConfig.displayMode == eVertical) {
        int textHeight = tvguideConfig.FontTimeLineWeekday->Height();
        int weekdayWidth = tvguideConfig.FontTimeLineWeekday->Width(*weekDay);
        int dateWidth = tvguideConfig.FontTimeLineDate->Width(*date);
        dateViewer->DrawText(cPoint((tvguideConfig.timeLineWidth-weekdayWidth)/2, (tvguideConfig.channelHeaderHeight-2*textHeight)/2), *weekDay, theme.Color(clrFontHeader), clrTransparent, tvguideConfig.FontTimeLineWeekday);
        dateViewer->DrawText(cPoint((tvguideConfig.timeLineWidth-dateWidth)/2, (tvguideConfig.channelHeaderHeight-2*textHeight)/2 + textHeight + 5), *date, theme.Color(clrFontHeader), clrTransparent, tvguideConfig.FontTimeLineDate);
    } else if (tvguideConfig.displayMode == eHorizontal) {
        cString strDate = cString::sprintf("%s %s", *weekDay, *date);
        int x = (dateViewer->Width() - tvguideConfig.FontTimeLineDateHorizontal->Width(*strDate))/2;
        int y = (dateViewer->Height() - tvguideConfig.FontTimeLineDateHorizontal->Height())/2;
        dateViewer->DrawText(cPoint(x, y), *strDate, theme.Color(clrFontHeader), clrTransparent, tvguideConfig.FontTimeLineDateHorizontal);
    }
}

void cTimeLine::drawTimeline() {
	timeline->SetTile(true);
	timeline->Fill(theme.Color(clrBackground));
    tColor colorFont;
	
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
            textWidth = tvguideConfig.FontTimeLineTime->Width(timetext);
            timeline->DrawText(cPoint((tvguideConfig.timeLineWidth-textWidth)/2, posY + 5), timetext, colorFont, clrTransparent, tvguideConfig.FontTimeLineTime);
        } else if (tvguideConfig.displayMode == eHorizontal) {
            posX = i*tvguideConfig.minutePixel*30;
            timeline->DrawImage(cPoint(posX, 2), *img);
            timeline->DrawText(cPoint(posX + 2, (dateViewer->Height() - tvguideConfig.FontTimeLineTimeHorizontal->Height())/2), timetext, colorFont, clrTransparent, tvguideConfig.FontTimeLineTimeHorizontal);
       }
	}
	setTimeline();
	delete img1;
	delete img2;
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
	clock->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
	clock->drawBackground();
	clock->drawBorder();
	clock->DrawText(cPoint((tvguideConfig.timeLineWidth-clockWidth)/2, (tvguideConfig.footerHeight-textHeight)/2), *currentTime, theme.Color(clrFontHeader), clrTransparent, tvguideConfig.FontTimeLineTime);
}