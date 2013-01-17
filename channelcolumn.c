#include "channelcolumn.h"

cChannelColumn::cChannelColumn(int num, cChannel *channel, cMyTime *myTime) {
	this->channel = channel;
	this->num = num;
	this->myTime = myTime;
	hasTimer = channel->HasTimer();
}

cChannelColumn::~cChannelColumn(void) {
    osdManager.releasePixmap(pixmapLogo, cString::sprintf("channelcolumn logo %s", channel->Name()));
	grids.Clear();
}

void cChannelColumn::clearGrids() {
	grids.Clear();
}

void cChannelColumn::createHeader() {
    color = theme.Color(clrHeader);
    colorBlending = theme.Color(clrHeaderBlending);
    caller = cString::sprintf("channelcolumn %s", channel->Name());
    pixmap = osdManager.requestPixmap(2, cRect(tvguideConfig.timeColWidth + num*tvguideConfig.colWidth, 0, tvguideConfig.colWidth, tvguideConfig.headerHeight),
                                         cRect::Null, *caller);
	if (!pixmap) {
		return;
	}
    pixmapLogo = osdManager.requestPixmap(3, cRect(tvguideConfig.timeColWidth + num*tvguideConfig.colWidth, 0, tvguideConfig.colWidth, tvguideConfig.headerHeight),
                                             cRect::Null, *caller);
    if (!pixmapLogo) {
		return;
	}
    pixmapLogo->Fill(clrTransparent);

	drawBackground();
	cTextWrapper tw;
	cString headerText = cString::sprintf("%d - %s", channel->Number(), channel->Name());
	tw.Set(*headerText, tvguideConfig.FontHeader, tvguideConfig.colWidth - 8);
	int lines = tw.Lines();
	int lineHeight = tvguideConfig.FontHeader->Height();
	int yStart = (tvguideConfig.headerHeight - lines*lineHeight)/2 + 8;

	if (!tvguideConfig.hideChannelLogos) {
		cImageLoader imgLoader;
		if (imgLoader.LoadLogo(channel->Name())) {
				cImage logo = imgLoader.GetImage();
				int logoX = (tvguideConfig.colWidth - tvguideConfig.logoWidth)/2;
				pixmapLogo->DrawImage(cPoint(logoX, 5), logo);
		}
		yStart = tvguideConfig.logoHeight + 8;
	}
	for (int i=0; i<lines; i++) {
		int textWidth = tvguideConfig.FontHeader->Width(tw.GetLine(i));
		int xText = (tvguideConfig.colWidth - textWidth) / 2;
		if (xText < 0) 
			xText = 0;
		pixmap->DrawText(cPoint(xText, yStart + i*lineHeight), tw.GetLine(i), theme.Color(clrFontHeader), clrTransparent, tvguideConfig.FontHeader);
	}
	drawBorder();
}

void cChannelColumn::drawHeader() {
	pixmap->SetViewPort(cRect(tvguideConfig.timeColWidth + num*tvguideConfig.colWidth, 0, tvguideConfig.colWidth, tvguideConfig.headerHeight));
	pixmapLogo->SetViewPort(cRect(tvguideConfig.timeColWidth + num*tvguideConfig.colWidth, 0, tvguideConfig.colWidth, tvguideConfig.headerHeight));
}

bool cChannelColumn::readGrids() {
	schedules = cSchedules::Schedules(schedulesLock);
	const cSchedule *Schedule = NULL;
	Schedule = schedules->GetSchedule(channel);
	if (!Schedule) {
		return false;
	}
	bool eventFound = false;
	const cEvent *event = Schedule->GetEventAround(myTime->GetStart());
	if (event != NULL) {
		eventFound = true;
	} else {
		for (int i=1; i<6; i++) {
			event = Schedule->GetEventAround(myTime->GetStart()+i*5*60);
			if (event) {
				eventFound = true;
				break;
			}
		}
	} 
	if (eventFound) {
		bool col = true;
		for (; event; event = Schedule->Events()->Next(event)) {
			cEpgGrid *grid = new cEpgGrid(this, event);
			grid->setText();
			grid->SetColor(col);
			col = !col;
			grids.Add(grid);
			if (event->EndTime() > myTime->GetStop()) {
				break;
			}
		}
		return true;
	} else {
		return false;
	}
	
}

void cChannelColumn::drawGrids() {
	for (cEpgGrid *grid = grids.First(); grid; grid = grids.Next(grid)) {
		grid->SetViewportHeight();
		grid->PositionPixmap();
		grid->Draw();
	}
}

int cChannelColumn::getX() {
	return tvguideConfig.timeColWidth + num*tvguideConfig.colWidth;
}

cEpgGrid * cChannelColumn::getActive() {
	cMyTime t;
	t.Now();
	for (cEpgGrid *grid = grids.First(); grid; grid = grids.Next(grid)) {
		if (grid->isActiveInitial(t.Get()))
			return grid;
	}
	return grids.First();
}

cEpgGrid * cChannelColumn::getNext(cEpgGrid *activeGrid) {
	if (activeGrid == NULL)
		return NULL;
	cEpgGrid *next = grids.Next(activeGrid);
	if (next)
		return next;
	return NULL;
}

cEpgGrid * cChannelColumn::getPrev(cEpgGrid *activeGrid) {
	if (activeGrid == NULL)
		return NULL;
	cEpgGrid *prev = grids.Prev(activeGrid);
	if (prev)
		return prev;
	return NULL;
}

cEpgGrid * cChannelColumn::getNeighbor(cEpgGrid *activeGrid) {
	if (!activeGrid)
		return NULL;
	cEpgGrid *neighbor = NULL;
	int overlap = 0;
	int overlapNew = 0;
	cEpgGrid *grid = NULL;
	grid = grids.First();
	if (grid) {
		for (; grid; grid = grids.Next(grid)) {
			if ( (grid->StartTime() == activeGrid->StartTime()) ) {
				neighbor = grid;
				break;
			}
			overlapNew = activeGrid->calcOverlap(grid);
			if (overlapNew > overlap) {
				neighbor = grid;
				overlap = overlapNew;
			}
		}
	}
	if (!neighbor)
		neighbor = grids.First();
	return neighbor;
}

void cChannelColumn::AddNewGridsAtStart() {
	cEpgGrid *firstGrid = NULL;
	firstGrid = grids.First();
	if (firstGrid == NULL) {
		//no epg, completely new.
		schedules = cSchedules::Schedules(schedulesLock);
		const cSchedule *Schedule = NULL;
		Schedule = schedules->GetSchedule(channel);
		if (!Schedule)
			return;
		const cEvent *event = Schedule->GetEventAround(myTime->GetStart());
		if (!event)
			return;
		cEpgGrid *grid = new cEpgGrid(this, event);
		grid->setText();
		grid->SetColor(true);
		grids.Ins(grid, grids.First());
		return;
	} else {
		//if first event is long enough, nothing to do.
		if (firstGrid->StartTime() <= myTime->GetStart()) {
			return;
		}
		//if not, i have to add new ones to the list
		schedules = cSchedules::Schedules(schedulesLock);
		const cSchedule *Schedule = NULL;
		Schedule = schedules->GetSchedule(channel);
		if (!Schedule)
			return;
		bool col = !(firstGrid->IsColor1());
		for (const cEvent *event = Schedule->GetEventAround(firstGrid->StartTime()-60); event; event = Schedule->Events()->Prev(event)) {
			if (!event)
				return;
			cEpgGrid *grid = new cEpgGrid(this, event);
			grid->setText();
			grid->SetColor(col);
			col = !col;
			grids.Ins(grid, firstGrid);
			firstGrid = grid;
			if (event->StartTime() <= myTime->GetStart()) {
				break;
			}
		}
	}
}

void cChannelColumn::AddNewGridsAtEnd() {
	cEpgGrid *lastGrid = NULL;
	lastGrid = grids.Last();
	if (lastGrid == NULL)
		return;
	//if last event is long enough, nothing to do.
	if (lastGrid->EndTime() > myTime->GetStop()) {
		return;
	}
	//if not, i have to add new ones to the list
	schedules = cSchedules::Schedules(schedulesLock);
	const cSchedule *Schedule = NULL;
	Schedule = schedules->GetSchedule(channel);
	if (!Schedule)
		return;
	bool col = !(lastGrid->IsColor1());
	for (const cEvent *event = Schedule->GetEventAround(lastGrid->EndTime()+60); event; event = Schedule->Events()->Next(event)) {
		if (!event)
			return;
		cEpgGrid *grid = new cEpgGrid(this, event);
		grid->setText();
		grid->SetColor(col);
		col = !col;
		grids.Add(grid);
		if (event->EndTime() > myTime->GetStop()) {
			break;
		}
	}
}

void cChannelColumn::ClearOutdatedStart() {
	bool goOn = true;
	cEpgGrid *firstGrid = NULL;
	while (goOn) {
		firstGrid = grids.First();	
		if ((firstGrid != NULL)&&(firstGrid->EndTime() < myTime->GetStart())) {
			grids.Del(firstGrid);
			firstGrid = NULL;
		} else {
			goOn = false;
		}
	}
}

void cChannelColumn::ClearOutdatedEnd() {
	bool goOn = true;
	cEpgGrid *lastGrid = NULL;
	while (goOn) {
		lastGrid = grids.Last();	
		if ((lastGrid != NULL)&&(lastGrid->StartTime() > myTime->GetStop())) {
			grids.Del(lastGrid);
			lastGrid = NULL;
		} else {
			goOn = false;
		}
	}
}

void cChannelColumn::dumpGrids() {
	esyslog("------Channel %s ---------", channel->Name());
	
	for (cEpgGrid *grid = grids.First(); grid; grid = grids.Next(grid)) {
		grid->debug();
	}
	
}
