#include "config.h"

enum {
	e12Hours,
	e24Hours
};

cTvguideConfig::cTvguideConfig() {
	osdWidth = 0; 
	osdHeight = 0;
	colWidth = 0;
	channelCols = 5;
	displayTime = 160;
	minuteHeight = 0;
	timeColWidth = 120;
	headerHeight = 145;
	footerHeight = 80;
	stepMinutes = 30;
	bigStepHours = 3;
	hugeStepHours = 24;
	jumpChannels = 5;
	hideChannelLogos = 0;
	logoWidth = 130;
	logoHeight = 100;
	logoExtension = 0;
	hideEpgImages = 0;
	epgImageWidth = 315;
	epgImageHeight = 240;
	fontIndex = 0;
	fontNameDefault = "VDRSymbols Sans:Book";
	fontHeaderSize = 33;
	fontGridSize = 27;
	fontGridSmallSize = 24;
	fontTimeLineWeekdaySize = 40;
	fontTimeLineDateSize = 33;
	fontTimeLineTimeSize = 0;
	fontTimeLineTimeSizeDef12 = 24;
	fontTimeLineTimeSizeDef24 = 33;
	fontButtonSize = 33;
	fontDetailViewSize = 33;
	fontDetailHeaderSize = 40;
	fontMessageBoxSize = 33;
	fontMessageBoxLargeSize = 40;


	FontHeader = NULL;
	FontGrid = NULL;
	FontGridSmall = NULL;
	FontTimeLineWeekday = NULL;
	FontTimeLineDate = NULL;
	FontTimeLineTime = NULL;
	FontButton = NULL;
	FontDetailView = NULL;
	FontDetailHeader = NULL;
	FontMessageBox = NULL;
	FontMessageBoxLarge = NULL;
	
	timeFormat = 1;
	themeIndex = 0;
	useBlending = 1;
	roundedCorners = 0;
}

cTvguideConfig::~cTvguideConfig() {
	delete FontHeader;
	delete FontGrid;
	delete FontGridSmall;
	delete FontTimeLineWeekday;
	delete FontTimeLineDate;
	delete FontTimeLineTime;
	delete FontButton;
	delete FontDetailView;
	delete FontDetailHeader;
	delete FontMessageBox;
	delete FontMessageBoxLarge;
}

void cTvguideConfig::setDynamicValues(int width, int height) {
	osdWidth = width;
	osdHeight = height;
	colWidth = (osdWidth - timeColWidth) / channelCols;
	minuteHeight = (osdHeight - headerHeight - footerHeight) / displayTime;
	
	if (!fontTimeLineTimeSize) {
		if (timeFormat == e12Hours) {
			fontTimeLineTimeSize = fontTimeLineTimeSizeDef12;
		} else if (timeFormat == e24Hours) {
			fontTimeLineTimeSize = fontTimeLineTimeSizeDef24;
		}
	} else if ((fontTimeLineTimeSize == fontTimeLineTimeSizeDef12) && (timeFormat == e24Hours)) {
		fontTimeLineTimeSize = fontTimeLineTimeSizeDef24;
	} else if ((fontTimeLineTimeSize == fontTimeLineTimeSizeDef24) && (timeFormat == e12Hours)) {
		fontTimeLineTimeSize = fontTimeLineTimeSizeDef12;
	}
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
	test = cFont::CreateFont(*fontname, fontHeaderSize);
	if (!test) {
		fontname = DefaultFontSml;
	}
	delete test;
	FontHeader = cFont::CreateFont(*fontname, fontHeaderSize);
	FontGrid = cFont::CreateFont(*fontname, fontGridSize);
	FontGridSmall = cFont::CreateFont(*fontname, fontGridSmallSize);
	FontTimeLineWeekday = cFont::CreateFont(*fontname, fontTimeLineWeekdaySize);
	FontTimeLineDate = cFont::CreateFont(*fontname, fontTimeLineDateSize);
	FontTimeLineTime = cFont::CreateFont(*fontname, fontTimeLineTimeSize);
	FontButton = cFont::CreateFont(*fontname, fontButtonSize);
	FontDetailView = cFont::CreateFont(*fontname, fontDetailViewSize);
	FontDetailHeader = cFont::CreateFont(*fontname, fontDetailHeaderSize);
	FontMessageBox = cFont::CreateFont(*fontname, fontMessageBoxSize);
	FontMessageBoxLarge = cFont::CreateFont(*fontname, fontMessageBoxLargeSize);	
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
	else if (strcmp(Name, "themeIndex") == 0)         		themeIndex = atoi(Value);
	else if (strcmp(Name, "useBlending") == 0)         		useBlending = atoi(Value);
	else if (strcmp(Name, "roundedCorners") == 0)         	roundedCorners = atoi(Value);
	else if (strcmp(Name, "channelCols") == 0)         		channelCols = atoi(Value);
	else if (strcmp(Name, "displayTime") == 0)         		displayTime = atoi(Value);
	else if (strcmp(Name, "hideChannelLogos") == 0)         hideChannelLogos = atoi(Value);
	else if (strcmp(Name, "logoExtension") == 0) 			logoExtension = atoi(Value);
	else if (strcmp(Name, "logoWidth") == 0) 				logoWidth = atoi(Value);
	else if (strcmp(Name, "logoHeight") == 0) 				logoHeight = atoi(Value);
	else if (strcmp(Name, "bigStepHours") == 0) 			bigStepHours = atoi(Value);
	else if (strcmp(Name, "hugeStepHours") == 0) 			hugeStepHours = atoi(Value);
	else if (strcmp(Name, "jumpChannels") == 0) 			jumpChannels = atoi(Value);
	else if (strcmp(Name, "hideEpgImages") == 0)         	hideEpgImages = atoi(Value);
	else if (strcmp(Name, "epgImageWidth") == 0) 			epgImageWidth = atoi(Value);
	else if (strcmp(Name, "epgImageHeight") == 0) 			epgImageHeight = atoi(Value);
	else if (strcmp(Name, "timeColWidth") == 0) 			timeColWidth = atoi(Value);
	else if (strcmp(Name, "headerHeight") == 0) 			headerHeight = atoi(Value);
	else if (strcmp(Name, "footerHeight") == 0) 			footerHeight = atoi(Value);	
	else if (strcmp(Name, "fontIndex") == 0) 				fontIndex = atoi(Value);	
	else if (strcmp(Name, "fontHeaderSize") == 0) 			fontHeaderSize = atoi(Value);	
	else if (strcmp(Name, "fontGridSize") == 0) 			fontGridSize = atoi(Value);	
	else if (strcmp(Name, "fontGridSmallSize") == 0) 		fontGridSmallSize = atoi(Value);	
	else if (strcmp(Name, "fontTimeLineWeekdaySize") == 0) 	fontTimeLineWeekdaySize = atoi(Value);	
	else if (strcmp(Name, "fontTimeLineDateSize") == 0) 	fontTimeLineDateSize = atoi(Value);
	else if (strcmp(Name, "fontTimeLineTimeSize") == 0) 	fontTimeLineTimeSize = atoi(Value);	
	else if (strcmp(Name, "fontButtonSize") == 0) 			fontButtonSize = atoi(Value);	
	else if (strcmp(Name, "fontDetailViewSize") == 0) 		fontDetailViewSize = atoi(Value);	
	else if (strcmp(Name, "fontDetailHeaderSize") == 0) 	fontDetailHeaderSize = atoi(Value);	
	else if (strcmp(Name, "fontMessageBoxSize") == 0) 		fontMessageBoxSize = atoi(Value);	
	else if (strcmp(Name, "fontMessageBoxLargeSize") == 0) 	fontMessageBoxLargeSize = atoi(Value);	
	else return false;
	return true;
}