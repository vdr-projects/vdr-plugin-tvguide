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
    viewportHeight = Duration() / 60 * tvguideConfig.minutePixel;
    if (viewportHeight != viewportHeightOld)
        dirty = true;
}

void cDummyGrid::PositionPixmap() {
    int x0, y0;
    if (tvguideConfig.displayMode == eVertical) {
        x0 = column->getX();
        y0 = tvguideConfig.statusHeaderHeight + tvguideConfig.channelHeaderHeight;
        if ( column->Start() < StartTime() ) {
            y0 += (StartTime() - column->Start())/60*tvguideConfig.minutePixel;
        }
        if (!pixmap) {
            pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, tvguideConfig.colWidth, viewportHeight));
        } else if (dirty) {
            osdManager.releasePixmap(pixmap);
            pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, tvguideConfig.colWidth, viewportHeight));
        } else {
            pixmap->SetViewPort(cRect(x0, y0, tvguideConfig.colWidth, viewportHeight));
        }
    } else if (tvguideConfig.displayMode == eHorizontal) {
        x0 = tvguideConfig.channelHeaderWidth;
        y0 = column->getY();
        if ( column->Start() < StartTime() ) {
            x0 += (StartTime() - column->Start())/60*tvguideConfig.minutePixel;
        }
        if (!pixmap) {
            pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, viewportHeight, tvguideConfig.rowHeight));
        } else if (dirty) {
            osdManager.releasePixmap(pixmap);
            pixmap = osdManager.requestPixmap(-1, cRect(x0, y0, viewportHeight, tvguideConfig.rowHeight));
        } else {
            pixmap->SetViewPort(cRect(x0, y0, viewportHeight, tvguideConfig.rowHeight));
        }
    }
}

void cDummyGrid::setText() {
    if (tvguideConfig.displayMode == eVertical) {
        text->Set(*strText, tvguideConfig.FontGrid, tvguideConfig.colWidth-2*borderWidth);
    }
}

void cDummyGrid::drawText() {
    tColor colorText = (active)?theme.Color(clrFontActive):theme.Color(clrFont);
    if (tvguideConfig.displayMode == eVertical) {
        if (Height()/tvguideConfig.minutePixel < 6)
            return;
        int textHeight = tvguideConfig.FontGrid->Height();
        int textLines = text->Lines();
        for (int i=0; i<textLines; i++) {
            pixmap->DrawText(cPoint(borderWidth, borderWidth + i*textHeight), text->GetLine(i), colorText, clrTransparent, tvguideConfig.FontGrid);
        
        }
    } else if (tvguideConfig.displayMode == eHorizontal) {
        if (Width()/tvguideConfig.minutePixel < 10) {
            int titleY = (tvguideConfig.rowHeight - tvguideConfig.FontGridHorizontal->Height())/2;
            pixmap->DrawText(cPoint(borderWidth - 2, titleY), "...", colorText, clrTransparent, tvguideConfig.FontGridHorizontal);
            return;
        }
        int titleY = (tvguideConfig.rowHeight - tvguideConfig.FontGridHorizontal->Height())/2;
        pixmap->DrawText(cPoint(borderWidth, titleY), *strText, colorText, clrTransparent, tvguideConfig.FontGridHorizontal);
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