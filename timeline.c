#include "timeline.h"

cTimeLine::cTimeLine(cMyTime *myTime) {
	this->myTime = myTime;
	dateViewer = new cStyledPixmap(osdManager.requestPixmap(3, cRect(0, 
                                                                     tvguideConfig.statusHeaderHeight, 
                                                                     tvguideConfig.timeColWidth, 
                                                                     tvguideConfig.headerHeight)
                                                             , cRect::Null));
	timeline = osdManager.requestPixmap(2, cRect(0, 
                                                 tvguideConfig.statusHeaderHeight + tvguideConfig.headerHeight, 
                                                 tvguideConfig.timeColWidth, 
                                                 tvguideConfig.osdHeight - tvguideConfig.statusHeaderHeight - tvguideConfig.headerHeight - tvguideConfig.footerHeight)
                                         , cRect(0,
                                                 0, 
                                                 tvguideConfig.timeColWidth, 
                                                 1440*tvguideConfig.minuteHeight));
	clock = new cStyledPixmap(osdManager.requestPixmap(3, cRect(0, 
                                                                tvguideConfig.osdHeight- tvguideConfig.footerHeight, 
                                                                tvguideConfig.timeColWidth, 
                                                                tvguideConfig.footerHeight-9)
                                                        , cRect::Null));
}

cTimeLine::~cTimeLine(void) {
	delete dateViewer;
	osdManager.releasePixmap(timeline);
	delete clock;
}

void cTimeLine::drawDateViewer() {
	cString weekDay = myTime->GetWeekday();
	cString date = myTime->GetDate();	

	int textHeight = tvguideConfig.FontTimeLineWeekday->Height();
	int weekdayWidth = tvguideConfig.FontTimeLineWeekday->Width(*weekDay);
	int dateWidth = tvguideConfig.FontTimeLineDate->Width(*date);

	dateViewer->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
	dateViewer->drawBackground();
	dateViewer->drawBorder();
	dateViewer->DrawText(cPoint((tvguideConfig.timeColWidth-weekdayWidth)/2, (tvguideConfig.headerHeight-2*textHeight)/2), *weekDay, theme.Color(clrFontHeader), clrTransparent, tvguideConfig.FontTimeLineWeekday);
	dateViewer->DrawText(cPoint((tvguideConfig.timeColWidth-dateWidth)/2, (tvguideConfig.headerHeight-2*textHeight)/2 + textHeight + 5), *date, theme.Color(clrFontHeader), clrTransparent, tvguideConfig.FontTimeLineDate);
}

void cTimeLine::drawTimeline() {
	timeline->SetTile(true);
	timeline->Fill(theme.Color(clrBackground));
	tColor colorFont;
	
	const cImage *img1 = createBackgroundImage(tvguideConfig.timeColWidth-4, tvguideConfig.minuteHeight*30, theme.Color(clrTimeline1), theme.Color(clrTimeline1Blending));
	const cImage *img2 = createBackgroundImage(tvguideConfig.timeColWidth-4, tvguideConfig.minuteHeight*30, theme.Color(clrTimeline2), theme.Color(clrTimeline2Blending));
	const cImage *img = NULL;
	
	int textWidth, posY;
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
		posY = i*tvguideConfig.minuteHeight*30;
		timeline->DrawImage(cPoint(2, posY), *img);
		textWidth = tvguideConfig.FontTimeLineTime->Width(timetext);
		timeline->DrawText(cPoint((tvguideConfig.timeColWidth-textWidth)/2, posY + 5), timetext, colorFont, clrTransparent, tvguideConfig.FontTimeLineTime);
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
	timeline->SetDrawPortPoint(cPoint(0, -offset*tvguideConfig.minuteHeight));
}

void cTimeLine::drawClock() {
	cString currentTime = myTime->GetCurrentTime();
	int textHeight = tvguideConfig.FontTimeLineTime->Height();
	int clockWidth = tvguideConfig.FontTimeLineTime->Width(*currentTime);
	clock->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
	clock->drawBackground();
	clock->drawBorder();
	clock->DrawText(cPoint((tvguideConfig.timeColWidth-clockWidth)/2, (tvguideConfig.footerHeight-textHeight)/2), *currentTime, theme.Color(clrFontHeader), clrTransparent, tvguideConfig.FontTimeLineTime);
}