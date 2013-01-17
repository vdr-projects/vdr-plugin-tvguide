#ifndef __TVGUIDE_CONFIG_H
#define __TVGUIDE_CONFIG_H

class cTvguideConfig {
	public:
		cTvguideConfig();
		~cTvguideConfig();
		void SetLogoPath(cString path);
		void SetImagesPath(cString path);
		int osdWidth; 
		int osdHeight;
		int colWidth;
		int channelCols;
		int displayTime;
		int minuteHeight;
		int timeColWidth;
		int headerHeight;
		int footerHeight;
		int stepMinutes;
		int bigStepHours;
		int hugeStepHours;
		int jumpChannels;
		int hideChannelLogos;
		int logoWidth;
		int logoHeight;
		cString logoPath;
		int logoExtension;
		int hideEpgImages;
		int epgImageWidth;
		int epgImageHeight;
		cString epgImagePath;
		int fontIndex;
		const char *fontNameDefault;
		int fontHeaderSize;
		int fontGridSize;
		int fontGridSmallSize;
		int fontTimeLineWeekdaySize;
		int fontTimeLineDateSize;
		int fontTimeLineTimeSize;
		int fontTimeLineTimeSizeDef12;
		int fontTimeLineTimeSizeDef24;
		int fontButtonSize;
		int fontDetailViewSize;
		int fontDetailHeaderSize;
		int fontMessageBoxSize;
		int fontMessageBoxLargeSize;
		const cFont *FontHeader;
		const cFont *FontGrid;
		const cFont *FontGridSmall;
		const cFont *FontTimeLineWeekday;
		const cFont *FontTimeLineDate;
		const cFont *FontTimeLineTime;
		const cFont *FontButton;
		const cFont *FontDetailView;
		const cFont *FontDetailHeader;
		const cFont *FontMessageBox;
		const cFont *FontMessageBoxLarge;
		int timeFormat;
		int themeIndex;
		int useBlending;
		int roundedCorners;
		void setDynamicValues(int width, int height);
		bool SetupParse(const char *Name, const char *Value);
		void loadTheme();
};

#endif //__TVGUIDE_CONFIG_H