// --- Theme -------------------------------------------------------------
static cTheme theme;

THEME_CLR(theme, clrBackgroundOSD, clrBlack);
THEME_CLR(theme, clrBackground, clrBlack);
THEME_CLR(theme, clrGrid1, 0xFF404749);
THEME_CLR(theme, clrGrid1Blending, 0xFF000000);
THEME_CLR(theme, clrGrid2, 0xFF20293F);
THEME_CLR(theme, clrGrid2Blending, 0xFF000000);
THEME_CLR(theme, clrHighlight, 0xFFFF4D00);
THEME_CLR(theme, clrHighlightBlending, 0xFF000000);
THEME_CLR(theme, clrFont, clrWhite);
THEME_CLR(theme, clrFontHeader, clrWhite);
THEME_CLR(theme, clrFontButtons, clrWhite);
THEME_CLR(theme, clrHeader, clrBlack);
THEME_CLR(theme, clrHeaderBlending, 0xFFE0E0E0);
THEME_CLR(theme, clrBorder, clrWhite);
THEME_CLR(theme, clrTimeline1, clrWhite);
THEME_CLR(theme, clrTimeline1Blending, 0xFF828282);
THEME_CLR(theme, clrTimeline2, clrBlack);
THEME_CLR(theme, clrTimeline2Blending, 0xFF3F3F3F);
THEME_CLR(theme, clrButtonRed, 0xFFD42627);
THEME_CLR(theme, clrButtonRedBlending, 0xFFE0E0E0);
THEME_CLR(theme, clrButtonGreen, 0xFF004F00);
THEME_CLR(theme, clrButtonGreenBlending, 0xFFE0E0E0);
THEME_CLR(theme, clrButtonYellow, 0xFFffa300);
THEME_CLR(theme, clrButtonYellowBlending, 0xFFE0E0E0);
THEME_CLR(theme, clrButtonBlue, 0xFF0000fe);
THEME_CLR(theme, clrButtonBlueBlending, 0xFFE0E0E0);

#include "config.c"
cTvguideConfig tvguideConfig;

#include "osdmanager.c"
cOsdManager osdManager;

#include "setup.c"
#include "imageloader.c"
#include "styledpixmap.c"
#include "timer.c"
#include "messagebox.c"
#include "timeline.c"
#include "epggrid.c"
#include "detailview.c"
#include "channelcolumn.c"
#include "footer.c"

#include "tvguideosd.h"
#include <stdlib.h>

cTvGuideOsd::cTvGuideOsd(void) {
	detailView = NULL;
	detailViewActive = false;
	timeLine = NULL;
}

cTvGuideOsd::~cTvGuideOsd() {
	delete myTime;
	columns.Clear();
	if (detailView)
		delete detailView;
	delete timeLine;
	delete footer;
	cMessageBox::Destroy();
	osdManager.deleteOsd();
}

void cTvGuideOsd::Show(void) {
	int start = cTimeMs::Now();
	bool ok = false;
	ok = osdManager.setOsd();
	if (ok) {
		tvguideConfig.setDynamicValues(osdManager.Width(), osdManager.Height());
		tvguideConfig.loadTheme();
		osdManager.setBackground();
		myTime = new cMyTime();
		myTime->Now();
		drawOsd();
	}
	esyslog("tvguide: Rendering took %d ms", int(cTimeMs::Now()-start));
}

void cTvGuideOsd::drawOsd() {
	cPixmap::Lock();
	cChannel *startChannel = Channels.GetByNumber(cDevice::CurrentChannel());
	timeLine = new cTimeLine(myTime);
	timeLine->drawDateViewer();
	timeLine->drawTimeline();
	timeLine->drawClock();
	footer = new cFooter();
	footer->drawRedButton();
	footer->drawGreenButton();
	footer->drawYellowButton();
	footer->drawBlueButton();
	osdManager.flush();
	readChannels(startChannel);
	drawGridsChannelJump();
	osdManager.flush();
	cPixmap::Unlock();
}

void cTvGuideOsd::readChannels(cChannel *channelStart) {
	int i=0;
	columns.Clear();
	if (!channelStart)
		return;
	for (cChannel *channel = channelStart; channel; channel = Channels.Next(channel)) {
      if (!channel->GroupSep()) {
		cChannelColumn *column = new cChannelColumn(i, channel, myTime);
		if (column->readGrids()) {
			columns.Add(column);
			i++;
		} else {
			delete column;
		}
	  }
	  if (i == tvguideConfig.channelCols)
	    break;
    }
}

bool cTvGuideOsd::readChannelsReverse(cChannel *channelStart) {
	bool doUpdate = false;
	int i = tvguideConfig.channelCols;
	if (!channelStart)
		return false;
	for (cChannel *channel = Channels.Prev(channelStart); channel; channel = Channels.Prev(channel)) {
      if (!channel->GroupSep()) {
		cChannelColumn *column = new cChannelColumn(i-1, channel, myTime);
		if (column->readGrids()) {
			if (i == tvguideConfig.channelCols) {
				columns.Clear();
				doUpdate = true;
			}
			columns.Ins(column, columns.First());
			i--;
		} else {
			delete column;
		}
	  }
	  if (i == 0)
	    break;
    }
	return doUpdate;
}

void cTvGuideOsd::drawGridsChannelJump() {
	if (columns.Count() == 0)
		return;
	activeGrid = columns.First()->getActive();
	if (activeGrid)
		activeGrid->SetActive();
	for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
		column->createHeader();
		column->drawGrids();
	}
}

void cTvGuideOsd::drawGridsTimeJump() {
	if (columns.Count() == 0)
		return;
	cChannelColumn *colActive = NULL;
	if (activeGrid) {
		colActive = activeGrid->column;
	} else {
		colActive = columns.First();
	}
	for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
		column->clearGrids();
		column->readGrids();
		column->drawGrids();
	}
	activeGrid = colActive->getActive();
	if (activeGrid) {
		activeGrid->SetActive();
		activeGrid->Draw();
	}
}

void cTvGuideOsd::setNextActiveGrid(cEpgGrid *next) {
	if (!next || !activeGrid) {
		return;
	}
	activeGrid->SetInActive();
	activeGrid->Draw();	
	activeGrid = next;
	activeGrid->SetActive();
	activeGrid->Draw();
}

void cTvGuideOsd::processKeyUp() {
	if (detailViewActive) {
		detailView->scrollUp();
	} else {
		if (activeGrid == NULL) {
			ScrollBack();
			//Search for new active Grid
			cEpgGrid *actGrid = NULL;
			for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
				actGrid = column->getActive();
				if (actGrid) {
					activeGrid = actGrid;
					activeGrid->SetActive();
					activeGrid->Draw();
					break;
				}
			}
		} else if (activeGrid->StartTime() <= myTime->GetStart()) {
			activeGrid->debug();
			ScrollBack();
		} else {
			cEpgGrid *prev = NULL;
			prev = activeGrid->column->getPrev(activeGrid);
			if (prev) {
				setNextActiveGrid(prev);
			} else {
				ScrollBack();
				prev = activeGrid->column->getPrev(activeGrid);
				if (prev) {
					setNextActiveGrid(prev);
				}
			}
		}
	}
	osdManager.flush();
}

void cTvGuideOsd::processKeyDown() {
	if (detailViewActive) {
		detailView->scrollDown();
	} else {
		if (activeGrid == NULL) {
			ScrollForward();
		} else if (activeGrid->EndTime() > myTime->GetStop()) {
			ScrollForward();
		} else {
			cEpgGrid *next = NULL;
			next = activeGrid->column->getNext(activeGrid);
			if (next) {
				setNextActiveGrid(next);
			} else {
				ScrollForward();
				next = activeGrid->column->getNext(activeGrid);
				if (next) {
					setNextActiveGrid(next);
				}
			}
		}
	}
	osdManager.flush();
}

void cTvGuideOsd::ScrollForward() {
	myTime->AddStep(tvguideConfig.stepMinutes);
	timeLine->drawDateViewer();
	timeLine->drawClock();
	timeLine->setTimeline();
	for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
		column->AddNewGridsAtEnd();
		column->ClearOutdatedStart();
		column->drawGrids();
	}
}

void cTvGuideOsd::ScrollBack() {
	bool tooFarInPast = myTime->DelStep(tvguideConfig.stepMinutes);
	if (tooFarInPast)
		return;
	timeLine->drawDateViewer();
	timeLine->drawClock();
	timeLine->setTimeline();
	for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
		column->AddNewGridsAtStart();
		column->ClearOutdatedEnd();
		column->drawGrids();
	}	
}

void cTvGuideOsd::processKeyLeft() {
	if (detailViewActive)
		return;
	if (activeGrid == NULL)
		return;
	cChannelColumn *colLeft = columns.Prev(activeGrid->column);
	if (!colLeft) {
		cChannel *channelLeft = activeGrid->column->getChannel();
		while (channelLeft = Channels.Prev(channelLeft)) {
			if (!channelLeft->GroupSep()) {
				colLeft = new cChannelColumn(0, channelLeft, myTime);
				if (colLeft->readGrids()) {
					break;
				} else {
					delete colLeft;
					colLeft = NULL;
				}
			}
		}
		if (colLeft) {
			if (columns.Count() == tvguideConfig.channelCols) {
				cChannelColumn *cLast = columns.Last();
				columns.Del(cLast);
			}
			for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
				column->SetNum(column->GetNum() + 1);
				column->drawHeader();
				column->drawGrids();
			}
			columns.Ins(colLeft, columns.First());
			colLeft->createHeader();
			colLeft->drawGrids();
		}
	}

	if (colLeft) {
		cEpgGrid *left = colLeft->getNeighbor(activeGrid);
		if (left) {
			setNextActiveGrid(left);
		}
	}
	osdManager.flush();
}

void cTvGuideOsd::processKeyRight() {
	if (detailViewActive)
		return;
	if (activeGrid == NULL)
		return;
	cChannelColumn *colRight = columns.Next(activeGrid->column);
	if (!colRight) {
		cChannel *channelRight = activeGrid->column->getChannel();
		while (channelRight = Channels.Next(channelRight)) {
			if (!channelRight->GroupSep()) {
				colRight = new cChannelColumn(tvguideConfig.channelCols - 1, channelRight, myTime);
				if (colRight->readGrids()) {
					break;
				} else {
					delete colRight;
					colRight = NULL;
				}
			}
		}
		if (colRight) {
			if (columns.Count() == tvguideConfig.channelCols) {
				cChannelColumn *cFirst = columns.First();
				columns.Del(cFirst);
			}
			for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
				column->SetNum(column->GetNum() - 1);
				column->drawHeader();
				column->drawGrids();
			}
			columns.Add(colRight);
			colRight->createHeader();
			colRight->drawGrids();
		}
	}
	if (colRight) {
		cEpgGrid *right = colRight->getNeighbor(activeGrid);
		if (right) {
			setNextActiveGrid(right);
		}
	}
	osdManager.flush();
}

void cTvGuideOsd::processKeyOk() {
	if (detailViewActive) {
		delete detailView;
		detailView = NULL;
		detailViewActive = false;
		osdManager.flush();
	} else {
		detailViewActive = true;
		detailView = new cDetailView(activeGrid);
		detailView->Start();
	}
}

void cTvGuideOsd::processKeyRed() {
	if (activeGrid == NULL)
		return;
    cTimer *timer = new cTimer(activeGrid->GetEvent());
    cTimer *t = Timers.GetTimer(timer);
	cString msg;
	if (t) {
        isyslog("timer %s already exists", *timer->ToDescr());
		delete timer;
		msg = cString::sprintf(tr("Timer not set! There is already a timer for this item."));
    } else {
        Timers.Add(timer);
        Timers.SetModified();
		msg = cString::sprintf("%s:\n%s (%s) %s - %s", tr("Timer set"), activeGrid->GetEvent()->Title(), timer->Channel()->Name(), *DayDateTime(timer->StartTime()), *TimeString(timer->StopTime()));
		timer->SetEvent(activeGrid->GetEvent());
		activeGrid->setTimer();
		activeGrid->column->setTimer();
		activeGrid->SetDirty();
		activeGrid->Draw();
		osdManager.flush();
        isyslog("timer %s added (active)", *timer->ToDescr());
    }
	cMessageBox::Start(4000, msg);
}

void cTvGuideOsd::processKeyGreen() {
	if (activeGrid == NULL)
		return;
	cChannel *currentChannel = activeGrid->column->getChannel();
	bool doUpdate = readChannelsReverse(currentChannel);
	if (doUpdate && (columns.Count() > 0)) {
		drawGridsChannelJump();
	}
	osdManager.flush();
}

void cTvGuideOsd::processKeyYellow() {
	if (activeGrid == NULL)
		return;
	cChannel *currentChannel = activeGrid->column->getChannel();
	cChannel *next = NULL;
	int i=0;
	for (cChannel *channel = currentChannel; channel; channel = Channels.Next(channel)) {
		if (!channel->GroupSep()) {
			next = channel;
			i++;
		}
		if (i == (tvguideConfig.jumpChannels+1))
			break;
    }
	if (next) {
		readChannels(next);
		if (columns.Count() > 0) {
			drawGridsChannelJump();
		}
		osdManager.flush();
	}
}

eOSState cTvGuideOsd::processKeyBlue() {
	if (activeGrid == NULL)
		return osContinue;
	cChannel *currentChannel = activeGrid->column->getChannel();
	if (currentChannel) {
		cDevice::PrimaryDevice()->SwitchChannel(currentChannel, true);
		return osEnd;
	}
	return osContinue;
}

void cTvGuideOsd::processKey1() {
	bool tooFarInPast = myTime->DelStep(tvguideConfig.bigStepHours*60);
	if (tooFarInPast)
		return;
	drawGridsTimeJump();
	timeLine->drawDateViewer();
	timeLine->drawClock();
	timeLine->setTimeline();
	osdManager.flush();
}

void cTvGuideOsd::processKey3() {
	myTime->AddStep(tvguideConfig.bigStepHours*60);
	drawGridsTimeJump();
	timeLine->drawDateViewer();
	timeLine->drawClock();
	timeLine->setTimeline();
	osdManager.flush();
}

void cTvGuideOsd::processKey4() {
	bool tooFarInPast = myTime->DelStep(tvguideConfig.hugeStepHours*60);
	if (tooFarInPast)
		return;
	drawGridsTimeJump();
	timeLine->drawDateViewer();
	timeLine->drawClock();
	timeLine->setTimeline();
	osdManager.flush();
}

void cTvGuideOsd::processKey6() {
	myTime->AddStep(tvguideConfig.hugeStepHours*60);
	drawGridsTimeJump();
	timeLine->drawDateViewer();
	timeLine->drawClock();
	timeLine->setTimeline();
	osdManager.flush();
}

void cTvGuideOsd::processKey7() {
	cMyTime *primeChecker = new cMyTime();
	primeChecker->Now();
	time_t prevPrime = primeChecker->getPrevPrimetime(myTime->GetStart());
	if (primeChecker->tooFarInPast(prevPrime))
		return;
	myTime->SetTime(prevPrime);
	drawGridsTimeJump();
	timeLine->drawDateViewer();
	timeLine->drawClock();
	timeLine->setTimeline();
	osdManager.flush();
}

void cTvGuideOsd::processKey9() {
	cMyTime *primeChecker = new cMyTime();
	time_t nextPrime = primeChecker->getNextPrimetime(myTime->GetStart());
	myTime->SetTime(nextPrime);
	drawGridsTimeJump();
	timeLine->drawDateViewer();
	timeLine->drawClock();
	timeLine->setTimeline();
	osdManager.flush();
}

eOSState cTvGuideOsd::ProcessKey(eKeys Key) {
	eOSState state = cOsdObject::ProcessKey(Key);
	if (state == osUnknown) {
		cPixmap::Lock();
		state = osContinue;
		switch (Key & ~k_Repeat) {
			case kUp:     	processKeyUp(); break;
			case kDown:   	processKeyDown(); break;
			case kLeft:   	processKeyLeft(); break;
			case kRight:  	processKeyRight(); break;
			case kRed:    	processKeyRed(); break;
			case kGreen:  	processKeyGreen(); break;
			case kYellow: 	processKeyYellow(); break;
			case kBlue:   	state = processKeyBlue(); break;
			case kOk:     	processKeyOk(); break;
			case kBack:	 	state=osEnd; break;
			case k1:		processKey1(); break;
			case k3:		processKey3(); break;
			case k4:		processKey4(); break;
			case k6:		processKey6(); break;
			case k7:		processKey7(); break;
			case k9:		processKey9(); break;
			default: 	 	break;
		}
		cPixmap::Unlock();
	}
	return state;
}