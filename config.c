#include "config.h"

enum {
    e12Hours,
    e24Hours
};

enum {
    eVertical,
    eHorizontal
};

enum {
    eNumJump,
    eGroupJump
};

cTvguideConfig::cTvguideConfig() {
    showMainMenuEntry = 1;
    osdWidth = 0; 
    osdHeight = 0;
    displayMode = eHorizontal;
    colWidth = 0;
    rowHeight = 0;
    channelCols = 5;
    channelRows = 10;
    displayTime = 160;
    minutePixel = 0;
    displayStatusHeader = 1;
    displayChannelGroups = 1;
    statusHeaderPercent = 20;
    statusHeaderHeight = 0;
    channelGroupsPercent = 5;
    channelGroupsWidth = 0;
    channelGroupsHeight = 0;
    scaleVideo = 1;
    decorateVideo = 1;
    timeLineWidthPercent = 8;
    timeLineHeightPercent = 4;
    displayChannelName = 1;
    channelHeaderWidthPercent = 20;
    channelHeaderHeightPercent = 15;
    footerHeight = 80;
    stepMinutes = 30;
    bigStepHours = 3;
    hugeStepHours = 24;
    channelJumpMode = eNumJump;
    jumpChannels = 10;
    hideLastGroup = 0;
    hideChannelLogos = 0;
    logoWidthRatio = 13;
    logoHeightRatio = 10;
    logoExtension = 0;
    hideEpgImages = 0;
    epgImageWidth = 315;
    epgImageHeight = 240;
    fontIndex = 0;
    fontNameDefault = "VDRSymbols Sans:Book";
    FontButtonDelta = 0;
    FontDetailViewDelta = 0;
    FontDetailHeaderDelta = 0;
    FontMessageBoxDelta = 0;
    FontMessageBoxLargeDelta = 0;
    FontStatusHeaderDelta = 0;
    FontStatusHeaderLargeDelta = 0;
    FontChannelHeaderDelta = 0;
    FontChannelGroupsDelta = 0;
    FontGridDelta = 0;
    FontGridSmallDelta = 0;
    FontTimeLineWeekdayDelta = 0;
    FontTimeLineDateDelta = 0;
    FontTimeLineTimeDelta = 0;
    FontChannelHeaderHorizontalDelta = 0;
    FontChannelGroupsHorizontalDelta = 0;
    FontGridHorizontalDelta = 0;
    FontGridHorizontalSmallDelta = 0;
    FontTimeLineDateHorizontalDelta = 0;
    FontTimeLineTimeHorizontalDelta = 0;
    //Common Fonts
    FontButton = NULL;
    FontDetailView = NULL;
    FontDetailHeader = NULL;
    FontMessageBox = NULL;
    FontMessageBoxLarge = NULL;
    FontStatusHeader = NULL;
    FontStatusHeaderLarge = NULL;
    //Fonts for vertical Display 
    FontChannelHeader = NULL;
    FontGrid = NULL;
    FontGridSmall = NULL;
    FontTimeLineWeekday = NULL;
    FontTimeLineDate = NULL;
    FontTimeLineTime = NULL;
    //Fonts for horizontal Display 
    FontChannelHeaderHorizontal = NULL;
    FontGridHorizontal = NULL;
    FontGridHorizontalSmall = NULL;
    FontTimeLineDateHorizontal = NULL;
    FontTimeLineTimeHorizontal = NULL;

    timeFormat = 1;
    themeIndex = 4;
    useBlending = 2;
    roundedCorners = 0;
    displayRerunsDetailEPGView = 1;
    numReruns = 5;
    useSubtitleRerun = 1;
}

cTvguideConfig::~cTvguideConfig() {
    delete FontButton;
    delete FontDetailView;
    delete FontDetailHeader;
    delete FontMessageBox;
    delete FontMessageBoxLarge;
    delete FontStatusHeader;
    delete FontStatusHeaderLarge;
    delete FontChannelHeader;
    delete FontChannelGroups;
    delete FontGrid;
    delete FontGridSmall;
    delete FontTimeLineWeekday;
    delete FontTimeLineDate;
    delete FontTimeLineTime;
    delete FontChannelHeaderHorizontal;
    delete FontChannelGroupsHorizontal;
    delete FontGridHorizontal;
    delete FontGridHorizontalSmall;
    delete FontTimeLineDateHorizontal;
    delete FontTimeLineTimeHorizontal;
}

void cTvguideConfig::setDynamicValues(int width, int height) {
    SetGeometry(width, height);
    SetFonts();
}

void cTvguideConfig::SetGeometry(int width, int height) {
    osdWidth = width;
    osdHeight = height;
    statusHeaderHeight = (displayStatusHeader)?(statusHeaderPercent * osdHeight / 100):0;
    channelGroupsWidth = (displayChannelGroups)?(channelGroupsPercent * osdWidth / 100):0;
    channelGroupsHeight = (displayChannelGroups)?(channelGroupsPercent * osdHeight / 100):0;
    channelHeaderWidth = channelHeaderWidthPercent * osdWidth / 100;
    channelHeaderHeight = channelHeaderHeightPercent * osdHeight / 100;
    timeLineWidth = timeLineWidthPercent * osdWidth / 100;
    timeLineHeight = timeLineHeightPercent * osdHeight / 100;

    if (displayMode == eVertical) {
        colWidth = (osdWidth - timeLineWidth) / channelCols;
        rowHeight = 0;
        minutePixel = (osdHeight - statusHeaderHeight - channelGroupsHeight - channelHeaderHeight - footerHeight) / displayTime;
    } else if (displayMode == eHorizontal) {
        colWidth = 0;
        rowHeight = (osdHeight - statusHeaderHeight - timeLineHeight - footerHeight) / channelRows;
        minutePixel = (osdWidth - channelHeaderWidth - channelGroupsWidth) / displayTime;
    }
    
    numGrids = (displayMode == eVertical)?channelCols:channelRows;
}

void cTvguideConfig::SetFonts(void){
    cString fontname;
    if (fontIndex == 0) {
        fontname = fontNameDefault;
    } else {
        cStringList availableFonts;
        cFont::GetAvailableFontNames(&availableFonts);
        if (availableFonts[fontIndex-1]) {
            fontname = availableFonts[fontIndex-1];
        } else 
            fontname = fontNameDefault;
    }
    cFont *test = NULL;
    test = cFont::CreateFont(*fontname, 30);
    if (!test) {
        fontname = DefaultFontSml;
    }
    delete test;
        
    //Common Fonts
    FontButton = cFont::CreateFont(*fontname, footerHeight/3 + 4 + FontButtonDelta);
    FontDetailView = cFont::CreateFont(*fontname, osdHeight/30 + FontDetailViewDelta);
    FontDetailHeader = cFont::CreateFont(*fontname, osdHeight/25 + FontDetailHeaderDelta);
    FontMessageBox = cFont::CreateFont(*fontname, osdHeight/33 + FontMessageBoxDelta);
    FontMessageBoxLarge = cFont::CreateFont(*fontname, osdHeight/30 + FontMessageBoxLargeDelta);    
    FontStatusHeader = cFont::CreateFont(*fontname, statusHeaderHeight/6 - 4 + FontStatusHeaderDelta);
    FontStatusHeaderLarge = cFont::CreateFont(*fontname, statusHeaderHeight/5 + FontStatusHeaderLargeDelta);
    //Fonts for vertical Display 
    FontChannelHeader = cFont::CreateFont(*fontname, colWidth/10 + FontChannelHeaderDelta);
    FontChannelGroups = cFont::CreateFont(*fontname, colWidth/8 + FontChannelGroupsDelta);
    FontGrid = cFont::CreateFont(*fontname, colWidth/12 + FontGridDelta);
    FontGridSmall = cFont::CreateFont(*fontname, colWidth/12 + FontGridSmallDelta);
    FontTimeLineWeekday = cFont::CreateFont(*fontname, timeLineWidth/3 + FontTimeLineWeekdayDelta);
    FontTimeLineDate = cFont::CreateFont(*fontname, timeLineWidth/4 + FontTimeLineDateDelta);
    FontTimeLineTime = cFont::CreateFont(*fontname, timeLineWidth/4 + FontTimeLineTimeDelta);
    //Fonts for horizontal Display 
    FontChannelHeaderHorizontal = cFont::CreateFont(*fontname, rowHeight/3 + FontChannelHeaderHorizontalDelta);
    FontChannelGroupsHorizontal = cFont::CreateFont(*fontname, rowHeight/3 + 5 + FontChannelGroupsHorizontalDelta);
    FontGridHorizontal = cFont::CreateFont(*fontname, rowHeight/3 + 5 + FontGridHorizontalDelta);
    FontGridHorizontalSmall = cFont::CreateFont(*fontname, rowHeight/4 + FontGridHorizontalSmallDelta);
    FontTimeLineDateHorizontal = cFont::CreateFont(*fontname, timeLineHeight/2 + 5 + FontTimeLineDateHorizontalDelta);
    FontTimeLineTimeHorizontal = cFont::CreateFont(*fontname, timeLineHeight/2 + FontTimeLineTimeHorizontalDelta);

}

void cTvguideConfig::SetBlending(void) {
    if (theme.Color(clrDoBlending) == CLR_BLENDING_OFF) {
        useBlending = 0;
    } else if (theme.Color(clrDoBlending) == CLR_BLENDING_DEFAULT)
        useBlending = 1;
    else {
        useBlending = 2;
    }
}

void cTvguideConfig::SetLogoPath(cString path) {
    logoPath = path;
}

void cTvguideConfig::SetImagesPath(cString path) {
    epgImagePath = path;
}

void cTvguideConfig::loadTheme() {
    cThemes themes;
    themes.Load(*cString("tvguide"));
    const char *FileName = themes.FileName(themeIndex);
    if (access(FileName, F_OK) == 0) {
        ::theme.Load(FileName);
    }
}

bool cTvguideConfig::SetupParse(const char *Name, const char *Value) {
    if      (strcmp(Name, "timeFormat") == 0)               timeFormat = atoi(Value);
    else if (strcmp(Name, "showMainMenuEntry") == 0)        showMainMenuEntry = atoi(Value);
    else if (strcmp(Name, "themeIndex") == 0)               themeIndex = atoi(Value);
    else if (strcmp(Name, "displayMode") == 0)              displayMode = atoi(Value);
    else if (strcmp(Name, "displayStatusHeader") == 0)      displayStatusHeader = atoi(Value);
    else if (strcmp(Name, "displayChannelGroups") == 0)     displayChannelGroups = atoi(Value);
    else if (strcmp(Name, "statusHeaderPercent") == 0)      statusHeaderPercent = atoi(Value);
    else if (strcmp(Name, "channelGroupsPercent") == 0)     channelGroupsPercent = atoi(Value);
    else if (strcmp(Name, "scaleVideo") == 0)               scaleVideo = atoi(Value);
    else if (strcmp(Name, "decorateVideo") == 0)            decorateVideo = atoi(Value);
    else if (strcmp(Name, "roundedCorners") == 0)           roundedCorners = atoi(Value);
    else if (strcmp(Name, "channelCols") == 0)              channelCols = atoi(Value);
    else if (strcmp(Name, "channelRows") == 0)              channelRows = atoi(Value);
    else if (strcmp(Name, "displayTime") == 0)              displayTime = atoi(Value);
    else if (strcmp(Name, "hideChannelLogos") == 0)         hideChannelLogos = atoi(Value);
    else if (strcmp(Name, "logoExtension") == 0)            logoExtension = atoi(Value);
    else if (strcmp(Name, "logoWidthRatio") == 0)           logoWidthRatio = atoi(Value);
    else if (strcmp(Name, "logoHeightRatio") == 0)          logoHeightRatio = atoi(Value);
    else if (strcmp(Name, "bigStepHours") == 0)             bigStepHours = atoi(Value);
    else if (strcmp(Name, "hugeStepHours") == 0)            hugeStepHours = atoi(Value);
    else if (strcmp(Name, "channelJumpMode") == 0)          channelJumpMode = atoi(Value);
    else if (strcmp(Name, "jumpChannels") == 0)             jumpChannels = atoi(Value);
    else if (strcmp(Name, "hideLastGroup") == 0)            hideLastGroup = atoi(Value);
    else if (strcmp(Name, "hideEpgImages") == 0)            hideEpgImages = atoi(Value);
    else if (strcmp(Name, "epgImageWidth") == 0)            epgImageWidth = atoi(Value);
    else if (strcmp(Name, "epgImageHeight") == 0)           epgImageHeight = atoi(Value);
    else if (strcmp(Name, "timeLineWidthPercent") == 0)     timeLineWidthPercent = atoi(Value);
    else if (strcmp(Name, "timeLineHeightPercent") == 0)    timeLineHeightPercent = atoi(Value);
    else if (strcmp(Name, "displayChannelName") == 0)       displayChannelName = atoi(Value);
    else if (strcmp(Name, "channelHeaderWidthPercent") == 0)  channelHeaderWidthPercent = atoi(Value);
    else if (strcmp(Name, "channelHeaderHeightPercent") == 0) channelHeaderHeightPercent = atoi(Value);
    else if (strcmp(Name, "footerHeight") == 0)             footerHeight = atoi(Value); 
    else if (strcmp(Name, "fontIndex") == 0)                fontIndex = atoi(Value);
    else if (strcmp(Name, "FontButtonDelta") == 0)          FontButtonDelta = atoi(Value);
    else if (strcmp(Name, "FontDetailViewDelta") == 0)      FontDetailViewDelta = atoi(Value);
    else if (strcmp(Name, "FontDetailHeaderDelta") == 0)    FontDetailHeaderDelta = atoi(Value);
    else if (strcmp(Name, "FontMessageBoxDelta") == 0)      FontMessageBoxDelta = atoi(Value);  
    else if (strcmp(Name, "FontMessageBoxLargeDelta") == 0) FontMessageBoxLargeDelta = atoi(Value); 
    else if (strcmp(Name, "FontStatusHeaderDelta") == 0)    FontStatusHeaderDelta = atoi(Value);    
    else if (strcmp(Name, "FontStatusHeaderLargeDelta") == 0) FontStatusHeaderLargeDelta = atoi(Value);
    else if (strcmp(Name, "FontChannelHeaderDelta") == 0)   FontChannelHeaderDelta = atoi(Value);
    else if (strcmp(Name, "FontChannelGroupsDelta") == 0)   FontChannelGroupsDelta = atoi(Value);
    else if (strcmp(Name, "FontGridDelta") == 0)            FontGridDelta = atoi(Value);    
    else if (strcmp(Name, "FontGridSmallDelta") == 0)       FontGridSmallDelta = atoi(Value);   
    else if (strcmp(Name, "FontTimeLineWeekdayDelta") == 0) FontTimeLineWeekdayDelta = atoi(Value); 
    else if (strcmp(Name, "FontTimeLineDateDelta") == 0)    FontTimeLineDateDelta = atoi(Value);    
    else if (strcmp(Name, "FontTimeLineTimeDelta") == 0)    FontTimeLineTimeDelta = atoi(Value);
    else if (strcmp(Name, "FontChannelHeaderHorizontalDelta") == 0) FontChannelHeaderHorizontalDelta = atoi(Value);
    else if (strcmp(Name, "FontChannelGroupsHorizontalDelta") == 0) FontChannelGroupsHorizontalDelta = atoi(Value);
    else if (strcmp(Name, "FontGridHorizontalDelta") == 0)  FontGridHorizontalDelta = atoi(Value);
    else if (strcmp(Name, "FontGridHorizontalSmallDelta") == 0) FontGridHorizontalSmallDelta = atoi(Value);
    else if (strcmp(Name, "FontTimeLineDateHorizontalDelta") == 0) FontTimeLineDateHorizontalDelta = atoi(Value);
    else if (strcmp(Name, "FontTimeLineTimeHorizontalDelta") == 0) FontTimeLineTimeHorizontalDelta = atoi(Value);
    else if (strcmp(Name, "displayRerunsDetailEPGView") == 0) displayRerunsDetailEPGView = atoi(Value);
    else if (strcmp(Name, "numReruns") == 0)                numReruns = atoi(Value);
    else if (strcmp(Name, "useSubtitleRerun") == 0)         useSubtitleRerun = atoi(Value);
    else return false;
    return true;
}
