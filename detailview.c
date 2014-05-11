#include "detailview.h"

cDetailView::cDetailView(const cEvent *event, cFooter *footer) {
    this->event = event;
    this->footer = footer;
}

cDetailView::~cDetailView(void){
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    footer->LeaveDetailedViewMode(Channels.GetByChannelID(event->ChannelID()));
    if (view)
        delete view;
}

void cDetailView::InitiateView(void) {
    static cPlugin *pScraper2Vdr = cPluginManager::GetPlugin("scraper2vdr");
    ScraperGetEventType call;
    if (!event)
        return;
    call.event = event;
    if (!pScraper2Vdr) {
        view = new cEPGView();
    } else if (pScraper2Vdr->Service("GetEventType", &call)) {
        if (call.type == tMovie) {
            view = new cMovieView(call.movieId);
        } else if (call.type == tSeries) {
            view = new cSeriesView(call.seriesId, call.episodeId);
        }
    } else {
        view = new cEPGView();                
    }
    view->SetTitle(event->Title());
    view->SetSubTitle(event->ShortText());
    view->SetInfoText(event->Description());
    cString dateTime;
    time_t vps = event->Vps();
    if (vps) {
        dateTime = cString::sprintf("%s  %s - %s (%d %s) VPS: %s", *event->GetDateString(), *event->GetTimeString(), *event->GetEndTimeString(), event->Duration()/60, tr("min"), *TimeString(vps));
    } else {
        dateTime = cString::sprintf("%s  %s - %s (%d %s)", *event->GetDateString(), *event->GetTimeString(), *event->GetEndTimeString(), event->Duration()/60, tr("min"));
    }
    view->SetDateTime(*dateTime);
    view->SetChannel(Channels.GetByChannelID(event->ChannelID(), true));
    view->SetEventID(event->EventID());
    view->SetEvent(event);
}

std::string cDetailView::LoadReruns(void) {
    if (!event)
        return "";
    
    cPlugin *epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
    if (!epgSearchPlugin)
        return "";

    if (isempty(event->Title()))
        return "";
    
    std::stringstream sstrReruns;
    sstrReruns << tr("Reruns of ") << "\"" << event->Title() << "\":" << std::endl << std::endl;

    Epgsearch_searchresults_v1_0 data;
    std::string strQuery = event->Title();

    if (tvguideConfig.displayRerunsDetailEPGView > 0) {
        if (tvguideConfig.useSubtitleRerun == 2 && !isempty(event->ShortText())) {
            strQuery += "~";
            strQuery += event->ShortText();
        }
        data.useSubTitle = true;
    } else {
        data.useSubTitle = false;
    }
    data.query = (char *)strQuery.c_str();
    data.mode = 0;
    data.channelNr = 0;
    data.useTitle = true;
    data.useDescription = false;

    bool foundRerun = false;
    if (epgSearchPlugin->Service("Epgsearch-searchresults-v1.0", &data)) {
        cList<Epgsearch_searchresults_v1_0::cServiceSearchResult>* list = data.pResultList;
        if (list && (list->Count() > 1)) {
            foundRerun = true;
            int i = 0;
            for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = list->First(); r && i < tvguideConfig.numReruns; r = list->Next(r)) {
                if ((event->ChannelID() == r->event->ChannelID()) && (event->StartTime() == r->event->StartTime()))
                    continue;
                i++;
                sstrReruns  << *DayDateTime(r->event->StartTime());
                cChannel *channel = Channels.GetByChannelID(r->event->ChannelID(), true, true);
                if (channel) {
                    sstrReruns << ", " << trVDR("Channel") << " " << channel->Number() << ":";
                    sstrReruns << " " << channel->ShortName(true);
                }
                sstrReruns << "\n" << r->event->Title();
                if (!isempty(r->event->ShortText()))
                    sstrReruns << "~" << r->event->ShortText();
                sstrReruns << std::endl << std::endl;
            }
            delete list;
        }
    }

    if (!foundRerun) {
        sstrReruns << std::endl << tr("No reruns found");        
    }
    return sstrReruns.str();
}


void cDetailView::Action(void) {
    InitiateView();
    if (!view)
        return;
    view->SetFonts();
    view->SetGeometry();
    view->LoadMedia();
    view->Start();
    if (event)
        view->SetAdditionalInfoText(LoadReruns());
}

eOSState cDetailView::ProcessKey(eKeys Key) {
    eOSState state = osContinue;
    if (Running())
        return state;
    switch (Key & ~k_Repeat) {
        case kUp: {
            bool scrolled = view->KeyUp();
            if (scrolled) {
                view->DrawScrollbar();
                osdManager.flush();
            }
            break; }
        case kDown: {
            bool scrolled = view->KeyDown();
            if (scrolled) {
                view->DrawScrollbar();
                osdManager.flush();
            }
            break; }
        case kLeft:
            view->KeyLeft();
            view->Start();
            break;
        case kRight:
            view->KeyRight();
            view->Start();
            break;
        case kOk:
        case kBack:
            state = osEnd;
            break;
    }
    return state;
}