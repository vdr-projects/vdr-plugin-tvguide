#ifndef __TVGUIDE_CONFIG_H
#define __TVGUIDE_CONFIG_H

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
        int fontIndex;
        const char *fontNameDefault;
        int FontButtonDelta;
        int FontDetailViewDelta;
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

#endif //__TVGUIDE_CONFIG_H