#include "setup.h"

cTvguideSetup::cTvguideSetup() {
	tmpTvguideConfig = tvguideConfig;
	Setup();
}

cTvguideSetup::~cTvguideSetup() {
}


void cTvguideSetup::Setup(void) {
	int currentItem = Current();
	Clear();
	
	Add(new cOsdItem(tr("General Settings")));
	Add(new cOsdItem(tr("Screen Presentation")));
	Add(new cOsdItem(tr("Fonts and Fontsizes")));
	
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
		}
	}	
	return state;
}

void cTvguideSetup::Store(void) {

	tvguideConfig = tmpTvguideConfig;

	SetupStore("themeIndex", tvguideConfig.themeIndex);
	SetupStore("displayMode", tvguideConfig.displayMode);
    SetupStore("displayStatusHeader", tvguideConfig.displayStatusHeader);
	SetupStore("statusHeaderPercent", tvguideConfig.statusHeaderPercent);
	SetupStore("scaleVideo", tvguideConfig.scaleVideo);
	SetupStore("decorateVideo", tvguideConfig.decorateVideo);
    SetupStore("useBlending", tvguideConfig.useBlending);
	SetupStore("roundedCorners", tvguideConfig.roundedCorners);
	SetupStore("timeFormat", tvguideConfig.timeFormat);
	SetupStore("channelCols", tvguideConfig.channelCols);
	SetupStore("channelRows", tvguideConfig.channelRows);
	SetupStore("displayTime", tvguideConfig.displayTime);
	SetupStore("bigStepHours", tvguideConfig.bigStepHours);
	SetupStore("hugeStepHours", tvguideConfig.hugeStepHours);
	SetupStore("jumpChannels", tvguideConfig.jumpChannels);
	SetupStore("hideChannelLogos", tvguideConfig.hideChannelLogos);
	SetupStore("logoExtension", tvguideConfig.logoExtension);
	SetupStore("logoWidthRatio", tvguideConfig.logoWidthRatio);
	SetupStore("logoHeightRatio", tvguideConfig.logoHeightRatio);
	SetupStore("hideEpgImages", tvguideConfig.hideEpgImages);
	SetupStore("epgImageWidth", tvguideConfig.epgImageWidth);
	SetupStore("epgImageHeight", tvguideConfig.epgImageHeight);
	SetupStore("epgImageHeight", tvguideConfig.epgImageHeight);
	SetupStore("timeLineWidthPercent", tvguideConfig.timeLineWidthPercent);
	SetupStore("timeLineHeightPercent", tvguideConfig.timeLineHeightPercent);
    SetupStore("displayChannelName", tvguideConfig.displayChannelName);
    SetupStore("channelHeaderWidthPercent", tvguideConfig.channelHeaderWidthPercent);
    SetupStore("channelHeaderHeightPercent", tvguideConfig.channelHeaderHeightPercent);
	SetupStore("footerHeight", tvguideConfig.footerHeight);
	SetupStore("fontIndex", tvguideConfig.fontIndex);
    SetupStore("FontButtonDelta", tvguideConfig.FontButtonDelta);
    SetupStore("FontDetailViewDelta", tvguideConfig.FontDetailViewDelta);
    SetupStore("FontDetailHeaderDelta", tvguideConfig.FontDetailHeaderDelta);
	SetupStore("FontMessageBoxDelta", tvguideConfig.FontMessageBoxDelta);
	SetupStore("FontMessageBoxLargeDelta", tvguideConfig.FontMessageBoxLargeDelta);
	SetupStore("FontStatusHeaderDelta", tvguideConfig.FontStatusHeaderDelta);
	SetupStore("FontStatusHeaderLargeDelta", tvguideConfig.FontStatusHeaderLargeDelta);
	SetupStore("FontChannelHeaderDelta", tvguideConfig.FontChannelHeaderDelta);
	SetupStore("FontGridDelta", tvguideConfig.FontGridDelta);
	SetupStore("FontGridSmallDelta", tvguideConfig.FontGridSmallDelta);
	SetupStore("FontTimeLineWeekdayDelta", tvguideConfig.FontTimeLineWeekdayDelta);
	SetupStore("FontTimeLineDateDelta", tvguideConfig.FontTimeLineDateDelta);
	SetupStore("FontTimeLineTimeDelta", tvguideConfig.FontTimeLineTimeDelta);
	SetupStore("FontChannelHeaderHorizontalDelta", tvguideConfig.FontChannelHeaderHorizontalDelta);
	SetupStore("FontGridHorizontalDelta", tvguideConfig.FontGridHorizontalDelta);
	SetupStore("FontGridHorizontalSmallDelta", tvguideConfig.FontGridHorizontalSmallDelta);
	SetupStore("FontTimeLineDateHorizontalDelta", tvguideConfig.FontTimeLineDateHorizontalDelta);
	SetupStore("FontTimeLineTimeHorizontalDelta", tvguideConfig.FontTimeLineTimeHorizontalDelta);
}

cMenuSetupSubMenu::cMenuSetupSubMenu(const char* Title, cTvguideConfig* data) : cOsdMenu(Title, 30) {
	tmpTvguideConfig = data;
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
    blendingMethods[0] = "none";
    blendingMethods[1] = "classic";
    blendingMethods[2] = "nOpacity style";
	Set();
}

void cMenuSetupGeneral::Set(void) {
	int currentItem = Current();
	Clear();
    if (themes.NumThemes())
		Add(new cMenuEditStraItem(tr("Theme"), &tmpTvguideConfig->themeIndex, themes.NumThemes(), themes.Descriptions()));
	Add(new cMenuEditStraItem(tr("Use color gradients"), &tmpTvguideConfig->useBlending, 3, blendingMethods));
	Add(new cMenuEditBoolItem(tr("Rounded Corners"), &tmpTvguideConfig->roundedCorners));

	Add(new cMenuEditIntItem(tr("Channels to Jump (Keys Green / Yellow)"), &tmpTvguideConfig->jumpChannels, 2, 30));
	Add(new cMenuEditIntItem(tr("Time to display in minutes"), &tmpTvguideConfig->displayTime, 120, 320));
	Add(new cMenuEditIntItem(tr("Big Step (Keys 1 / 3) in hours"), &tmpTvguideConfig->bigStepHours, 1, 12));
	Add(new cMenuEditIntItem(tr("Huge Step (Keys 4 / 6) in hours"), &tmpTvguideConfig->hugeStepHours, 13, 48));
	Add(new cMenuEditStraItem(tr("Time Format (12h/24h)"), &tmpTvguideConfig->timeFormat, 2,  timeFormatItems));

	SetCurrent(Get(currentItem));
	Display();
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
	const char *indent = "    ";
	int currentItem = Current();
	Clear();
	
    Add(new cMenuEditStraItem(tr("Display Mode"), &tmpTvguideConfig->displayMode, 2,  displayModeItems));
    if (tmpTvguideConfig->displayMode == eVertical) {
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("Height of Channel Header (Perc. of osd height)")), &tmpTvguideConfig->channelHeaderHeightPercent, 5, 30));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("Width of Timeline (Perc. of osd width)")), &tmpTvguideConfig->timeLineWidthPercent, 5, 30));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("Number of Channels to display")), &tmpTvguideConfig->channelCols, 3, 12));
    } else if (tmpTvguideConfig->displayMode == eHorizontal) {
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("Width of Channel Header (Perc. of osd width)")), &tmpTvguideConfig->channelHeaderWidthPercent, 5, 30));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("Height of Timeline (Perc. of osd height)")), &tmpTvguideConfig->timeLineHeightPercent, 5, 30));
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("Number of Channels to display")), &tmpTvguideConfig->channelRows, 3, 12));
    }
	Add(new cMenuEditIntItem(tr("Height of Footer"), &tmpTvguideConfig->footerHeight, 50, 300));
    
    Add(new cMenuEditBoolItem(tr("Display status header"), &tmpTvguideConfig->displayStatusHeader));
    if (tmpTvguideConfig->displayStatusHeader) {
        Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("Height of status header (Perc. of osd height)")), &tmpTvguideConfig->statusHeaderPercent, 5, 50));
        Add(new cMenuEditBoolItem(*cString::sprintf("%s%s", indent, tr("Scale video to upper right corner")), &tmpTvguideConfig->scaleVideo));
        Add(new cMenuEditBoolItem(*cString::sprintf("%s%s", indent, tr("Rounded corners around video frame")), &tmpTvguideConfig->decorateVideo));
    }
	
	Add(new cMenuEditBoolItem(tr("Display Channel Names in Header"), &tmpTvguideConfig->displayChannelName));
	Add(new cMenuEditStraItem(tr("Show Channel Logos"), &tmpTvguideConfig->hideChannelLogos, 2,  hideChannelLogosItems));	
	if (!tmpTvguideConfig->hideChannelLogos) {
		Add(InfoItem(tr("Logo Path used"), *tvguideConfig.logoPath));
		Add(new cMenuEditStraItem(*cString::sprintf("%s%s", indent, tr("Logo Extension")), &tmpTvguideConfig->logoExtension, 2,  logoExtensionItems));
		Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("Logo width ratio")), &tmpTvguideConfig->logoWidthRatio, 1, 1000));
		Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("Logo height ratio")), &tmpTvguideConfig->logoHeightRatio, 1, 1000));
	}
	
	Add(new cMenuEditStraItem(tr("Show EPG Images"), &tmpTvguideConfig->hideEpgImages, 2,  hideChannelLogosItems));	
	if (!tmpTvguideConfig->hideEpgImages) {
		Add(InfoItem(tr("EPG Images Path used"), *tvguideConfig.epgImagePath));
		Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("EPG Image width")), &tmpTvguideConfig->epgImageWidth, 0, 800));
		Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("EPG Image height")), &tmpTvguideConfig->epgImageHeight, 0, 800));
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
        Add(new cMenuEditIntItem(tr("Grid Font Size"), &tmpTvguideConfig->FontGridDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Grid Font Small Size"), &tmpTvguideConfig->FontGridSmallDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Timeline Weekday Font Size"), &tmpTvguideConfig->FontTimeLineWeekdayDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Timeline Date Font Size"), &tmpTvguideConfig->FontTimeLineDateDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Timeline Time Font Size"), &tmpTvguideConfig->FontTimeLineTimeDelta, -30, 30));
    } else if (tmpTvguideConfig->displayMode == eHorizontal) {
        Add(new cMenuEditIntItem(tr("Channel Header Font Size"), &tmpTvguideConfig->FontChannelHeaderHorizontalDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Grid Font Size"), &tmpTvguideConfig->FontGridHorizontalDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Grid Font Small Size"), &tmpTvguideConfig->FontGridHorizontalSmallDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Timeline Date Font Size"), &tmpTvguideConfig->FontTimeLineDateHorizontalDelta, -30, 30));
        Add(new cMenuEditIntItem(tr("Timeline Time Font Size"), &tmpTvguideConfig->FontTimeLineTimeHorizontalDelta, -30, 30));
    }


	SetCurrent(Get(currentItem));
	Display();
}