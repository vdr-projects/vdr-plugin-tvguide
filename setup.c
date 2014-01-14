#include "setup.h"

cTvguideSetup::cTvguideSetup() {
    tmpTvguideConfig = tvguideConfig;
    Setup();
}

cTvguideSetup::~cTvguideSetup() {
    geoManager.SetGeometry(cOsd::OsdWidth(), cOsd::OsdHeight(), true);
    fontManager.DeleteFonts();
    fontManager.SetFonts();
    tvguideConfig.LoadTheme();
    tvguideConfig.setDynamicValues();
    imgCache.Clear();
    imgCache.CreateCache();
}


void cTvguideSetup::Setup(void) {
    int currentItem = Current();
    Clear();
    
    Add(new cOsdItem(tr("General Settings")));
    Add(new cOsdItem(tr("Screen Presentation")));
    Add(new cOsdItem(tr("Fonts and Fontsizes")));
    Add(new cOsdItem(tr("Image Loading and Caching")));

    SetCurrent(Get(currentItem));
    Display();
}

eOSState cTvguideSetup::ProcessKey(eKeys Key) {
    bool hadSubMenu = HasSubMenu();   
    eOSState state = cMenuSetupPage::ProcessKey(Key);
    if (hadSubMenu && Key == kOk)
        Store();
        
    if (!hadSubMenu && (state == osUnknown || Key == kOk)) {
        if ((Key == kOk && !hadSubMenu)) {
            const char* ItemText = Get(Current())->Text();
            if (strcmp(ItemText, tr("General Settings")) == 0)
                state = AddSubMenu(new cMenuSetupGeneral(&tmpTvguideConfig));
            if (strcmp(ItemText, tr("Screen Presentation")) == 0)
                state = AddSubMenu(new cMenuSetupScreenLayout(&tmpTvguideConfig));
            if (strcmp(ItemText, tr("Fonts and Fontsizes")) == 0)
                state = AddSubMenu(new cMenuSetupFont(&tmpTvguideConfig));
            if (strcmp(ItemText, tr("Image Loading and Caching")) == 0)
                state = AddSubMenu(new cMenuSetupImageCache(&tmpTvguideConfig));  
        }
    }   
    return state;
}

void cTvguideSetup::Store(void) {

    tvguideConfig = tmpTvguideConfig;
    SetupStore("debugImageLoading", tvguideConfig.debugImageLoading);
    SetupStore("useNopacityTheme", tvguideConfig.useNopacityTheme);
    SetupStore("themeIndex", tvguideConfig.themeIndex);
    SetupStore("showMainMenuEntry", tvguideConfig.showMainMenuEntry);
    SetupStore("replaceOriginalSchedule", tvguideConfig.replaceOriginalSchedule);
    SetupStore("displayMode", tvguideConfig.displayMode);
    SetupStore("showTimeInGrid", tvguideConfig.showTimeInGrid);
    SetupStore("displayStatusHeader", tvguideConfig.displayStatusHeader);
    SetupStore("displayChannelGroups", tvguideConfig.displayChannelGroups);
    SetupStore("headerHeightPercent", tvguideConfig.headerHeightPercent);
    SetupStore("channelGroupsPercent", tvguideConfig.channelGroupsPercent);
    SetupStore("epgViewBorder", tvguideConfig.epgViewBorder);
    SetupStore("scaleVideo", tvguideConfig.scaleVideo);
    SetupStore("decorateVideo", tvguideConfig.decorateVideo);
    SetupStore("roundedCorners", tvguideConfig.roundedCorners);
    SetupStore("timeFormat", tvguideConfig.timeFormat);
    SetupStore("channelCols", tvguideConfig.channelCols);
    SetupStore("channelRows", tvguideConfig.channelRows);
    SetupStore("displayTime", tvguideConfig.displayTime);
    SetupStore("bigStepHours", tvguideConfig.bigStepHours);
    SetupStore("hugeStepHours", tvguideConfig.hugeStepHours);
    SetupStore("channelJumpMode", tvguideConfig.channelJumpMode);
    SetupStore("blueKeyMode", tvguideConfig.blueKeyMode);
    SetupStore("numkeyMode", tvguideConfig.numkeyMode);
    SetupStore("useRemoteTimers", tvguideConfig.useRemoteTimers);
    SetupStore("closeOnSwitch", tvguideConfig.closeOnSwitch);
    SetupStore("hideLastGroup", tvguideConfig.hideLastGroup);
    SetupStore("hideChannelLogos", tvguideConfig.hideChannelLogos);
    SetupStore("logoExtension", tvguideConfig.logoExtension);
    SetupStore("logoWidthRatio", tvguideConfig.logoWidthRatio);
    SetupStore("logoHeightRatio", tvguideConfig.logoHeightRatio);
    SetupStore("hideEpgImages", tvguideConfig.hideEpgImages);
    SetupStore("epgImageWidth", tvguideConfig.epgImageWidth);
    SetupStore("epgImageHeight", tvguideConfig.epgImageHeight);
    SetupStore("numAdditionalEPGPictures", tvguideConfig.numAdditionalEPGPictures);
    SetupStore("epgImageWidthLarge", tvguideConfig.epgImageWidthLarge);
    SetupStore("epgImageHeightLarge", tvguideConfig.epgImageHeightLarge);
    SetupStore("timeLineWidthPercent", tvguideConfig.timeLineWidthPercent);
    SetupStore("timeLineHeightPercent", tvguideConfig.timeLineHeightPercent);
    SetupStore("displayChannelName", tvguideConfig.displayChannelName);
    SetupStore("channelHeaderWidthPercent", tvguideConfig.channelHeaderWidthPercent);
    SetupStore("channelHeaderHeightPercent", tvguideConfig.channelHeaderHeightPercent);
    SetupStore("footerHeightPercent", tvguideConfig.footerHeightPercent);
    SetupStore("recMenuAskFolder", tvguideConfig.recMenuAskFolder);
    SetupStore("fontIndex", tvguideConfig.fontIndex);
    SetupStore("FontButtonDelta", tvguideConfig.FontButtonDelta);
    SetupStore("FontDetailViewDelta", tvguideConfig.FontDetailViewDelta);
    SetupStore("FontDetailHeaderDelta", tvguideConfig.FontDetailHeaderDelta);
    SetupStore("FontMessageBoxDelta", tvguideConfig.FontMessageBoxDelta);
    SetupStore("FontMessageBoxLargeDelta", tvguideConfig.FontMessageBoxLargeDelta);
    SetupStore("FontStatusHeaderDelta", tvguideConfig.FontStatusHeaderDelta);
    SetupStore("FontStatusHeaderLargeDelta", tvguideConfig.FontStatusHeaderLargeDelta);
    SetupStore("FontChannelHeaderDelta", tvguideConfig.FontChannelHeaderDelta);
    SetupStore("FontChannelGroupsDelta", tvguideConfig.FontChannelGroupsDelta);
    SetupStore("FontGridDelta", tvguideConfig.FontGridDelta);
    SetupStore("FontGridSmallDelta", tvguideConfig.FontGridSmallDelta);
    SetupStore("FontTimeLineWeekdayDelta", tvguideConfig.FontTimeLineWeekdayDelta);
    SetupStore("FontTimeLineDateDelta", tvguideConfig.FontTimeLineDateDelta);
    SetupStore("FontTimeLineTimeDelta", tvguideConfig.FontTimeLineTimeDelta);
    SetupStore("FontChannelHeaderHorizontalDelta", tvguideConfig.FontChannelHeaderHorizontalDelta);
    SetupStore("FontChannelGroupsHorizontalDelta", tvguideConfig.FontChannelGroupsHorizontalDelta);
    SetupStore("FontGridHorizontalDelta", tvguideConfig.FontGridHorizontalDelta);
    SetupStore("FontGridHorizontalSmallDelta", tvguideConfig.FontGridHorizontalSmallDelta);
    SetupStore("FontTimeLineDateHorizontalDelta", tvguideConfig.FontTimeLineDateHorizontalDelta);
    SetupStore("FontTimeLineTimeHorizontalDelta", tvguideConfig.FontTimeLineTimeHorizontalDelta);
    SetupStore("FontRecMenuItemDelta", tvguideConfig.FontRecMenuItemDelta);
    SetupStore("FontRecMenuItemSmallDelta", tvguideConfig.FontRecMenuItemSmallDelta);
    SetupStore("FontRecMenuItemLargeDelta", tvguideConfig.FontRecMenuItemLargeDelta);
    SetupStore("displayRerunsDetailEPGView", tvguideConfig.displayRerunsDetailEPGView);
    SetupStore("numReruns", tvguideConfig.numReruns);
    SetupStore("useSubtitleRerun", tvguideConfig.useSubtitleRerun);
    SetupStore("numLogosInitial", tvguideConfig.numLogosInitial);
    SetupStore("numLogosMax", tvguideConfig.numLogosMax);
    SetupStore("limitLogoCache", tvguideConfig.limitLogoCache);
}

cMenuSetupSubMenu::cMenuSetupSubMenu(const char* Title, cTvguideConfig* data) : cOsdMenu(Title, 30) {
    tmpTvguideConfig = data;
    indent = "    ";
}

cOsdItem *cMenuSetupSubMenu::InfoItem(const char *label, const char *value) {
    cOsdItem *item;
    item = new cOsdItem(cString::sprintf("%s: %s", label, value));
    item->SetSelectable(false);
    return item;
}

eOSState cMenuSetupSubMenu::ProcessKey(eKeys Key) {
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
    switch (Key) {
      case kOk:
        return osBack;
      default:
        break;
    }
  }
  return state;
}

//------------------------------------------------------------------------------------------------------------------

cMenuSetupGeneral::cMenuSetupGeneral(cTvguideConfig* data)  : cMenuSetupSubMenu(tr("General Settings"), data) {
    themes.Load(*cString("tvguide"));
    timeFormatItems[0] = "12h";
    timeFormatItems[1] = "24h";
    jumpMode[0] = tr("x channels back / forward");
    jumpMode[1] = tr("previous / next channel group");
    blueMode[0] = tr("Blue: Channel Switch, Ok: Detailed EPG");
    blueMode[1] = tr("Blue: Detailed EPG, Ok: Channel Switch");
    blueMode[2] = tr("Blue: Favorites / Switch, Ok: Detailed EPG");
    numMode[0] = tr("Timely Jump");
    numMode[1] = tr("Jump to specific channel");
    useSubtitleRerunTexts[0] = tr("never");
    useSubtitleRerunTexts[1] = tr("if exists");
    useSubtitleRerunTexts[2] = tr("always");
    Set();
}

void cMenuSetupGeneral::Set(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditBoolItem(tr("Show Main Menu Entry"), &tmpTvguideConfig->showMainMenuEntry));
    Add(new cMenuEditBoolItem(tr("Replace VDR Schedules Menu"), &tmpTvguideConfig->replaceOriginalSchedule));
    Add(new cMenuEditBoolItem(tr("Use appropriate nOpacity Theme"), &tmpTvguideConfig->useNopacityTheme));
    if (!tmpTvguideConfig->useNopacityTheme) {
        if (themes.NumThemes())
            Add(new cMenuEditStraItem(cString::sprintf("%s%s", *indent, tr("Theme")), &tmpTvguideConfig->themeIndex, themes.NumThemes(), themes.Descriptions()));
    }
    Add(new cMenuEditIntItem(tr("Time to display in minutes"), &tmpTvguideConfig->displayTime, 120, 320));
    Add(new cMenuEditBoolItem(tr("Rounded Corners"), &tmpTvguideConfig->roundedCorners));
    
    Add(new cMenuEditStraItem(tr("Channel Jump Mode (Keys Green / Yellow)"), &tmpTvguideConfig->channelJumpMode, 2,  jumpMode));
    Add(new cMenuEditStraItem(tr("Keys Blue and OK"), &tmpTvguideConfig->blueKeyMode, 3,  blueMode));
    Add(new cMenuEditBoolItem(tr("Close TVGuide after channel switch"), &tmpTvguideConfig->closeOnSwitch));
    Add(new cMenuEditStraItem(tr("Functionality of numeric Keys"), &tmpTvguideConfig->numkeyMode, 2,  numMode));
    Add(new cMenuEditBoolItem(tr("Hide last Channel Group"), &tmpTvguideConfig->hideLastGroup));
    Add(new cMenuEditIntItem(tr("Big Step (Keys 1 / 3) in hours"), &tmpTvguideConfig->bigStepHours, 1, 12));
    Add(new cMenuEditIntItem(tr("Huge Step (Keys 4 / 6) in hours"), &tmpTvguideConfig->hugeStepHours, 13, 48));
    Add(new cMenuEditStraItem(tr("Time Format (12h/24h)"), &tmpTvguideConfig->timeFormat, 2,  timeFormatItems));
    Add(new cMenuEditBoolItem(tr("Use folders for instant records"), &tmpTvguideConfig->recMenuAskFolder));
    if (pRemoteTimers)
        Add(new cMenuEditBoolItem(tr("Use Remotetimers"), &tmpTvguideConfig->useRemoteTimers));
    Add(new cMenuEditBoolItem(tr("Display Reruns in detailed EPG View"), &tmpTvguideConfig->displayRerunsDetailEPGView));
    if (tmpTvguideConfig->displayRerunsDetailEPGView) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *indent, tr("Number of reruns to display")), &tmpTvguideConfig->numReruns, 1, 10));
        Add(new cMenuEditStraItem(cString::sprintf("%s%s", *indent, tr("Use Subtitle for reruns")), &tmpTvguideConfig->useSubtitleRerun, 3, useSubtitleRerunTexts));
    }
    SetCurrent(Get(currentItem));
    Display();
}

eOSState cMenuSetupGeneral::ProcessKey(eKeys Key) {
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (Key == kOk) {
        state = osBack;
    } else if (Key != kNone) {
        Set();
    }
    return state;
}

//------------------------------------------------------------------------------------------------------------------

cMenuSetupScreenLayout::cMenuSetupScreenLayout(cTvguideConfig* data)  : cMenuSetupSubMenu(tr("Screen Presentation"), data) {
    displayModeItems[0] = "vertical";
    displayModeItems[1] = "horizontal";
    hideChannelLogosItems[0] = trVDR("yes");
    hideChannelLogosItems[1] = trVDR("no");
    logoExtensionItems[0] = "png";
    logoExtensionItems[1] = "jpg";
    Set();
}

void cMenuSetupScreenLayout::Set(void) {
    int currentItem = Current();
    Clear();
    
    Add(new cMenuEditStraItem(tr("Display Mode"), &tmpTvguideConfig->displayMode, 2,  displayModeItems));
    if (tmpTvguideConfig->displayMode == eVertical) {
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Height of Channel Header (Perc. of osd height)")), &tmpTvguideConfig->channelHeaderHeightPercent, 5, 30));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Width of Timeline (Perc. of osd width)")), &tmpTvguideConfig->timeLineWidthPercent, 5, 30));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Number of Channels to display")), &tmpTvguideConfig->channelCols, 3, 12));
    } else if (tmpTvguideConfig->displayMode == eHorizontal) {
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Width of Channel Header (Perc. of osd width)")), &tmpTvguideConfig->channelHeaderWidthPercent, 5, 30));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Height of Timeline (Perc. of osd height)")), &tmpTvguideConfig->timeLineHeightPercent, 5, 30));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Number of Channels to display")), &tmpTvguideConfig->channelRows, 3, 12));
        Add(new cMenuEditBoolItem(*cString::sprintf("%s%s", *indent, tr("Display time in EPG Grids")), &tmpTvguideConfig->showTimeInGrid));
    }
    Add(new cMenuEditIntItem(tr("Height of Headers (Status Header and EPG View, Perc. of osd height)"), &tmpTvguideConfig->headerHeightPercent, 10, 50));
    Add(new cMenuEditIntItem(tr("Height of Footer (Perc. of osd height)"), &tmpTvguideConfig->footerHeightPercent, 3, 20));
    
    Add(new cMenuEditBoolItem(tr("Display status header"), &tmpTvguideConfig->displayStatusHeader));
    if (tmpTvguideConfig->displayStatusHeader) {
        Add(new cMenuEditBoolItem(*cString::sprintf("%s%s", *indent, tr("Scale video to upper right corner")), &tmpTvguideConfig->scaleVideo));
        Add(new cMenuEditBoolItem(*cString::sprintf("%s%s", *indent, tr("Rounded corners around video frame")), &tmpTvguideConfig->decorateVideo));
    }
    
    Add(new cMenuEditBoolItem(tr("Display Channel Names in Header"), &tmpTvguideConfig->displayChannelName));
    Add(new cMenuEditBoolItem(tr("Display channel groups"), &tmpTvguideConfig->displayChannelGroups));
    if (tmpTvguideConfig->displayChannelGroups) {
        if (tmpTvguideConfig->displayMode == eVertical) {
            Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Height of channel groups (Perc. of osd height)")), &tmpTvguideConfig->channelGroupsPercent, 3, 30));
        } else if (tmpTvguideConfig->displayMode == eHorizontal) {
            Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Width of channel groups (Perc. of osd width)")), &tmpTvguideConfig->channelGroupsPercent, 3, 30));
        }
    }
    
    Add(new cMenuEditStraItem(tr("Show Channel Logos"), &tmpTvguideConfig->hideChannelLogos, 2,  hideChannelLogosItems));   
    if (!tmpTvguideConfig->hideChannelLogos) {
        Add(InfoItem(tr("Logo Path used"), *tvguideConfig.logoPath));
        Add(new cMenuEditStraItem(*cString::sprintf("%s%s", *indent, tr("Logo Extension")), &tmpTvguideConfig->logoExtension, 2,  logoExtensionItems));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Logo width ratio")), &tmpTvguideConfig->logoWidthRatio, 1, 1000));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Logo height ratio")), &tmpTvguideConfig->logoHeightRatio, 1, 1000));
    }
    
    Add(new cMenuEditIntItem(tr("Text Border in Detailed View (pixel)"), &tmpTvguideConfig->epgViewBorder, 0, 300));
    
    Add(new cMenuEditStraItem(tr("Show EPG Images"), &tmpTvguideConfig->hideEpgImages, 2,  hideChannelLogosItems)); 
    if (!tmpTvguideConfig->hideEpgImages) {
        Add(InfoItem(tr("EPG Images Path used"), *tvguideConfig.epgImagePath));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("EPG Image width")), &tmpTvguideConfig->epgImageWidth, 0, 800));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("EPG Image height")), &tmpTvguideConfig->epgImageHeight, 0, 800));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Number of additional EPG Images")), &tmpTvguideConfig->numAdditionalEPGPictures, 0, 20));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Additional EPG Image width")), &tmpTvguideConfig->epgImageWidthLarge, 1, 800));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", *indent, tr("Additional EPG Image height")), &tmpTvguideConfig->epgImageHeightLarge, 0, 800));
    }
    
    SetCurrent(Get(currentItem));
    Display();
}

eOSState cMenuSetupScreenLayout::ProcessKey(eKeys Key) {
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (Key == kOk) {
        state = osBack;
    } else if (Key != kNone) {
        Set();
    }
    return state;
}

//------------------------------------------------------------------------------------------------------------------

cMenuSetupFont::cMenuSetupFont(cTvguideConfig* data)  : cMenuSetupSubMenu(tr("Fonts and Fontsizes"), data) {
    cFont::GetAvailableFontNames(&fontNames);
    fontNames.Insert(strdup(tvguideConfig.fontNameDefault));
    Set();
}

void cMenuSetupFont::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditStraItem(tr("Font"), &tmpTvguideConfig->fontIndex, fontNames.Size(), &fontNames[0]));
    
    Add(new cMenuEditIntItem(tr("Status Header Font Size"), &tmpTvguideConfig->FontStatusHeaderDelta, -30, 30));    
    Add(new cMenuEditIntItem(tr("Status Header Large Font Size"), &tmpTvguideConfig->FontStatusHeaderLargeDelta, -30, 30));
    Add(new cMenuEditIntItem(tr("Detail EPG View Font Size"), &tmpTvguideConfig->FontDetailViewDelta, -30, 30));
    Add(new cMenuEditIntItem(tr("Detail EPG View Header Font Size"), &tmpTvguideConfig->FontDetailHeaderDelta, -30, 30));
    Add(new cMenuEditIntItem(tr("Message Font Size"), &tmpTvguideConfig->FontMessageBoxDelta, -30, 30));
    Add(new cMenuEditIntItem(tr("Message Large Font Size"), &tmpTvguideConfig->FontMessageBoxLargeDelta, -30, 30));
    Add(new cMenuEditIntItem(tr("Button Font Size"), &tmpTvguideConfig->FontButtonDelta, -30, 30));


    if (tmpTvguideConfig->displayMode == eVertical) {
        Add(new cMenuEditIntItem(tr("Channel Header Font Size"), &tmpTvguideConfig->FontChannelHeaderDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Channel Groups Font Size"), &tmpTvguideConfig->FontChannelGroupsDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Grid Font Size"), &tmpTvguideConfig->FontGridDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Grid Font Small Size"), &tmpTvguideConfig->FontGridSmallDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Timeline Weekday Font Size"), &tmpTvguideConfig->FontTimeLineWeekdayDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Timeline Date Font Size"), &tmpTvguideConfig->FontTimeLineDateDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Timeline Time Font Size"), &tmpTvguideConfig->FontTimeLineTimeDelta, -30, 30));
    } else if (tmpTvguideConfig->displayMode == eHorizontal) {
        Add(new cMenuEditIntItem(tr("Channel Header Font Size"), &tmpTvguideConfig->FontChannelHeaderHorizontalDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Channel Groups Font Size"), &tmpTvguideConfig->FontChannelGroupsHorizontalDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Grid Font Size"), &tmpTvguideConfig->FontGridHorizontalDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Grid Font Small Size"), &tmpTvguideConfig->FontGridHorizontalSmallDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Timeline Date Font Size"), &tmpTvguideConfig->FontTimeLineDateHorizontalDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Timeline Time Font Size"), &tmpTvguideConfig->FontTimeLineTimeHorizontalDelta, -30, 30));
    }
    
    Add(new cMenuEditIntItem(tr("Search & Recording Menu Font Size"), &tmpTvguideConfig->FontRecMenuItemDelta, -30, 30));
    Add(new cMenuEditIntItem(tr("Search & Recording Menu Small Font Size"), &tmpTvguideConfig->FontRecMenuItemSmallDelta, -30, 30));
    Add(new cMenuEditIntItem(tr("Search & Recording Menu Header Font Size"), &tmpTvguideConfig->FontRecMenuItemLargeDelta, -30, 30));

    SetCurrent(Get(currentItem));
    Display();
}

//-----Image Caching-------------------------------------------------------------------------------------------------------------
cMenuSetupImageCache::cMenuSetupImageCache(cTvguideConfig* data)  : cMenuSetupSubMenu(tr("Image Loading and Caching"), data) {
    Set();
}

void cMenuSetupImageCache::Set(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditBoolItem(tr("Create Log Messages for image loading"), &tmpTvguideConfig->debugImageLoading));
    Add(new cMenuEditBoolItem(tr("Limit Logo Cache"), &tmpTvguideConfig->limitLogoCache));
    if (&tmpTvguideConfig->limitLogoCache) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *indent, tr("Maximal number of logos to cache")), &tmpTvguideConfig->numLogosMax, 1, 9999));
    }
    Add(new cMenuEditIntItem(tr("Number of  logos to cache at start"), &tmpTvguideConfig->numLogosInitial, 0, 9999));

    Add(InfoItem(tr("Cache Sizes"), ""));
    Add(InfoItem(tr("OSD Element Cache"), (imgCache.GetCacheSize(ctOsdElement)).c_str()));
    Add(InfoItem(tr("Logo cache"), (imgCache.GetCacheSize(ctLogo)).c_str()));
    Add(InfoItem(tr("EPG Grid Cache"), (imgCache.GetCacheSize(ctGrid)).c_str()));
    Add(InfoItem(tr("Channel Groups Cache"), (imgCache.GetCacheSize(ctChannelGroup)).c_str()));
    Add(InfoItem(tr("Recording Menus Icon Cache"), (imgCache.GetCacheSize(ctIcon)).c_str()));

    SetCurrent(Get(currentItem));
    Display();
}
