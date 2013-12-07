#ifndef __TVGUIDE_CONFIG_H
#define __TVGUIDE_CONFIG_H

#include <vdr/themes.h>
#include <vdr/plugin.h>
#include "osdmanager.h"

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

class cTvguideConfig {
    private:
        void SetGeometry(int width, int height);
        void SetFonts(void);
    public:
        cTvguideConfig();
        ~cTvguideConfig();
        void SetLogoPath(cString path);
        void SetImagesPath(cString path);
        void SetIconsPath(cString path);
        void SetBlending(void);
        int showMainMenuEntry;
        int replaceOriginalSchedule;
        int osdWidth; 
        int osdHeight;
        int displayMode;
        int showTimeInGrid;
        int colWidth;
        int rowHeight;
        int channelCols;
        int channelRows;
        int numGrids;
        int displayTime;
        int minutePixel;
        int displayStatusHeader;
        int displayChannelGroups;
        int statusHeaderPercent;
        int statusHeaderHeight;
        int channelGroupsPercent;
        int channelGroupsWidth;
        int channelGroupsHeight;
        int scaleVideo;
        int decorateVideo;
        int timeLineWidthPercent;
        int timeLineHeightPercent;
        int timeLineWidth;
        int timeLineHeight;
        int displayChannelName;
        int channelHeaderWidthPercent;
        int channelHeaderHeightPercent;
        int channelHeaderWidth;
        int channelHeaderHeight;
        int footerHeight;
        int stepMinutes;
        int bigStepHours;
        int hugeStepHours;
        int channelJumpMode;
        int jumpChannels;
        int blueKeyMode;
        int closeOnSwitch;
        int useRemoteTimers;
        int hideLastGroup;
        int hideChannelLogos;
        int logoWidthRatio;
        int logoHeightRatio;
        cString logoPath;
        int logoExtension;
        int hideEpgImages;
        int epgImageWidth;
        int epgImageHeight;
        int numAdditionalEPGPictures;
        int epgImageWidthLarge;
        int epgImageHeightLarge;
        cString epgImagePath;
        cString iconPath;
        int recMenuAskFolder;
        int fontIndex;
        const char *fontNameDefault;
        int FontButtonDelta;
        int FontDetailViewDelta;
        int FontDetailViewSmallDelta;
        int FontDetailHeaderDelta;
        int FontMessageBoxDelta;
        int FontMessageBoxLargeDelta;
        int FontStatusHeaderDelta;
        int FontStatusHeaderLargeDelta;
        int FontChannelHeaderDelta;
        int FontChannelGroupsDelta;
        int FontGridDelta;
        int FontGridSmallDelta;
        int FontTimeLineWeekdayDelta;
        int FontTimeLineDateDelta;
        int FontTimeLineTimeDelta;
        int FontChannelHeaderHorizontalDelta;
        int FontChannelGroupsHorizontalDelta;
        int FontGridHorizontalDelta;
        int FontGridHorizontalSmallDelta;
        int FontTimeLineDateHorizontalDelta;
        int FontTimeLineTimeHorizontalDelta;
        int FontRecMenuItemDelta;
        int FontRecMenuItemSmallDelta;
        const cFont *FontChannelHeader;
        const cFont *FontChannelHeaderHorizontal;
        const cFont *FontChannelGroups;
        const cFont *FontChannelGroupsHorizontal;
        const cFont *FontStatusHeader;
        const cFont *FontStatusHeaderLarge;
        const cFont *FontGrid;
        const cFont *FontGridSmall;
        const cFont *FontGridHorizontal;
        const cFont *FontGridHorizontalSmall;
        const cFont *FontTimeLineWeekday;
        const cFont *FontTimeLineDate;
        const cFont *FontTimeLineDateHorizontal;
        const cFont *FontTimeLineTime;
        const cFont *FontTimeLineTimeHorizontal;
        const cFont *FontButton;
        const cFont *FontDetailView;
        const cFont *FontDetailViewSmall;
        const cFont *FontDetailHeader;
        const cFont *FontMessageBox;
        const cFont *FontMessageBoxLarge;
        const cFont *FontRecMenuItem;
        const cFont *FontRecMenuItemSmall;
        int timeFormat;
        int themeIndex;
        int useBlending;
        int roundedCorners;
        int displayRerunsDetailEPGView;
        int numReruns;
        int useSubtitleRerun;
        void setDynamicValues(int width, int height);
        bool SetupParse(const char *Name, const char *Value);
        void loadTheme();
};

#ifdef DEFINE_CONFIG
    cTvguideConfig tvguideConfig;
    cOsdManager osdManager;
    cTheme theme;
    cPlugin* pRemoteTimers = NULL;
#else
    extern cTvguideConfig tvguideConfig;
    extern cOsdManager osdManager;
    extern cTheme theme;
    extern cPlugin* pRemoteTimers;
#endif


// --- Theme -------------------------------------------------------------
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

#endif //__TVGUIDE_CONFIG_H