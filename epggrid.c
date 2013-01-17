#include "channelcolumn.h"
#include "epggrid.h"

cEpgGrid::cEpgGrid(cChannelColumn *c, const cEvent *event) {
	this->event = event;
	this->column = c;
	text = new cTextWrapper();
	extText = new cTextWrapper();
	dirty = true;
	active = false;
	viewportHeight = 0;
	borderWidth = 10;
	hasTimer = false;
	if (column->hasTimer)
		hasTimer = event->HasTimer();		
}

cEpgGrid::~cEpgGrid(void) {
	delete text;
	delete extText;
}

void cEpgGrid::SetViewportHeight() {
	int viewportHeightOld = viewportHeight;
	if ( column->myTime->GetStart() > event->StartTime() ) {
		viewportHeight = (min(event->EndTime(), column->myTime->GetStop()) - column->myTime->GetStart()) /60;
	} else if ( column->myTime->GetStop() < event->EndTime() ) {
		viewportHeight = (column->myTime->GetStop() - event->StartTime()) /60;
	} else {
		viewportHeight = event->Duration() / 60;
	}
	if (viewportHeight != viewportHeightOld)
		dirty = true;
}

void cEpgGrid::PositionPixmap() {
	int x0 = column->getX();
	int y0 = tvguideConfig.headerHeight;
	if ( column->myTime->GetStart() < event->StartTime() ) {
		y0 += (event->StartTime() - column->myTime->GetStart())/60*tvguideConfig.minuteHeight;
	}

	if (!pixmap) {
		caller = cString::sprintf("epggrid %s %s", column->channel->Name(), event->Title());
		pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, tvguideConfig.colWidth, viewportHeight * tvguideConfig.minuteHeight), 
											cRect(0, 0, tvguideConfig.colWidth, event->Duration()/60*tvguideConfig.minuteHeight), *caller);
	} else {
		pixmap->SetViewPort(cRect(x0, y0, tvguideConfig.colWidth, viewportHeight * tvguideConfig.minuteHeight));
	}
}

void cEpgGrid::setBackground() {
	if (active) {
		color = theme.Color(clrHighlight);
		colorBlending = theme.Color(clrHighlightBlending);
	} else {
		if (isColor1) {
			color = theme.Color(clrGrid1);
			colorBlending = theme.Color(clrGrid1Blending);
		} else {
			color = theme.Color(clrGrid2);
			colorBlending = theme.Color(clrGrid2Blending);
		}
	}
}

void cEpgGrid::Draw() {
	if (!pixmap) {
		return;
	}
	if (dirty) {
		setBackground();
		drawBackground();
		drawText();
		if (hasTimer) 
			DrawRecIcon();
		drawBorder();
		pixmap->SetLayer(1);
		dirty = false;
	}
}

void cEpgGrid::DrawRecIcon() {
	cString recIconText("REC");
	int width = tvguideConfig.FontGrid->Width(*recIconText)+2*borderWidth;
	int height = tvguideConfig.FontGrid->Height()+10;
	pixmap->DrawRectangle( cRect(pixmap->ViewPort().Width() - width - borderWidth,  pixmap->ViewPort().Height() - height - borderWidth, width, height), theme.Color(clrButtonRed));
	pixmap->DrawText(cPoint(pixmap->ViewPort().Width() - width, pixmap->ViewPort().Height() - height - borderWidth/2), *recIconText, theme.Color(clrFont), clrTransparent, tvguideConfig.FontGrid);	
}

void cEpgGrid::setText() {
	cString strText;
	strText = cString::sprintf("%s - %s:\n%s", *(event->GetTimeString()), *(event->GetEndTimeString()), event->Title());
	text->Set(*(strText), tvguideConfig.FontGrid, tvguideConfig.colWidth-2*borderWidth);
	extText->Set(event->ShortText(), tvguideConfig.FontGridSmall, tvguideConfig.colWidth-2*borderWidth);
}

void cEpgGrid::drawText() {
	int gridHeight = pixmap->ViewPort().Height();
	if (gridHeight/tvguideConfig.minuteHeight < 6)
		return;
	int textHeight = tvguideConfig.FontGrid->Height();
	int textLines = text->Lines();
	for (int i=0; i<textLines; i++) {
		pixmap->DrawText(cPoint(borderWidth, borderWidth + i*textHeight), text->GetLine(i), theme.Color(clrFont), clrTransparent, tvguideConfig.FontGrid);
	}
	int extTextLines = extText->Lines();
	int offset = (textLines+1)*textHeight - 0.5*textHeight;
	textHeight = tvguideConfig.FontGridSmall->Height();
	if ((pixmap->ViewPort().Height()-textHeight-10) > offset) {
		for (int i=0; i<extTextLines; i++) {
			pixmap->DrawText(cPoint(borderWidth, borderWidth + offset + i*textHeight), extText->GetLine(i), theme.Color(clrFont), clrTransparent, tvguideConfig.FontGridSmall);
		}
	}
}

int cEpgGrid::calcOverlap(cEpgGrid *neighbor) {
	int overlap = 0;
	if (intersects(neighbor)) {
		if ((event->StartTime() <= neighbor->StartTime()) && (event->EndTime() <= neighbor->EndTime())) {
			overlap = event->EndTime() - neighbor->StartTime();
		} else if ((event->StartTime() >= neighbor->StartTime()) && (event->EndTime() >= neighbor->EndTime())) {
			overlap = neighbor->EndTime() - event->StartTime();
		} else if ((event->StartTime() >= neighbor->StartTime()) && (event->EndTime() <= neighbor->EndTime())) {
			overlap = event->Duration();
		} else if ((event->StartTime() <= neighbor->StartTime()) && (event->EndTime() >= neighbor->EndTime())) {
			overlap = neighbor->EndTime() - neighbor->StartTime();
		}
	}
	return overlap;
}

bool cEpgGrid::intersects(cEpgGrid *neighbor) {
	return ! ( (neighbor->EndTime() <= event->StartTime()) || (neighbor->StartTime() >= event->EndTime()) ); 
}

bool cEpgGrid::isActiveInitial(time_t t) {
	if ((event->StartTime() < t) && (event->EndTime() > t))
		return true;
	else
		return false;
}

void cEpgGrid::debug() {
	esyslog("tvguide Grid: %s, %s, viewportHeight: %d, Duration: %d", *(event->GetTimeString()), event->Title(), viewportHeight, event->Duration()/60);
}