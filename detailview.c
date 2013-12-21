#include <sstream>
#include <vdr/plugin.h>
#include "imageloader.h"
#include "imagecache.h"
#include "services/epgsearch.h"
#include "services/remotetimers.h"
#include "config.h"
#include "tools.h"
#include "detailview.h"

cDetailView::cDetailView(const cEvent *event) {
    this->event = event;
    imgScrollBar = NULL;
    border = tvguideConfig.epgViewBorder; //px, border in view window
    scrollBarWidth = 40;
    headerHeight = geoManager.epgViewHeaderHeight;
    pixmapPoster = NULL;
    width = geoManager.osdWidth;
    contentWidth = width - scrollBarWidth;
    contentX = 0;
    contentHeight = geoManager.osdHeight - headerHeight;
    widthPoster = 30 * contentWidth / 100;
}

cDetailView::~cDetailView(void){
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    osdManager.releasePixmap(back);
    delete header;
    header = NULL;
    osdManager.releasePixmap(headerLogo);
    headerLogo = NULL;
    osdManager.releasePixmap(headerBack);
    headerBack = NULL;
    osdManager.releasePixmap(content);
    content = NULL;
    if (pixmapPoster)
        osdManager.releasePixmap(pixmapPoster);
    pixmapPoster = NULL;
    osdManager.releasePixmap(scrollBar);
    scrollBar = NULL;
    osdManager.releasePixmap(footer);
    footer = NULL;
    delete imgScrollBar;
}

void cDetailView::setContent() {
    hasAdditionalMedia = false;
    static cPlugin *pTVScraper = cPluginManager::GetPlugin("tvscraper");
    if (pTVScraper) {
        mediaInfo.event = event;
        mediaInfo.isRecording = false;
        if (pTVScraper->Service("TVScraperGetFullInformation", &mediaInfo)) {
            hasAdditionalMedia = true;
        }
    }
    if (hasAdditionalMedia) {
        if (mediaInfo.posters.size() >= 1) {
            contentWidth -=  widthPoster;
            contentX += widthPoster;
        }
    }
    description.Set(event->Description(), fontManager.FontDetailView, contentWidth - scrollBarWidth - 2*border);
    if (tvguideConfig.displayRerunsDetailEPGView) {
        loadReruns();
    }
    numEPGPics = 0;
    contentScrollable = setContentDrawportHeight();
    createPixmaps();
}

bool cDetailView::setContentDrawportHeight() {
    int lineHeight = fontManager.FontDetailView->Height();
    //Height of banner (only for series)
    int heightBanner = 0;
    if (hasAdditionalMedia && (mediaInfo.type == typeSeries)) {
        heightBanner = mediaInfo.banner.height + 2*lineHeight;
    }
    //Height of EPG Text
    int heightEPG = (description.Lines()+1) * lineHeight;
    //Height of rerun information
    int heightReruns = 0;
    if (tvguideConfig.displayRerunsDetailEPGView) {
        heightReruns = (reruns.Lines()+1) * lineHeight;
    }
    //Height of actor pictures
    int heightActors = 0;
    if (hasAdditionalMedia) {
        heightActors = heightActorPics();
    }
    //Height of fanart
    int heightFanart = 0;
    if (hasAdditionalMedia) {
        heightFanart = heightFanartImg() + lineHeight;
    }
    //Height of EPG Pictures
    int heightEpgPics = 0;
    if (!tvguideConfig.hideEpgImages) {
        heightEpgPics = heightEPGPics();
    }
    
    yBanner  = border;
    yEPGText = yBanner + heightBanner;
    yAddInf  = yEPGText + heightEPG;
    yActors  = yAddInf + heightReruns;
    yFanart  = yActors + heightActors;
    yEPGPics = yAddInf + heightFanart;
    
    int totalHeight = heightBanner + heightEPG + heightReruns + heightActors + heightFanart +  heightEpgPics + lineHeight;
    //check if pixmap content has to be scrollable
    if (totalHeight > contentHeight) {
        heightContent = totalHeight;
        return true;
    } else {
        heightContent = contentHeight;
        return false;
    }
    return false;
}

void cDetailView::createPixmaps() {
    back = osdManager.requestPixmap(3, cRect(0, 0, geoManager.osdWidth, geoManager.osdHeight), cRect::Null);
    back->Fill(clrBlack);
    header = new cStyledPixmap(osdManager.requestPixmap(5, cRect(0, 0, width, headerHeight), cRect::Null));
    headerLogo = osdManager.requestPixmap(6, cRect(0, 0, width, headerHeight), cRect::Null);
    headerLogo->Fill(clrTransparent);
    headerBack = osdManager.requestPixmap(4, cRect(0, 0, width, headerHeight), cRect::Null);
    headerBack->Fill(clrBlack);
    header->setColor(theme.Color(clrHeader), theme.Color(clrHeaderBlending));
    content = osdManager.requestPixmap(5, cRect(contentX, headerHeight, contentWidth, contentHeight),
                                    cRect(0,0, contentWidth, max(heightContent, contentHeight)));
    if (hasAdditionalMedia) {
        pixmapPoster = osdManager.requestPixmap(4, cRect(0, 0 + headerHeight, widthPoster, contentHeight));
        pixmapPoster->Fill(theme.Color(clrBorder));
        pixmapPoster->DrawRectangle(cRect(2, 0, widthPoster - 2, content->DrawPort().Height()), theme.Color(clrBackground));
    }
    scrollBar = osdManager.requestPixmap(5, cRect(geoManager.osdWidth - scrollBarWidth, headerHeight, scrollBarWidth, contentHeight));
    
    footer = osdManager.requestPixmap(5, cRect(0, headerHeight + content->ViewPort().Height(), width, 3));
    footer->Fill(theme.Color(clrBorder));
}

void cDetailView::drawHeader() {
    if (tvguideConfig.style == eStyleGraphical) {
        header->drawBackgroundGraphical(bgEpgHeader);
    } else {
        header->drawBackground();
        header->drawBoldBorder();
    }
    tColor colorTextBack = (tvguideConfig.style == eStyleFlat)?theme.Color(clrHeader):clrTransparent;
    int logoHeight = 2 * header->Height() / 3;
    int logoWidth = logoHeight * tvguideConfig.logoWidthRatio / tvguideConfig.logoHeightRatio;
    int lineHeight = fontManager.FontDetailHeader->Height();
    cImageLoader imgLoader;
    bool logoDrawn = false;
    if (!tvguideConfig.hideChannelLogos) {
        const cChannel *channel = Channels.GetByChannelID(event->ChannelID());
        if (imgLoader.LoadLogo(channel, logoWidth, logoHeight)) {
            cImage logo = imgLoader.GetImage();
            headerLogo->DrawImage(cPoint(10, (header->Height() - logoHeight)/2), logo);
            logoDrawn = true;
        }
    }
    
    bool epgImageDrawn = false;
    int epgImageWidth = 0;
    if (!tvguideConfig.hideEpgImages) {
        int epgImageHeight = 3 * headerHeight / 4;
        if (tvguideConfig.epgImageHeight > 0)
            epgImageWidth = epgImageHeight * tvguideConfig.epgImageWidth / tvguideConfig.epgImageHeight;
        if (imgLoader.LoadEPGImage(event->EventID(), epgImageWidth, epgImageHeight)) {
            cImage epgImage = imgLoader.GetImage();
            int epgImageX = header->Width() - border - epgImageWidth;
            int epgImageY = (header->Height() - epgImageHeight) / 2;
            header->DrawRectangle(cRect(epgImageX-2, epgImageY-2, epgImageWidth + 4, epgImageHeight + 4), theme.Color(clrBorder));
            header->DrawImage(cPoint(epgImageX, epgImageY), epgImage);
            epgImageDrawn = true;
        }
    }
    int textX = logoDrawn?(border + logoWidth + 5):border;
    int textY = (header->Height() - 7*lineHeight/2)/2;
    int maxTextWidth = header->Width() - 2 * border;
    if (logoDrawn)
        maxTextWidth -= logoWidth;
    if (epgImageDrawn)
        maxTextWidth -= epgImageWidth;
    std::string title = CutText((event->Title())?event->Title():"", maxTextWidth, fontManager.FontDetailHeader);
    header->DrawText(cPoint(textX, textY), title.c_str(), theme.Color(clrFont), colorTextBack, fontManager.FontDetailHeader);
    std::string datetime = *cString::sprintf("%s, %s - %s (%d min)", *event->GetDateString(),  *event->GetTimeString(), *event->GetEndTimeString(), event->Duration()/60);
    datetime = CutText(datetime, maxTextWidth, fontManager.FontDetailView);
    textY += 5*lineHeight/4;
    header->DrawText(cPoint(textX, textY), datetime.c_str(), theme.Color(clrFont), colorTextBack, fontManager.FontDetailView);
    std::string shortText = CutText((event->ShortText())?event->ShortText():"", maxTextWidth, fontManager.FontDetailView);
    textY += 5*lineHeight/4;
    header->DrawText(cPoint(textX, textY), shortText.c_str(), theme.Color(clrFont), colorTextBack, fontManager.FontDetailView);
    
    eTimerMatch timerMatch=tmNone; 
    cTimer *ti;
    if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
        RemoteTimers_GetMatch_v1_0 rtMatch;
        rtMatch.event = event;
        pRemoteTimers->Service("RemoteTimers::GetMatch-v1.0", &rtMatch);
        timerMatch = (eTimerMatch)rtMatch.timerMatch;
        ti = rtMatch.timer;
    } else {
        ti=Timers.GetMatch(event, &timerMatch);
    }
    if (timerMatch == tmFull) {
        drawRecIcon();
    }
}

void cDetailView::drawRecIcon() {
    cString recIconText(" REC ");
    int headerWidth = width;
    int widthIcon = fontManager.FontDetailHeader->Width(*recIconText);
    int height = fontManager.FontDetailHeader->Height()+10;
    int posX = headerWidth - widthIcon - 20;
    int posY = 20;
    header->DrawRectangle( cRect(posX, posY, widthIcon, height), theme.Color(clrButtonRed));
    header->DrawText(cPoint(posX, posY+5), *recIconText, theme.Color(clrFont), theme.Color(clrButtonRed), fontManager.FontDetailHeader);
}

void cDetailView::drawContent() {
    content->Fill(theme.Color(clrBorder));
    content->DrawRectangle(cRect(2, 0, content->ViewPort().Width() - 2, content->DrawPort().Height()), theme.Color(clrBackground));
    tColor colorTextBack = (tvguideConfig.style == eStyleFlat)?theme.Color(clrBackground):clrTransparent;
    
    int textHeight = fontManager.FontDetailView->Height();
    int textLines = description.Lines();
    for (int i=0; i<textLines; i++) {
        content->DrawText(cPoint(border, yEPGText + i*textHeight), description.GetLine(i), theme.Color(clrFont), colorTextBack, fontManager.FontDetailView);
    }
    if (tvguideConfig.displayRerunsDetailEPGView) {
        textLines = reruns.Lines();
        for (int j=0; j<textLines; j++) {
            content->DrawText(cPoint(border, yAddInf + j*textHeight), reruns.GetLine(j), theme.Color(clrFont), colorTextBack, fontManager.FontDetailView);
        }
    }
}

void cDetailView::Action(void) {
    if (hasAdditionalMedia && Running()) {
        drawPoster();
        if (Running())
            osdManager.flush();
    }
       //draw banner only for series
    if (hasAdditionalMedia && (mediaInfo.type == typeSeries) && Running()) {
        drawBanner(yBanner);
        if (Running())
            osdManager.flush();
    }
    //draw actors
    if (hasAdditionalMedia && Running()) {
        drawActors(yActors);
        if (Running())
            osdManager.flush();
    }
    //draw fanart
    if (hasAdditionalMedia && Running()) {
        drawFanart(yFanart);
        if (Running())
            osdManager.flush();
    }
    if (!tvguideConfig.hideEpgImages && Running()) {
        drawEPGPictures(yEPGPics);
        if (Running())
            osdManager.flush();
    }
}

void cDetailView::drawScrollbar() {
    scrollBar->Fill(theme.Color(clrBorder));
    double scrollBarOffset = 0.0;
    if (contentScrollable) {
        heightScrollbar =  ( (double)scrollBar->ViewPort().Height() ) / (double)heightContent * ( (double)scrollBar->ViewPort().Height() );
        scrollBarOffset = (-1.0)*(double)content->DrawPort().Point().Y() / (double)(content->DrawPort().Height() - (contentHeight));
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
        int newDrawportHeight = content->DrawPort().Point().Y() + fontManager.FontDetailView->Height();
        content->SetDrawPortPoint(cPoint(0, min(newDrawportHeight,0)));
        drawScrollbar();
    }
}

void cDetailView::scrollDown() {
    if (contentScrollable) {
        int newDrawportHeight = content->DrawPort().Point().Y() - fontManager.FontDetailView->Height();
        int maxDrawportHeight = (content->DrawPort().Height() - contentHeight);
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
    if (tvguideConfig.style != eStyleFlat) {
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
        reruns.Set(sstrReruns.str().c_str(), fontManager.FontDetailView, contentWidth - scrollBarWidth - 2*border);
    } else
        reruns.Set("", fontManager.FontDetailView, contentWidth - scrollBarWidth);
}

int cDetailView::heightEPGPics(void) {
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
    int picsPerLine = contentWidth / (tvguideConfig.epgImageWidthLarge + border);
    int picLines = numPicsAvailable / picsPerLine;
    if (numPicsAvailable%picsPerLine != 0)
        picLines++;
    return picLines * (tvguideConfig.epgImageHeightLarge + border) + 2*border;
}

int cDetailView::heightActorPics(void) {
    int numActors = mediaInfo.actors.size();
    if (numActors < 1)
        return 0;
    if (mediaInfo.type == typeMovie) {
        actorThumbWidth = mediaInfo.actors[0].thumb.width/2;
        actorThumbHeight = mediaInfo.actors[0].thumb.height/2;
    } else if (mediaInfo.type == typeSeries) {
        actorThumbWidth = mediaInfo.actors[0].thumb.width/2;
        actorThumbHeight = mediaInfo.actors[0].thumb.height/2;
    }
    int picsPerLine = contentWidth / (actorThumbWidth + 2*border);
    int picLines = numActors / picsPerLine;
    if (numActors%picsPerLine != 0)
        picLines++;
    int actorsHeight = picLines * (actorThumbHeight + 2*fontManager.FontDetailViewSmall->Height()) + fontManager.FontDetailView->Height() + fontManager.FontDetailHeader->Height();
    return actorsHeight;
}

int cDetailView::heightFanartImg(void) {
    int retVal = 0;
    if (mediaInfo.fanart.size() >= 1) {
        int fanartWidthOrig = mediaInfo.fanart[0].width;
        int fanartHeightOrig = mediaInfo.fanart[0].height;
        int fanartWidth = fanartWidthOrig;
        int fanartHeight = fanartHeightOrig;
        retVal = fanartHeight;
        if (fanartWidthOrig > (contentWidth - 2*border)) {
            fanartWidth = contentWidth - 2*border;
            fanartHeight = fanartHeightOrig * ((double)fanartWidth / (double)fanartWidthOrig);
            retVal = fanartHeight;
        }
    }
    return retVal;
}

void cDetailView::drawEPGPictures(int height) {
    int border = 5;
    int picsPerLine = contentWidth / (tvguideConfig.epgImageWidthLarge + border);
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

void cDetailView::drawPoster(void) {
    int border = 10;
    if (mediaInfo.posters.size() < 1)
        return;
    int posterWidthOrig = mediaInfo.posters[0].width;
    int posterHeightOrig = mediaInfo.posters[0].height;
    if ((posterWidthOrig < 10) || (posterHeightOrig < 10))
        return;
    int posterWidth = posterWidthOrig;
    int posterHeight = posterHeightOrig;
    if ((posterWidthOrig > widthPoster) && (posterHeightOrig < contentHeight)) {
        posterWidth = widthPoster - 2*border;
        posterHeight = posterHeightOrig * ((double)posterWidth / (double)posterWidthOrig);
    } else if ((posterWidthOrig < widthPoster) && (posterHeightOrig > contentHeight)) {
        posterHeight = contentHeight - 2*border;
        posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
    } else if ((posterWidthOrig > widthPoster) && (posterHeightOrig > contentHeight)) {
        double ratioPoster = posterHeightOrig / posterWidthOrig;
        double ratioWindow = contentHeight / widthPoster;
        if (ratioPoster >= ratioWindow) {
            posterWidth = widthPoster - 2*border;
            posterHeight = posterHeightOrig * ((double)posterWidth / (double)posterWidthOrig);
        } else {
            posterHeight = contentHeight - 2*border;
            posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
        }
    }
    if (!Running())
        return;
    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(mediaInfo.posters[0].path.c_str(), posterWidth, posterHeight)) {
        int posterX = (widthPoster - posterWidth) / 2;
        int posterY = (contentHeight - posterHeight) / 2;
        if (Running() && pixmapPoster)
            pixmapPoster->DrawImage(cPoint(posterX, posterY), imgLoader.GetImage());
    }        
}

void cDetailView::drawBanner(int height) {
    int bannerWidthOrig = mediaInfo.banner.width;
    int bannerHeightOrig = mediaInfo.banner.height;
    int bannerWidth = bannerWidthOrig;
    int bannerHeight = bannerHeightOrig;
    
    if (bannerWidthOrig > contentWidth - 2*border) {
        bannerWidth = contentWidth - 2*border;
        bannerHeight = bannerHeightOrig * ((double)bannerWidth / (double)bannerWidthOrig);
    }
    if (!Running())
        return;
    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(mediaInfo.banner.path.c_str(), bannerWidth, bannerHeight)) {
        int bannerX = (contentWidth - bannerWidth) / 2;
        if (Running() && content)
            content->DrawImage(cPoint(bannerX, height), imgLoader.GetImage());
    }
}

void cDetailView::drawActors(int height) {
    int numActors = mediaInfo.actors.size();
    if (numActors < 1)
        return;
    tColor colorTextBack = (tvguideConfig.style == eStyleFlat)?theme.Color(clrBackground):clrTransparent;
    
    cString header = cString::sprintf("%s:", tr("Actors"));
    content->DrawText(cPoint(border, height), *header, theme.Color(clrFont), colorTextBack, fontManager.FontDetailHeader);

    int picsPerLine = contentWidth / (actorThumbWidth + 2*border);
    int picLines = numActors / picsPerLine;
    if (numActors%picsPerLine != 0)
        picLines++;
    int x = 0;
    int y = height + fontManager.FontDetailHeader->Height();
    if (!Running())
        return;
    cImageLoader imgLoader;
    int actor = 0;
    for (int row = 0; row < picLines; row++) {
        for (int col = 0; col < picsPerLine; col++) {
            if (!Running())
                return;
            if (actor == numActors)
                break;
            std::string path = mediaInfo.actors[actor].thumb.path;
            if (imgLoader.LoadPoster(path.c_str(), actorThumbWidth, actorThumbHeight)) {
                if (Running() && content)
                    content->DrawImage(cPoint(x + border, y), imgLoader.GetImage());
            }
            std::string name = mediaInfo.actors[actor].name;
            std::stringstream sstrRole;
            sstrRole << "\"" << mediaInfo.actors[actor].role << "\"";
            std::string role = sstrRole.str();
            if (fontManager.FontDetailViewSmall->Width(name.c_str()) > actorThumbWidth + 2*border)
                name = CutText(name, actorThumbWidth + 2*border, fontManager.FontDetailViewSmall);
            if (fontManager.FontDetailViewSmall->Width(role.c_str()) > actorThumbWidth + 2*border)
                role = CutText(role, actorThumbWidth + 2*border, fontManager.FontDetailViewSmall);
            int xName = x + ((actorThumbWidth+2*border) - fontManager.FontDetailViewSmall->Width(name.c_str()))/2;
            int xRole = x + ((actorThumbWidth+2*border) - fontManager.FontDetailViewSmall->Width(role.c_str()))/2;
            if (Running() && content) {
                content->DrawText(cPoint(xName, y + actorThumbHeight), name.c_str(), theme.Color(clrFont), colorTextBack, fontManager.FontDetailViewSmall);
                content->DrawText(cPoint(xRole, y + actorThumbHeight + fontManager.FontDetailViewSmall->Height()), role.c_str(), theme.Color(clrFont), colorTextBack, fontManager.FontDetailViewSmall);
                x += actorThumbWidth + 2*border;
            }
            actor++;
        }
        x = 0;
        y += actorThumbHeight + 2 * fontManager.FontDetailViewSmall->Height();
    }
}

void cDetailView::drawFanart(int height) {
    if (mediaInfo.fanart.size() < 1)
        return;
    int fanartWidthOrig = mediaInfo.fanart[0].width;
    int fanartHeightOrig = mediaInfo.fanart[0].height;
    int fanartWidth = fanartWidthOrig;
    int fanartHeight = fanartHeightOrig;
    
    if (fanartWidthOrig > contentWidth - 2*border) {
        fanartWidth = contentWidth - 2*border;
        fanartHeight = fanartHeightOrig * ((double)fanartWidth / (double)fanartWidthOrig);
    }
    if (!Running())
        return;
    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(mediaInfo.fanart[0].path.c_str(), fanartWidth, fanartHeight)) {
        int fanartX = (contentWidth - fanartWidth) / 2;
        if (Running() && content)
            content->DrawImage(cPoint(fanartX, height), imgLoader.GetImage());
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