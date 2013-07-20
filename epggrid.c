#include "channelcolumn.h"
#include "epggrid.h"

cEpgGrid::cEpgGrid(cChannelColumn *c, const cEvent *event)  : cGrid(c) {
    this->event = event;
    extText = new cTextWrapper();
    hasTimer = false;
    SetTimer();
    hasSwitchTimer = false;
    SetSwitchTimer();
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
    viewportHeight *= tvguideConfig.minutePixel;
    if (viewportHeight != viewportHeightOld)
        dirty = true;
}

void cEpgGrid::PositionPixmap() {
    int x0, y0;
    if (tvguideConfig.displayMode == eVertical) {
        int x0 = column->getX();
        int y0 = tvguideConfig.statusHeaderHeight + tvguideConfig.channelHeaderHeight + tvguideConfig.channelGroupsHeight;
        if ( column->Start() < StartTime() ) {
            y0 += (StartTime() - column->Start())/60*tvguideConfig.minutePixel;
        }
        if (!pixmap) {
            pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, tvguideConfig.colWidth, viewportHeight), 
                                                cRect(0, 0, tvguideConfig.colWidth, Duration()/60*tvguideConfig.minutePixel));
        } else {
            pixmap->SetViewPort(cRect(x0, y0, tvguideConfig.colWidth, viewportHeight));
        }
    } else if (tvguideConfig.displayMode == eHorizontal) {
        int x0 = tvguideConfig.channelHeaderWidth + tvguideConfig.channelGroupsWidth;
        int y0 = column->getY();
        if ( column->Start() < StartTime() ) {
            x0 += (StartTime() - column->Start())/60*tvguideConfig.minutePixel;
        }
        if (!pixmap) {
            pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, viewportHeight, tvguideConfig.rowHeight), 
                                                cRect(0, 0, Duration()/60*tvguideConfig.minutePixel, tvguideConfig.rowHeight));
        } else {
            pixmap->SetViewPort(cRect(x0, y0, viewportHeight, tvguideConfig.rowHeight ));
        }
   }

}

void cEpgGrid::SetTimer() {
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_Event_v1_0 rt;
        rt.event = event;
        if (pRemoteTimers->Service("RemoteTimers::GetTimerByEvent-v1.0", &rt))
            hasTimer = true;
    } else if (column->HasTimer()) {
        hasTimer = event->HasTimer();
    } else {
        hasTimer = false;
    }
}

void cEpgGrid::SetSwitchTimer() {
    if (column->HasSwitchTimer()) {
        hasSwitchTimer = SwitchTimers.EventInSwitchList(event);
    } else {
        hasSwitchTimer = false;
    }
}

void cEpgGrid::setText() {
    if (tvguideConfig.displayMode == eVertical) {
        cString strText;
        strText = cString::sprintf("%s - %s:\n%s", *(event->GetTimeString()), *(event->GetEndTimeString()), event->Title());
        text->Set(*(strText), tvguideConfig.FontGrid, tvguideConfig.colWidth-2*borderWidth);
        extText->Set(event->ShortText(), tvguideConfig.FontGridSmall, tvguideConfig.colWidth-2*borderWidth);
    } else if (tvguideConfig.displayMode == eHorizontal) {
        timeString = cString::sprintf("%s - %s", *(event->GetTimeString()), *(event->GetEndTimeString()));
    }
}

void cEpgGrid::drawText() {
    tColor colorText = (active)?theme.Color(clrFontActive):theme.Color(clrFont);
    tColor colorTextBack = (tvguideConfig.useBlending==0)?color:clrTransparent;
    if (tvguideConfig.displayMode == eVertical) {
        if (Height()/tvguideConfig.minutePixel < 6)
            return;
        int textHeight = tvguideConfig.FontGrid->Height();
        int textLines = text->Lines();
        for (int i=0; i<textLines; i++) {
            pixmap->DrawText(cPoint(borderWidth, borderWidth + i*textHeight), text->GetLine(i), colorText, colorTextBack, tvguideConfig.FontGrid);
        }
        int extTextLines = extText->Lines();
        int offset = (textLines+1)*textHeight - 0.5*textHeight;
        textHeight = tvguideConfig.FontGridSmall->Height();
        if ((Height()-textHeight-10) > offset) {
            for (int i=0; i<extTextLines; i++) {
                pixmap->DrawText(cPoint(borderWidth, borderWidth + offset + i*textHeight), extText->GetLine(i), colorText, colorTextBack, tvguideConfig.FontGridSmall);
            }
        }
    } else if (tvguideConfig.displayMode == eHorizontal) {
        if (Width()/tvguideConfig.minutePixel < 10) {
            int titleY = (tvguideConfig.rowHeight - tvguideConfig.FontGridHorizontal->Height())/2;
            pixmap->DrawText(cPoint(borderWidth - 2, titleY), "...", colorText, colorTextBack, tvguideConfig.FontGridHorizontal);
            return;
        }
        cString strTitle = CutText(event->Title(), viewportHeight, tvguideConfig.FontGridHorizontal).c_str();
        int titleY = 0;
        if (tvguideConfig.showTimeInGrid) {
            pixmap->DrawText(cPoint(borderWidth, borderWidth), *timeString, colorText, colorTextBack, tvguideConfig.FontGridHorizontalSmall);
            titleY = tvguideConfig.FontGridHorizontalSmall->Height() + (tvguideConfig.rowHeight - tvguideConfig.FontGridHorizontalSmall->Height() - tvguideConfig.FontGridHorizontal->Height())/2;
        } else {
            titleY = (tvguideConfig.rowHeight - tvguideConfig.FontGridHorizontal->Height())/2;
        }
        pixmap->DrawText(cPoint(borderWidth, titleY), *strTitle, colorText, colorTextBack, tvguideConfig.FontGridHorizontal);
    }
    if (hasSwitchTimer) 
        drawIcon("Switch", theme.Color(clrButtonYellow));
    if (hasTimer) 
        drawIcon("REC", theme.Color(clrButtonRed));
}

void cEpgGrid::drawIcon(cString iconText, tColor color) {
    
    const cFont *font = (tvguideConfig.displayMode == eVertical)
                        ?tvguideConfig.FontGrid
                        :tvguideConfig.FontGridHorizontalSmall;
    int textWidth = font->Width(*iconText)+2*borderWidth;
    int textHeight = font->Height()+10;
    pixmap->DrawRectangle( cRect(Width() - textWidth - borderWidth, Height() - textHeight - borderWidth, textWidth, textHeight), color);
    pixmap->DrawText(cPoint(Width() - textWidth, Height() - textHeight - borderWidth/2), *iconText, theme.Color(clrFont), color, font);   
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