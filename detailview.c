#include <sstream>
#include "detailview.h"

cDetailView::cDetailView(const cEvent *event) {
    this->event = event;
    imgScrollBar = NULL;
    borderWidth = 100; //px
    scrollBarWidth = 40;
    headerHeight = max (40 + 3 * tvguideConfig.FontDetailHeader->Height(), // border + 3 Lines
                        40 + tvguideConfig.epgImageHeight);
    description.Set(event->Description(), tvguideConfig.FontDetailView, tvguideConfig.osdWidth-2*borderWidth - 50 - 40);
    if (tvguideConfig.displayRerunsDetailEPGView) {
        loadReruns();
    }
    numEPGPics = 0;
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
    if (tvguideConfig.displayRerunsDetailEPGView) {
        linesContent += reruns.Lines() + 1;
    }
    heightContent = linesContent * tvguideConfig.FontDetailView->Height();
    if (!tvguideConfig.hideEpgImages) {
        heightContent += heightEPGPics();
    }
    if (heightContent > (tvguideConfig.osdHeight - 2 * borderWidth - headerHeight))
        return true;
    else
        return false;
}

void cDetailView::createPixmaps() {
    header = new cStyledPixmap(osdManager.requestPixmap(5, cRect(borderWidth, borderWidth, tvguideConfig.osdWidth - 2*borderWidth, headerHeight), cRect::Null));
    headerLogo = osdManager.requestPixmap(6, cRect(borderWidth, borderWidth, tvguideConfig.osdWidth - 2*borderWidth, headerHeight), cRect::Null);
    headerLogo->Fill(clrTransparent);
    headerBack = osdManager.requestPixmap(4, cRect(borderWidth, borderWidth, tvguideConfig.osdWidth - 2*borderWidth, headerHeight), cRect::Null);
    headerBack->Fill(clrBlack);
    header->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
    content = osdManager.requestPixmap(5, cRect(borderWidth, borderWidth + headerHeight, tvguideConfig.osdWidth - 2*borderWidth - scrollBarWidth, tvguideConfig.osdHeight-2*borderWidth-headerHeight),
                                    cRect(0,0, tvguideConfig.osdWidth - 2*borderWidth - scrollBarWidth, max(heightContent, tvguideConfig.osdHeight-2*borderWidth-headerHeight)));   
    header->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
    
    scrollBar = osdManager.requestPixmap(5, cRect(tvguideConfig.osdWidth-borderWidth-scrollBarWidth, borderWidth + headerHeight, scrollBarWidth, tvguideConfig.osdHeight-2*borderWidth-headerHeight));
    
    footer = osdManager.requestPixmap(5, cRect(borderWidth, borderWidth + headerHeight + content->ViewPort().Height(), tvguideConfig.osdWidth - 2*borderWidth, 3));
    footer->Fill(theme.Color(clrBorder));
}

void cDetailView::drawHeader() {
    header->drawBackground();
    header->drawBoldBorder();
    tColor colorTextBack = (tvguideConfig.useBlending==0)?theme.Color(clrHeader):clrTransparent;
    int logoHeight = header->Height() / 2;
    int logoWidth = logoHeight * tvguideConfig.logoWidthRatio / tvguideConfig.logoHeightRatio;
    int lineHeight = tvguideConfig.FontDetailHeader->Height();
    cImageLoader imgLoader;
    bool logoDrawn = false;
    if (!tvguideConfig.hideChannelLogos) {
        cString channelName = Channels.GetByChannelID(event->ChannelID())->Name();
        if (imgLoader.LoadLogo(*channelName, logoWidth, logoHeight)) {
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
    header->DrawText(cPoint(textX, textY), event->Title(), theme.Color(clrFont), colorTextBack, tvguideConfig.FontDetailHeader);
    cString datetime = cString::sprintf("%s, %s - %s (%d min)", *event->GetDateString(),  *event->GetTimeString(), *event->GetEndTimeString(), event->Duration()/60);
    header->DrawText(cPoint(textX, textY + lineHeight), *datetime, theme.Color(clrFont), colorTextBack, tvguideConfig.FontDetailView);
    header->DrawText(cPoint(textX, textY + 2 * lineHeight), event->ShortText(), theme.Color(clrFont), colorTextBack, tvguideConfig.FontDetailView);
}

void cDetailView::drawContent() {
    content->Fill(theme.Color(clrBorder));
    content->DrawRectangle(cRect(2, 0, content->ViewPort().Width() - 2, content->DrawPort().Height()), theme.Color(clrBackground));
    tColor colorTextBack = (tvguideConfig.useBlending==0)?theme.Color(clrBackground):clrTransparent;
    
    int textHeight = tvguideConfig.FontDetailView->Height();
    int textLines = description.Lines();
    int i=0;
    for (; i<textLines; i++) {
        content->DrawText(cPoint(20, 20 + i*textHeight), description.GetLine(i), theme.Color(clrFont), colorTextBack, tvguideConfig.FontDetailView);
    }
    i++;
    if (tvguideConfig.displayRerunsDetailEPGView) {
        textLines = reruns.Lines();
        for (int j=0; j<textLines; j++) {
            content->DrawText(cPoint(20, 20 + i*textHeight), reruns.GetLine(j), theme.Color(clrFont), colorTextBack, tvguideConfig.FontDetailView);
            i++;
        }
    }
    if (!tvguideConfig.hideEpgImages) {
        drawEPGPictures((i+1)*textHeight);
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

void cDetailView::pageUp() {
    if (contentScrollable) {
        int aktHeight = (-1)*content->DrawPort().Point().Y();
        int totalHeight = content->DrawPort().Height();
        int screenHeight = content->ViewPort().Height();
        int newHeight = max(aktHeight - screenHeight, 0);
        content->SetDrawPortPoint(cPoint(0, (-1)*newHeight));
        drawScrollbar();
   }
}

void cDetailView::pageDown() {
    if (contentScrollable) {
        int aktHeight = (-1)*content->DrawPort().Point().Y();
        int totalHeight = content->DrawPort().Height();
        int screenHeight = content->ViewPort().Height();
        int newHeight = min(aktHeight + screenHeight, totalHeight - screenHeight);
        content->SetDrawPortPoint(cPoint(0, (-1)*newHeight));
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

void cDetailView::loadReruns(void) {
    cPlugin *epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
    if (epgSearchPlugin && !isempty(event->Title())) {
        std::stringstream sstrReruns;
        Epgsearch_searchresults_v1_0 data;
        std::string strQuery = event->Title();
        if (tvguideConfig.useSubtitleRerun > 0) {
            if (tvguideConfig.useSubtitleRerun == 2 || !isempty(event->ShortText()))
                strQuery += "~";
            if (!isempty(event->ShortText()))
                strQuery += event->ShortText();
                data.useSubTitle = true;
        } else {
            data.useSubTitle = false;
        }
        data.query = (char *)strQuery.c_str();
        data.mode = 0;
        data.channelNr = 0;
        data.useTitle = true;
        data.useDescription = false;
        
        if (epgSearchPlugin->Service("Epgsearch-searchresults-v1.0", &data)) {
            cList<Epgsearch_searchresults_v1_0::cServiceSearchResult>* list = data.pResultList;
            if (list && (list->Count() > 1)) {
                sstrReruns << tr("RERUNS OF THIS SHOW") << ':' << std::endl;
                int i = 0;
                for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = list->First(); r && i < tvguideConfig.numReruns; r = list->Next(r)) {
                    if ((event->ChannelID() == r->event->ChannelID()) && (event->StartTime() == r->event->StartTime()))
                        continue;
                    i++;
                    sstrReruns  << "- "
                                << *DayDateTime(r->event->StartTime());
                    cChannel *channel = Channels.GetByChannelID(r->event->ChannelID(), true, true);
                    if (channel)
                        sstrReruns << " " << channel->ShortName(true);
                    sstrReruns << ":  " << r->event->Title();
                    if (!isempty(r->event->ShortText()))
                        sstrReruns << "~" << r->event->ShortText();
                    sstrReruns << std::endl;
                }
                delete list;
            }
        }
        reruns.Set(sstrReruns.str().c_str(), tvguideConfig.FontDetailView, tvguideConfig.osdWidth-2*borderWidth - 50 - 40);
    } else
        reruns.Set("", tvguideConfig.FontDetailView, tvguideConfig.osdWidth-2*borderWidth - 50 - 40);
}

int cDetailView::heightEPGPics(void) {
    int width = tvguideConfig.osdWidth - 2*borderWidth - scrollBarWidth;
    int border = 5;
    int numPicsAvailable = 0;
    for (int i=1; i <= tvguideConfig.numAdditionalEPGPictures; i++) {
        cString epgimage = cString::sprintf("%s%d_%d.jpg", *tvguideConfig.epgImagePath, event->EventID(), i);
        FILE *fp = fopen(*epgimage, "r");
        if (fp) {
            numPicsAvailable = i;
            fclose(fp);
        } else {
            break;
        }
    }
    numEPGPics = numPicsAvailable;
    int picsPerLine = width / (tvguideConfig.epgImageWidthLarge + border);
    int picLines = numPicsAvailable / picsPerLine;
    if (numPicsAvailable%picsPerLine != 0)
        picLines++;
    return picLines * (tvguideConfig.epgImageHeightLarge + border) + 2*border;
}

void cDetailView::drawEPGPictures(int height) {
    int width = content->ViewPort().Width();
    int border = 5;
    int picsPerLine = width / (tvguideConfig.epgImageWidthLarge + border);
    int currentX = border;
    int currentY = height + border;
    int currentPicsPerLine = 1;
    cImageLoader imgLoader;
    for (int i=1; i <= numEPGPics; i++) {
        cString epgimage = cString::sprintf("%d_%d", event->EventID(), i);
        if (imgLoader.LoadAdditionalEPGImage(epgimage)) {
            content->DrawImage(cPoint(currentX, currentY), imgLoader.GetImage());
            int radius = 10;
            content->DrawEllipse(cRect(currentX,currentY,radius,radius), theme.Color(clrBackground), -2);
            content->DrawEllipse(cRect(currentX + tvguideConfig.epgImageWidthLarge - radius,currentY,radius,radius), theme.Color(clrBackground), -1);
            content->DrawEllipse(cRect(currentX,currentY + tvguideConfig.epgImageHeightLarge - radius,radius,radius), theme.Color(clrBackground), -3);
            content->DrawEllipse(cRect(currentX + tvguideConfig.epgImageWidthLarge - radius,currentY + tvguideConfig.epgImageHeightLarge - radius,radius,radius), theme.Color(clrBackground), -4);
            if (currentPicsPerLine < picsPerLine) {
                currentX += tvguideConfig.epgImageWidthLarge + border;
                currentPicsPerLine++;
            } else {
                currentX = border;
                currentY += tvguideConfig.epgImageHeightLarge + border;
                currentPicsPerLine = 1;
            }
        } else {
            break;
        }
    }
}

eOSState cDetailView::ProcessKey(eKeys Key) {
    eOSState state = osContinue;
    switch (Key & ~k_Repeat) {
        case kUp:
            scrollUp();
            osdManager.flush();
            break;
        case kDown:     
            scrollDown(); 
            osdManager.flush();
            break;
        case kLeft:
            pageUp();
            osdManager.flush();
            break;
        case kRight:
            pageDown();
            osdManager.flush();
            break;
        case kOk:
        case kBack:
            state = osEnd;
            break;
    }
    return state;
}