// --- Theme -------------------------------------------------------------
static cTheme theme;

//BLENDING SETUP
#define CLR_BLENDING_NOPACITY   0xFFFFFFFF
#define CLR_BLENDING_DEFAULT    0xAAAAAAAA
#define CLR_BLENDING_OFF        0x00000000

THEME_CLR(theme, clrDoBlending, CLR_BLENDING_DEFAULT);
THEME_CLR(theme, clrBackgroundOSD, clrBlack);
THEME_CLR(theme, clrBackground, clrBlack);
THEME_CLR(theme, clrGrid1, 0xFF404749);
THEME_CLR(theme, clrGrid1Blending, 0xFF000000);
THEME_CLR(theme, clrGrid2, 0xFF20293F);
THEME_CLR(theme, clrGrid2Blending, 0xFF000000);
THEME_CLR(theme, clrHighlight, 0xFFFF4D00);
THEME_CLR(theme, clrHighlightBlending, 0xFF000000);
THEME_CLR(theme, clrFont, clrWhite);
THEME_CLR(theme, clrFontActive, clrWhite);
THEME_CLR(theme, clrFontHeader, clrWhite);
THEME_CLR(theme, clrFontButtons, clrWhite);
THEME_CLR(theme, clrStatusHeader, clrBlack);
THEME_CLR(theme, clrStatusHeaderBlending, clrBlack);
THEME_CLR(theme, clrHeader, clrBlack);
THEME_CLR(theme, clrHeaderBlending, 0xFFE0E0E0);
THEME_CLR(theme, clrBorder, clrWhite);
THEME_CLR(theme, clrTimeline1, clrWhite);
THEME_CLR(theme, clrTimeline1Blending, 0xFF828282);
THEME_CLR(theme, clrTimeline2, clrBlack);
THEME_CLR(theme, clrTimeline2Blending, 0xFF3F3F3F);
THEME_CLR(theme, clrButtonRed, 0x99BB0000);
THEME_CLR(theme, clrButtonRedBorder, 0xFFBB0000);
THEME_CLR(theme, clrButtonGreen, 0x9900BB00);
THEME_CLR(theme, clrButtonGreenBorder, 0xFF00BB00);
THEME_CLR(theme, clrButtonYellow, 0x99BBBB00);
THEME_CLR(theme, clrButtonYellowBorder, 0xFFBBBB00);
THEME_CLR(theme, clrButtonBlue, 0x990000BB);
THEME_CLR(theme, clrButtonBlueBorder, 0xFF0000BB);
THEME_CLR(theme, clrButtonBlend, 0xDD000000);

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
#include "grid.c"
#include "headergrid.c"
#include "dummygrid.c"
#include "epggrid.c"
#include "statusheader.c"
#include "detailview.c"
#include "channelcolumn.c"
#include "footer.c"

#include "tvguideosd.h"
#include <stdlib.h>

cTvGuideOsd::cTvGuideOsd(void) {
    detailView = NULL;
    detailViewActive = false;
    activeGrid = NULL;
    timeLine = NULL;
}

cTvGuideOsd::~cTvGuideOsd() {
    delete myTime;
    columns.Clear();
    if (tvguideConfig.displayStatusHeader) {
        delete statusHeader;
    }
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
        tvguideConfig.SetBlending();
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
    if (tvguideConfig.displayStatusHeader) {
        statusHeader = new cStatusHeader();
        statusHeader->ScaleVideo();
    }
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

void cTvGuideOsd::readChannels(const cChannel *channelStart) {
    int i=0;
    columns.Clear();
    if (!channelStart)
        return;
    for (const cChannel *channel = channelStart; channel; channel = Channels.Next(channel)) {
      if (!channel->GroupSep()) {
        cChannelColumn *column = new cChannelColumn(i, channel, myTime);
        if (column->readGrids()) {
            columns.Add(column);
            i++;
        } else {
            delete column;
        }
      }
      if (i == tvguideConfig.numGrids)
        break;
    }
}

void cTvGuideOsd::drawGridsChannelJump() {
    if (columns.Count() == 0)
        return;
    activeGrid = columns.First()->getActive();
    if (activeGrid)
        activeGrid->SetActive();
    if (tvguideConfig.displayStatusHeader) {
        statusHeader->DrawInfoText(activeGrid);
    }
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
        if (tvguideConfig.displayStatusHeader) {
            statusHeader->DrawInfoText(activeGrid);
        }
    }
}

void cTvGuideOsd::setNextActiveGrid(cGrid *next) {
    if (!next || !activeGrid) {
        return;
    }
    activeGrid->SetInActive();
    activeGrid->Draw(); 
    activeGrid = next;
    activeGrid->SetActive();
    activeGrid->Draw();
    if (tvguideConfig.displayStatusHeader) {
        statusHeader->DrawInfoText(activeGrid);
    }
}

void cTvGuideOsd::channelForward() {
    cChannelColumn *colRight = columns.Next(activeGrid->column);
    if (!colRight) {
        const cChannel *channelRight = activeGrid->column->getChannel();
        while (channelRight = Channels.Next(channelRight)) {
            if (!channelRight->GroupSep()) {
                colRight = new cChannelColumn(tvguideConfig.numGrids - 1, channelRight, myTime);
                if (colRight->readGrids()) {
                    break;
                } else {
                    delete colRight;
                    colRight = NULL;
                }
            }
        }
        if (colRight) {
            if (columns.Count() == tvguideConfig.numGrids) {
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
        cGrid *right = colRight->getNeighbor(activeGrid);
        if (right) {
            setNextActiveGrid(right);
        }
    }
    osdManager.flush();
}

void cTvGuideOsd::channelBack() {
    cChannelColumn *colLeft = columns.Prev(activeGrid->column);
    if (!colLeft) {
        const cChannel *channelLeft = activeGrid->column->getChannel();
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
            if (columns.Count() == tvguideConfig.numGrids) {
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
        cGrid *left = colLeft->getNeighbor(activeGrid);
        if (left) {
            setNextActiveGrid(left);
        }
    }
    osdManager.flush();
}

void cTvGuideOsd::timeForward() {
    bool actionDone = false;
    if ( (myTime->GetEnd() - activeGrid->EndTime())/60 < 30 ) {
        ScrollForward();
        actionDone = true;
    }
    cGrid *next = activeGrid->column->getNext(activeGrid);
    if (next) {
        if (   (next->EndTime() < myTime->GetEnd())
            || ( (myTime->GetEnd() - next->StartTime())/60 > 30 ) ) {
            setNextActiveGrid(next);
            actionDone = true;
        }
    }
    if (!actionDone) {
        ScrollForward();
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

void cTvGuideOsd::timeBack() {
    bool actionDone = false;
    if ( (activeGrid->StartTime() - myTime->GetStart())/60 < 30 ) {
        ScrollBack();
        actionDone = true;
    }
    cGrid *prev = activeGrid->column->getPrev(activeGrid);
    if (prev) {
        if (   (prev->StartTime() > myTime->GetStart())
            || ( (prev->EndTime() - myTime->GetStart())/60 > 30 )
            || ( prev->isFirst()) ) {
            setNextActiveGrid(prev);
            actionDone = true;
        }
    }
    if (!actionDone) {
        ScrollBack();
    }
    osdManager.flush();
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

void cTvGuideOsd::processKeyUp() {
    if (!activeGrid) {
        return;
    }
    if (detailViewActive) {
        detailView->scrollUp();
        osdManager.flush();
    } else {
        if (tvguideConfig.displayMode == eVertical) {
            timeBack();
        } else if (tvguideConfig.displayMode == eHorizontal) {
            channelBack();
        }
    }
}

void cTvGuideOsd::processKeyDown() {
    if (!activeGrid) {
            return;
    }
    if (detailViewActive) {
        detailView->scrollDown();
        osdManager.flush();
    } else {
        if (tvguideConfig.displayMode == eVertical) {
            timeForward();
        } else if (tvguideConfig.displayMode == eHorizontal) {
            channelForward();
        }
    }
}

void cTvGuideOsd::processKeyLeft() {
    if (detailViewActive)
        return;
    if (activeGrid == NULL)
        return;
    if (tvguideConfig.displayMode == eVertical) {
        channelBack();
    } else if (tvguideConfig.displayMode == eHorizontal) {
        timeBack();
    }
}

void cTvGuideOsd::processKeyRight() {
    if (detailViewActive)
        return;
    if (activeGrid == NULL)
        return;
    if (tvguideConfig.displayMode == eVertical) {
        channelForward();
    } else if (tvguideConfig.displayMode == eHorizontal) {
        timeForward();
    }
}

void cTvGuideOsd::processKeyOk() {
    if (detailViewActive) {
        delete detailView;
        detailView = NULL;
        detailViewActive = false;
        osdManager.flush();
    } else {
        if (!activeGrid->isDummy()) {
            detailViewActive = true;
            detailView = new cDetailView(activeGrid);
            detailView->Start();
        }
    }
}

void cTvGuideOsd::processKeyRed() {
    if  ((activeGrid == NULL) || activeGrid->isDummy())
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
    const cChannel *currentChannel = activeGrid->column->getChannel();
    const cChannel *prev = NULL;
    int i = tvguideConfig.jumpChannels + 1;
    for (const cChannel *channel = currentChannel; channel; channel = Channels.Prev(channel)) {
        if (!channel->GroupSep()) {
            prev = channel;
            i--;
        }
        if (i == 0)
            break;
    }
    if (prev) {
        readChannels(prev);
        if (columns.Count() > 0) {
            drawGridsChannelJump();
        }
        osdManager.flush();
    }
}

void cTvGuideOsd::processKeyYellow() {
    if (activeGrid == NULL)
        return;
    const cChannel *currentChannel = activeGrid->column->getChannel();
    const cChannel *next = NULL;
    int i=0;
    for (const cChannel *channel = currentChannel; channel; channel = Channels.Next(channel)) {
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
    const cChannel *currentChannel = activeGrid->column->getChannel();
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
            case kUp:       processKeyUp(); break;
            case kDown:     processKeyDown(); break;
            case kLeft:     processKeyLeft(); break;
            case kRight:    processKeyRight(); break;
            case kRed:      processKeyRed(); break;
            case kGreen:    processKeyGreen(); break;
            case kYellow:   processKeyYellow(); break;
            case kBlue:     state = processKeyBlue(); break;
            case kOk:       processKeyOk(); break;
            case kBack:     state=osEnd; break;
            case k1:        processKey1(); break;
            case k3:        processKey3(); break;
            case k4:        processKey4(); break;
            case k6:        processKey6(); break;
            case k7:        processKey7(); break;
            case k9:        processKey9(); break;
            default:        break;
        }
        cPixmap::Unlock();
    }
    return state;
}

void cTvGuideOsd::dump() {
    esyslog("tvguide: ------Dumping Content---------");
    activeGrid->debug();
    int i=1;
    for (cChannelColumn *col = columns.First(); col; col = columns.Next(col)) {
        col->dumpGrids();
    }
}