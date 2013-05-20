#include "channelcolumn.h"
#include "epggrid.h"

cEpgGrid::cEpgGrid(cChannelColumn *c, const cEvent *event)  : cGrid(c) {
	this->event = event;
	extText = new cTextWrapper();
	hasTimer = false;
	if (column->HasTimer())
		hasTimer = event->HasTimer();
    dummy = false;
}

cEpgGrid::~cEpgGrid(void) {
	delete extText;
}

void cEpgGrid::SetViewportHeight() {
	int viewportHeightOld = viewportHeight;
	if ( column->Start() > StartTime() ) {
		viewportHeight = (min((int)EndTime(), column->Stop()) - column->Start()) /60;
	} else if ( column->Stop() < EndTime() ) {
		viewportHeight = (column->Stop() - StartTime()) /60;
        if (viewportHeight < 0) viewportHeight = 0;
	} else {
		viewportHeight = Duration() / 60;
	}
    viewportHeight *= tvguideConfig.minuteHeight;
	if (viewportHeight != viewportHeightOld)
		dirty = true;
}

void cEpgGrid::PositionPixmap() {
	int x0 = column->getX();
	int y0 = tvguideConfig.statusHeaderHeight + tvguideConfig.headerHeight;
	if ( column->Start() < StartTime() ) {
		y0 += (StartTime() - column->Start())/60*tvguideConfig.minuteHeight;
	}
	if (!pixmap) {
		pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, tvguideConfig.colWidth, viewportHeight), 
											cRect(0, 0, tvguideConfig.colWidth, Duration()/60*tvguideConfig.minuteHeight));
	} else {
		pixmap->SetViewPort(cRect(x0, y0, tvguideConfig.colWidth, viewportHeight));
	}
}

void cEpgGrid::setText() {
	cString strText;
	strText = cString::sprintf("%s - %s:\n%s", *(event->GetTimeString()), *(event->GetEndTimeString()), event->Title());
	text->Set(*(strText), tvguideConfig.FontGrid, tvguideConfig.colWidth-2*borderWidth);
	extText->Set(event->ShortText(), tvguideConfig.FontGridSmall, tvguideConfig.colWidth-2*borderWidth);
}

void cEpgGrid::drawText() {
	if (Height()/tvguideConfig.minuteHeight < 6)
		return;
	int textHeight = tvguideConfig.FontGrid->Height();
	int textLines = text->Lines();
	for (int i=0; i<textLines; i++) {
		pixmap->DrawText(cPoint(borderWidth, borderWidth + i*textHeight), text->GetLine(i), theme.Color(clrFont), clrTransparent, tvguideConfig.FontGrid);
	}
	int extTextLines = extText->Lines();
	int offset = (textLines+1)*textHeight - 0.5*textHeight;
	textHeight = tvguideConfig.FontGridSmall->Height();
	if ((Height()-textHeight-10) > offset) {
		for (int i=0; i<extTextLines; i++) {
			pixmap->DrawText(cPoint(borderWidth, borderWidth + offset + i*textHeight), extText->GetLine(i), theme.Color(clrFont), clrTransparent, tvguideConfig.FontGridSmall);
		}
	}
    if (hasTimer) 
			drawRecIcon();
}

void cEpgGrid::drawRecIcon() {
	cString recIconText("REC");
	int width = tvguideConfig.FontGrid->Width(*recIconText)+2*borderWidth;
	int height = tvguideConfig.FontGrid->Height()+10;
	pixmap->DrawRectangle( cRect(Width() - width - borderWidth,  Height() - height - borderWidth, width, height), theme.Color(clrButtonRed));
	pixmap->DrawText(cPoint(Width() - width, Height() - height - borderWidth/2), *recIconText, theme.Color(clrFont), clrTransparent, tvguideConfig.FontGrid);	
}

cString cEpgGrid::getTimeString(void) {
    return cString::sprintf("%s - %s", *(event->GetTimeString()), *(event->GetEndTimeString()));
}

void cEpgGrid::debug() {
	esyslog("tvguide epggrid: %s: %s, %s, viewportHeight: %d px, Duration: %d min, active: %d", 
                column->Name(),
                *(event->GetTimeString()), 
                event->Title(), 
                viewportHeight, 
                event->Duration()/60,
                active);
}