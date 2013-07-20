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
THEME_CLR(theme, clrRecMenuBackground, 0xB0000000);
THEME_CLR(theme, clrRecMenuTimerConflictBackground, 0xFFCCCCCC);
THEME_CLR(theme, clrRecMenuTimerConflictBar, 0xFF222222);
THEME_CLR(theme, clrRecMenuTimerConflictOverlap, 0xAAFF0000);
THEME_CLR(theme, clrRecMenuDayActive, 0xFF00FF00);
THEME_CLR(theme, clrRecMenuDayInactive, 0xFFFF0000);
THEME_CLR(theme, clrRecMenuDayHighlight, 0x44FFFFFF);
THEME_CLR(theme, clrRecMenuTextBack, 0xFF000000);
THEME_CLR(theme, clrRecMenuTextActiveBack, 0xFF404749);
THEME_CLR(theme, clrRecMenuKeyboardBack, 0xFF000000);
THEME_CLR(theme, clrRecMenuKeyboardBorder, clrWhite);
THEME_CLR(theme, clrRecMenuKeyboardHigh, 0x55FFFFFF);
THEME_CLR(theme, clrButtonRedKeyboard, 0xFFBB0000);
THEME_CLR(theme, clrButtonGreenKeyboard, 0xFF00BB00);
THEME_CLR(theme, clrButtonYellowKeyboard, 0xFFBBBB00);

#include "config.c"
cTvguideConfig tvguideConfig;

#include "osdmanager.c"
cOsdManager osdManager;

#include "services/epgsearch.h"
#include "services/remotetimers.h"
cPlugin* pRemoteTimers = NULL;

#include "tools.c"
#include "switchtimer.c"
#include "setup.c"
#include "imageloader.c"
#include "styledpixmap.c"
#include "timer.c"
#include "timeline.c"
#include "grid.c"
#include "headergrid.c"
#include "dummygrid.c"
#include "epggrid.c"
#include "statusheader.c"
#include "detailview.c"
#include "channelcolumn.c"
#include "channelgroup.c"
#include "channelgroups.c"
#include "footer.c"
#include "recmenuitem.c"
#include "recmenu.c"
#include "recmanager.c"
#include "recmenus.c"
#include "recmenumanager.c"

#include "tvguideosd.h"
#include <stdlib.h>

cTvGuideOsd::cTvGuideOsd(void) {
    detailView = NULL;
    detailViewActive = false;
    activeGrid = NULL;
    timeLine = NULL;
    recMenuManager = NULL;
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
    delete channelGroups;
    delete footer;
    delete recMenuManager;
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
        SwitchTimers.Load(AddDirectory(cPlugin::ConfigDirectory("epgsearch"), "epgsearchswitchtimers.conf"));
        recMenuManager = new cRecMenuManager();
        pRemoteTimers = cPluginManager::CallFirstService("RemoteTimers::RefreshTimers-v1.0", NULL);
        if (pRemoteTimers) {
            isyslog("tvguide: remotetimers-plugin is available");
        }
        if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
            cString errorMsg;
            if (!pRemoteTimers->Service("RemoteTimers::RefreshTimers-v1.0", &errorMsg)) {
                esyslog("tvguide: %s", *errorMsg);
            }
        }
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
    channelGroups = new cChannelGroups();
    channelGroups->ReadChannelGroups();
    //channelGroups->DumpGroups();
    footer = new cFooter(channelGroups);
    footer->drawRedButton();
    if (tvguideConfig.channelJumpMode == eNumJump) {
        footer->drawGreenButton();
        footer->drawYellowButton();
    }
    footer->drawBlueButton();
    osdManager.flush();
    readChannels(startChannel);
    drawGridsChannelJump();
    osdManager.flush();
    cPixmap::Unlock();
}

void cTvGuideOsd::readChannels(const cChannel *channelStart) {
    int i=0;
    bool foundEnough = false;
    columns.Clear();
    if (!channelStart)
        return;
    for (const cChannel *channel = channelStart; channel; channel = Channels.Next(channel)) {
        if (!channel->GroupSep()) {
            if (channelGroups->IsInLastGroup(channel)) {
                break;
            }
            cChannelColumn *column = new cChannelColumn(i, channel, myTime);
            if (column->readGrids()) {
                columns.Add(column);
                i++;
            } else {
                delete column;
            }
        }
        if (i == tvguideConfig.numGrids) {
            foundEnough = true;
            break;
        }
    }
    if (!foundEnough) {
        int numCurrent = columns.Count();
        int numBack = tvguideConfig.numGrids - numCurrent;
        int newChannelNumber = columns.First()->getChannel()->Number() - numBack;
        const cChannel *newStart = Channels.GetByNumber(newChannelNumber);
        readChannels(newStart);
    }
}

void cTvGuideOsd::drawGridsChannelJump(int offset) {
    if (columns.Count() == 0)
        return;
    activeGrid = columns.Get(offset)->getActive();
    if (activeGrid)
        activeGrid->SetActive();
    if (tvguideConfig.displayStatusHeader) {
        statusHeader->DrawInfoText(activeGrid);
    }
    if (activeGrid && (tvguideConfig.channelJumpMode == eGroupJump)) {
        footer->UpdateGroupButtons(activeGrid->column->getChannel());
    }
    if (tvguideConfig.displayChannelGroups) {
        channelGroups->DrawChannelGroups(columns.First()->getChannel(), columns.Last()->getChannel());
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
    bool colAdded = false;
    if (!colRight) {
        const cChannel *channelRight = activeGrid->column->getChannel();
        while (channelRight = Channels.Next(channelRight)) {
            if (!channelRight->GroupSep()) {
                if (channelGroups->IsInLastGroup(channelRight)) {
                    break;
                }
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
            colAdded = true;
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
    if (tvguideConfig.displayChannelGroups && colAdded) {
        channelGroups->DrawChannelGroups(columns.First()->getChannel(), columns.Last()->getChannel());
    }
    if (activeGrid && (tvguideConfig.channelJumpMode == eGroupJump)) {
        footer->UpdateGroupButtons(activeGrid->column->getChannel());
    }
    osdManager.flush();
}

void cTvGuideOsd::channelBack() {
    cChannelColumn *colLeft = columns.Prev(activeGrid->column);
    bool colAdded = false;
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
            colAdded = true;
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
    if (tvguideConfig.displayChannelGroups && colAdded) {
        channelGroups->DrawChannelGroups(columns.First()->getChannel(), columns.Last()->getChannel());
    }

    if (activeGrid && (tvguideConfig.channelJumpMode == eGroupJump)) {
        footer->UpdateGroupButtons(activeGrid->column->getChannel());
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
    if (tvguideConfig.displayMode == eVertical) {
        timeBack();
    } else if (tvguideConfig.displayMode == eHorizontal) {
        channelBack();
    }
}

void cTvGuideOsd::processKeyDown() {
    if (!activeGrid) {
            return;
    }
    if (tvguideConfig.displayMode == eVertical) {
        timeForward();
    } else if (tvguideConfig.displayMode == eHorizontal) {
        channelForward();
    }
}

void cTvGuideOsd::processKeyLeft() {
    if (activeGrid == NULL)
        return;
    if (tvguideConfig.displayMode == eVertical) {
        channelBack();
    } else if (tvguideConfig.displayMode == eHorizontal) {
        timeBack();
    }
}

void cTvGuideOsd::processKeyRight() {
    if (activeGrid == NULL)
        return;
    if (tvguideConfig.displayMode == eVertical) {
        channelForward();
    } else if (tvguideConfig.displayMode == eHorizontal) {
        timeForward();
    }
}

void cTvGuideOsd::processKeyRed() {
    if  ((activeGrid == NULL) || activeGrid->isDummy())
        return;
    recMenuManager->Start(activeGrid->GetEvent());
}

void cTvGuideOsd::processKeyGreen() {
    if (activeGrid == NULL)
        return;
    
    const cChannel *currentChannel = activeGrid->column->getChannel();
    const cChannel *firstChannel = columns.First()->getChannel();
    int currentCol = activeGrid->column->GetNum();
    const cChannel *prev = NULL;
    
    if (tvguideConfig.channelJumpMode == eGroupJump) {
        int prevNum = channelGroups->GetPrevGroupChannelNumber(currentChannel);
        if (prevNum) {
            prev = Channels.GetByNumber(prevNum);
        }    
    } else if (tvguideConfig.channelJumpMode == eNumJump) {
        int i = tvguideConfig.jumpChannels + 1;
        for (const cChannel *channel = firstChannel; channel; channel = Channels.Prev(channel)) {
            if (!channel->GroupSep()) {
                prev = channel;
                i--;
            }
            if (i == 0)
                break;
        }
    }
    if (prev) {
        readChannels(prev);
        if (columns.Count() > 0) {
            drawGridsChannelJump(currentCol);
        }
        osdManager.flush();
    }
}

void cTvGuideOsd::processKeyYellow() {
    if (activeGrid == NULL)
        return;
    const cChannel *currentChannel = activeGrid->column->getChannel();
    int currentCol = activeGrid->column->GetNum();
    const cChannel *firstChannel = columns.First()->getChannel();
    const cChannel *next = NULL;
    
    if (tvguideConfig.channelJumpMode == eGroupJump) {
        int nextNum = channelGroups->GetNextGroupChannelNumber(currentChannel);
        if (nextNum) {
            next = Channels.GetByNumber(nextNum);
        }    
    } else if (tvguideConfig.channelJumpMode == eNumJump) {
        int i=0;
        for (const cChannel *channel = firstChannel; channel; channel = Channels.Next(channel)) {
            if (channelGroups->IsInLastGroup(channel)) {
                break;
            }
            if (!channel->GroupSep()) {
                next = channel;
                i++;
            }
            if (i == (tvguideConfig.jumpChannels+1)) {
                break;
            }
        }
    }
    if (next) {
        readChannels(next);
        if (columns.Count() > 0) {
            drawGridsChannelJump(currentCol);
        }
        osdManager.flush();
    }
}

eOSState cTvGuideOsd::processKeyBlue() {
    if (tvguideConfig.blueKeyMode == 0) {
        return ChannelSwitch();
    } else if (tvguideConfig.blueKeyMode == 1) {
        DetailedEPG();
    }
    return osContinue;
}

eOSState cTvGuideOsd::processKeyOk() {
    if (tvguideConfig.blueKeyMode == 0) {
        DetailedEPG();
    } else if (tvguideConfig.blueKeyMode == 1) {
        return ChannelSwitch();
    }
    return osContinue;
}

eOSState cTvGuideOsd::ChannelSwitch() {
    if (activeGrid == NULL)
        return osContinue;
    const cChannel *currentChannel = activeGrid->column->getChannel();
    if (currentChannel) {
        cDevice::PrimaryDevice()->SwitchChannel(currentChannel, true);
        if (tvguideConfig.closeOnSwitch)
            return osEnd;
    }
    return osContinue;
}

void cTvGuideOsd::DetailedEPG() {
    if (!activeGrid->isDummy()) {
        detailViewActive = true;
        detailView = new cDetailView(activeGrid->GetEvent());
        detailView->drawHeader();
        detailView->drawContent();
        detailView->drawScrollbar();
        osdManager.flush();
    }
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

void cTvGuideOsd::SetTimers() {
    for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
        column->SetTimers();
    }
}

eOSState cTvGuideOsd::ProcessKey(eKeys Key) {
    eOSState state = osContinue;
    cPixmap::Lock();
    if (recMenuManager->isActive()) {
        state = recMenuManager->ProcessKey(Key);
        if (state == osEnd) {
            SetTimers();                
            osdManager.flush();
        }
        state = osContinue;
    } else if (detailViewActive) {
        if ((Key & ~k_Repeat) == kRed) {
            delete detailView;
            detailView = NULL;
            detailViewActive = false;
            processKeyRed();
        } else if (((Key & ~k_Repeat) == kBlue) && (tvguideConfig.blueKeyMode == 0)) {
            delete detailView;
            detailView = NULL;
            detailViewActive = false;
            processKeyBlue();
        } else {
            state = detailView->ProcessKey(Key);
            if (state == osEnd) {
                delete detailView;
                detailView = NULL;
                detailViewActive = false;
                osdManager.flush();
                state = osContinue;
            }
        }
    } else {
        switch (Key & ~k_Repeat) {
            case kUp:       processKeyUp(); break;
            case kDown:     processKeyDown(); break;
            case kLeft:     processKeyLeft(); break;
            case kRight:    processKeyRight(); break;
            case kRed:      processKeyRed(); break;
            case kGreen:    processKeyGreen(); break;
            case kYellow:   processKeyYellow(); break;
            case kBlue:     state = processKeyBlue(); break;
            case kOk:       state = processKeyOk(); break;
            case kBack:     state=osEnd; break;    
            case k1:        processKey1(); break;
            case k3:        processKey3(); break;
            case k4:        processKey4(); break;
            case k6:        processKey6(); break;
            case k7:        processKey7(); break;
            case k9:        processKey9(); break;
            default:        break;
        }
    }
    cPixmap::Unlock();
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
