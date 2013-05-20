#include "dummygrid.h"

cDummyGrid::cDummyGrid(cChannelColumn *c, time_t start, time_t end) : cGrid(c) {
    this->start = start;
    this->end = end;
    strText = tr("No EPG Information available");
    dummy = true;
}

cDummyGrid::~cDummyGrid(void) {
}

void cDummyGrid::SetViewportHeight() {
	int viewportHeightOld = viewportHeight;
    viewportHeight = Duration() / 60 * tvguideConfig.minuteHeight;
    if (viewportHeight != viewportHeightOld)
		dirty = true;
}

void cDummyGrid::PositionPixmap() {
	int x0 = column->getX();
	int y0 = tvguideConfig.statusHeaderHeight + tvguideConfig.headerHeight;
	if ( column->Start() < StartTime() ) {
		y0 += (StartTime() - column->Start())/60*tvguideConfig.minuteHeight;
	}
	if (!pixmap) {
		pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, tvguideConfig.colWidth, viewportHeight), cRect::Null);
	} else if (dirty) {
        osdManager.releasePixmap(pixmap);
		pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, tvguideConfig.colWidth, viewportHeight), cRect::Null);
    } else {
		pixmap->SetViewPort(cRect(x0, y0, tvguideConfig.colWidth, viewportHeight));
	}
}

void cDummyGrid::setText() {
	text->Set(*(strText), tvguideConfig.FontGrid, tvguideConfig.colWidth-2*borderWidth);
}

void cDummyGrid::drawText() {
	if (Height()/tvguideConfig.minuteHeight < 6)
		return;
    int textHeight = tvguideConfig.FontGrid->Height();
	int textLines = text->Lines();
    for (int i=0; i<textLines; i++) {
		pixmap->DrawText(cPoint(borderWidth, borderWidth + i*textHeight), text->GetLine(i), theme.Color(clrFont), clrTransparent, tvguideConfig.FontGrid);
	}
}
cString cDummyGrid::getText(void) {
    return strText;
}

cString cDummyGrid::getTimeString(void) {
    return cString::sprintf("%s - %s", *TimeString(start), *TimeString(end));
}

void cDummyGrid::debug() {
	esyslog("tvguide dummygrid: %s: %s, %s, viewportHeight: %d px, Duration: %ld min, active: %d", 
                column->Name(),
                *cMyTime::printTime(start), 
                *cMyTime::printTime(end), 
                viewportHeight, 
                Duration()/60,
                active);
}