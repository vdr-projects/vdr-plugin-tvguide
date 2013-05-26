#include "detailview.h"

cDetailView::cDetailView(cGrid *grid) {
    this->grid = grid;
    this->event = grid->GetEvent();
    imgScrollBar = NULL;
    FrameTime = 40; // ms
    FadeTime = 500; // ms
    borderWidth = 100; //px
    headerHeight = max (40 + 3 * tvguideConfig.FontDetailHeader->Height(), // border + 3 Lines
                        40 + tvguideConfig.epgImageHeight);
    description.Set(event->Description(), tvguideConfig.FontDetailView, tvguideConfig.osdWidth-2*borderWidth - 50 - 40);
    contentScrollable = setContentDrawportHeight();
    createPixmaps();
}

cDetailView::~cDetailView(void){
    delete header;
    osdManager.releasePixmap(headerLogo);
    osdManager.releasePixmap(headerBack);
    osdManager.releasePixmap(content);
    osdManager.releasePixmap(scrollBar);
    osdManager.releasePixmap(footer);
    delete imgScrollBar;
}

bool cDetailView::setContentDrawportHeight() {
    int linesContent = description.Lines() + 1;
    heightContent = linesContent * tvguideConfig.FontDetailView->Height();
    if (heightContent > (tvguideConfig.osdHeight - 2 * borderWidth - headerHeight))
        return true;
    else
        return false;
}

void cDetailView::createPixmaps() {
    int scrollBarWidth = 50;
    
    header = new cStyledPixmap(osdManager.requestPixmap(5, cRect(borderWidth, borderWidth, tvguideConfig.osdWidth - 2*borderWidth, headerHeight), cRect::Null));
    header->SetAlpha(0);
    headerLogo = osdManager.requestPixmap(6, cRect(borderWidth, borderWidth, tvguideConfig.osdWidth - 2*borderWidth, headerHeight), cRect::Null);
    headerLogo->Fill(clrTransparent);
    headerLogo->SetAlpha(0);
    headerBack = osdManager.requestPixmap(4, cRect(borderWidth, borderWidth, tvguideConfig.osdWidth - 2*borderWidth, headerHeight), cRect::Null);
    headerBack->SetAlpha(0);
    headerBack->Fill(clrBlack);
    header->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
    content = osdManager.requestPixmap(5, cRect(borderWidth, borderWidth + headerHeight, tvguideConfig.osdWidth - 2*borderWidth - scrollBarWidth, tvguideConfig.osdHeight-2*borderWidth-headerHeight),
                                    cRect(0,0, tvguideConfig.osdWidth - 2*borderWidth - scrollBarWidth, max(heightContent, tvguideConfig.osdHeight-2*borderWidth-headerHeight)));   
    content->SetAlpha(0);
    header->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
    
    scrollBar = osdManager.requestPixmap(5, cRect(tvguideConfig.osdWidth-borderWidth-scrollBarWidth, borderWidth + headerHeight, scrollBarWidth, tvguideConfig.osdHeight-2*borderWidth-headerHeight));
    scrollBar->SetAlpha(0);
    
    footer = osdManager.requestPixmap(5, cRect(borderWidth, borderWidth + headerHeight + content->ViewPort().Height(), tvguideConfig.osdWidth - 2*borderWidth, 3));
    footer->SetAlpha(0);
    footer->Fill(theme.Color(clrBorder));
}

void cDetailView::drawHeader() {
    header->drawBackground();
    header->drawBoldBorder();
    int logoHeight = header->Height() / 2;
    int logoWidth = logoHeight * tvguideConfig.logoWidthRatio / tvguideConfig.logoHeightRatio;
    int lineHeight = tvguideConfig.FontDetailHeader->Height();
    cImageLoader imgLoader;
    bool logoDrawn = false;
    if (!tvguideConfig.hideChannelLogos) {
        if (imgLoader.LoadLogo(grid->column->getChannel()->Name(), logoWidth, logoHeight)) {
            cImage logo = imgLoader.GetImage();
            headerLogo->DrawImage(cPoint(10, (header->Height() - logoHeight)/2), logo);
            logoDrawn = true;
        }
    }
    
    if (!tvguideConfig.hideEpgImages) {
        if (imgLoader.LoadEPGImage(event->EventID())) {
            cImage epgImage = imgLoader.GetImage();
            int epgImageX = header->Width() - 30 - tvguideConfig.epgImageWidth;
            int epgImageY = (header->Height() - 10 - tvguideConfig.epgImageHeight) / 2;
            header->DrawRectangle(cRect(epgImageX-2, epgImageY-2, tvguideConfig.epgImageWidth + 4, tvguideConfig.epgImageHeight + 4), theme.Color(clrBorder));
            header->DrawImage(cPoint(epgImageX, epgImageY), epgImage);
        }
    }
    int textX = logoDrawn?(20 + logoWidth):20;
    int textY = (header->Height() - 2*lineHeight)/2;
    header->DrawText(cPoint(textX, textY), event->Title(), theme.Color(clrFont), clrTransparent, tvguideConfig.FontDetailHeader);
    cString datetime = cString::sprintf("%s, %s - %s (%d min)", *event->GetDateString(),  *event->GetTimeString(), *event->GetEndTimeString(), event->Duration()/60);
    header->DrawText(cPoint(textX, textY + lineHeight), *datetime, theme.Color(clrFont), clrTransparent, tvguideConfig.FontDetailView);
    header->DrawText(cPoint(textX, textY + 2 * lineHeight), event->ShortText(), theme.Color(clrFont), clrTransparent, tvguideConfig.FontDetailView);
}

void cDetailView::drawContent() {
    content->Fill(theme.Color(clrBorder));
    content->DrawRectangle(cRect(2, 0, content->ViewPort().Width() - 2, content->DrawPort().Height()), theme.Color(clrBackground));
    
    int textHeight = tvguideConfig.FontDetailView->Height();
    int textLines = description.Lines();
    
    for (int i=0; i<textLines; i++) {
        content->DrawText(cPoint(20, 20 + i*textHeight), description.GetLine(i), theme.Color(clrFont), clrTransparent, tvguideConfig.FontDetailView);
    }
}

void cDetailView::drawScrollbar() {
    scrollBar->Fill(theme.Color(clrBorder));
    double scrollBarOffset = 0.0;
    if (contentScrollable) {
        heightScrollbar =  ( (double)scrollBar->ViewPort().Height() ) / (double)heightContent * ( (double)scrollBar->ViewPort().Height() );
        scrollBarOffset = (-1.0)*(double)content->DrawPort().Point().Y() / (double)(content->DrawPort().Height() - (tvguideConfig.osdHeight-2*borderWidth-headerHeight));
        scrollBarOffset *= ( (double)scrollBar->ViewPort().Height()-7.0  - heightScrollbar); 
        scrollBarOffset++;
    } else {
        heightScrollbar =  scrollBar->ViewPort().Height();
    }
    scrollBar->DrawRectangle(cRect(3,0,scrollBar->ViewPort().Width()-6, scrollBar->ViewPort().Height()), theme.Color(clrBackground));
    if (imgScrollBar == NULL) {
        imgScrollBar = createScrollbar(scrollBar->ViewPort().Width()-10, heightScrollbar, theme.Color(clrHighlight), theme.Color(clrHighlightBlending));
    }
    scrollBar->DrawImage(cPoint(5, 2 + scrollBarOffset), *imgScrollBar);
}

void cDetailView::scrollUp() {
    if (contentScrollable) {
        int newDrawportHeight = content->DrawPort().Point().Y() + tvguideConfig.FontDetailView->Height();
        content->SetDrawPortPoint(cPoint(0, min(newDrawportHeight,0)));
        drawScrollbar();
    }
}

void cDetailView::scrollDown() {
    if (contentScrollable) {
        int newDrawportHeight = content->DrawPort().Point().Y() - tvguideConfig.FontDetailView->Height();
        int maxDrawportHeight = (content->DrawPort().Height() - (tvguideConfig.osdHeight-2*borderWidth-headerHeight));
        content->SetDrawPortPoint(cPoint(0, max(newDrawportHeight,(-1)*maxDrawportHeight)));
        drawScrollbar();
    }
}

cImage *cDetailView::createScrollbar(int width, int height, tColor clrBgr, tColor clrBlend) {
    cImage *image = new cImage(cSize(width, height));
    image->Fill(clrBgr);
    if (tvguideConfig.useBlending) {
        int numSteps = 64;
        int alphaStep = 0x03;
        if (height < 30)
            return image;
        else if (height < 100) {
            numSteps = 32;
            alphaStep = 0x06;
        }
        int stepY = 0.5*height / numSteps;
        if (stepY == 0)
            stepY = 1;
        int alpha = 0x40;
        tColor clr;
        for (int i = 0; i<numSteps; i++) {
            clr = AlphaBlend(clrBgr, clrBlend, alpha);
            for (int y = i*stepY; y < (i+1)*stepY; y++) {
                for (int x=0; x<width; x++) {
                    image->SetPixel(cPoint(x,y), clr);
                }
            }
            alpha += alphaStep;
        }
    }
    return image;
}

void cDetailView::Action(void) {
    drawHeader();
    drawContent();
    drawScrollbar();
    uint64_t Start = cTimeMs::Now();
    while (true) {
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        double t = min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        header->SetAlpha(Alpha);
        headerBack->SetAlpha(Alpha);
        headerLogo->SetAlpha(Alpha);
        content->SetAlpha(Alpha);
        scrollBar->SetAlpha(Alpha);
        footer->SetAlpha(Alpha);
        osdManager.flush();
        cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Delta < FrameTime)
           cCondWait::SleepMs(FrameTime - Delta);
        if ((Now - Start) > FadeTime)
            break;
    }
}