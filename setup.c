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
	SetupStore("useBlending", tvguideConfig.useBlending);
	SetupStore("roundedCorners", tvguideConfig.roundedCorners);
	SetupStore("timeFormat", tvguideConfig.timeFormat);
	SetupStore("channelCols", tvguideConfig.channelCols);
	SetupStore("displayTime", tvguideConfig.displayTime);
	SetupStore("bigStepHours", tvguideConfig.bigStepHours);
	SetupStore("hugeStepHours", tvguideConfig.hugeStepHours);
	SetupStore("jumpChannels", tvguideConfig.jumpChannels);
	SetupStore("hideChannelLogos", tvguideConfig.hideChannelLogos);
	SetupStore("logoExtension", tvguideConfig.logoExtension);
	SetupStore("logoWidth", tvguideConfig.logoWidth);
	SetupStore("logoHeight", tvguideConfig.logoHeight);
	SetupStore("hideEpgImages", tvguideConfig.hideEpgImages);
	SetupStore("epgImageWidth", tvguideConfig.epgImageWidth);
	SetupStore("epgImageHeight", tvguideConfig.epgImageHeight);
	SetupStore("epgImageHeight", tvguideConfig.epgImageHeight);
	SetupStore("timeColWidth", tvguideConfig.timeColWidth);
	SetupStore("headerHeight", tvguideConfig.headerHeight);
	SetupStore("footerHeight", tvguideConfig.footerHeight);
	SetupStore("fontIndex", tvguideConfig.fontIndex);
	SetupStore("fontHeaderSize", tvguideConfig.fontHeaderSize);
	SetupStore("fontGridSize", tvguideConfig.fontGridSize);
	SetupStore("fontGridSmallSize", tvguideConfig.fontGridSmallSize);
	SetupStore("fontTimeLineWeekdaySize", tvguideConfig.fontTimeLineWeekdaySize);
	SetupStore("fontTimeLineDateSize", tvguideConfig.fontTimeLineDateSize);
	SetupStore("fontTimeLineTimeSize", tvguideConfig.fontTimeLineTimeSize);
	SetupStore("fontButtonSize", tvguideConfig.fontButtonSize);
	SetupStore("fontDetailViewSize", tvguideConfig.fontDetailViewSize);
	SetupStore("fontDetailHeaderSize", tvguideConfig.fontDetailHeaderSize);
	SetupStore("fontMessageBoxSize", tvguideConfig.fontMessageBoxSize);
	SetupStore("fontMessageBoxLargeSize", tvguideConfig.fontMessageBoxLargeSize);
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
	timeFormatItems[0] = "12h";
	timeFormatItems[1] = "24h";

	Set();
}

void cMenuSetupGeneral::Set(void) {
	int currentItem = Current();
	Clear();
	
	Add(new cMenuEditIntItem(tr("Number of Channels / Columns"), &tmpTvguideConfig->channelCols, 3, 8));
	Add(new cMenuEditIntItem(tr("Channels to Jump (Keys Green / Yellow)"), &tmpTvguideConfig->jumpChannels, 2, 30));
	Add(new cMenuEditIntItem(tr("Time to display vertically in minutes"), &tmpTvguideConfig->displayTime, 120, 320));
	Add(new cMenuEditIntItem(tr("Big Step (Keys 1 / 3) in hours"), &tmpTvguideConfig->bigStepHours, 1, 12));
	Add(new cMenuEditIntItem(tr("Huge Step (Keys 4 / 6) in hours"), &tmpTvguideConfig->hugeStepHours, 13, 48));
	Add(new cMenuEditStraItem(tr("Time Format (12h/24h)"), &tmpTvguideConfig->timeFormat, 2,  timeFormatItems));

	SetCurrent(Get(currentItem));
	Display();
}

//------------------------------------------------------------------------------------------------------------------

cMenuSetupScreenLayout::cMenuSetupScreenLayout(cTvguideConfig* data)  : cMenuSetupSubMenu(tr("Screen Presentation"), data) {
	themes.Load(*cString("tvguide"));
	hideChannelLogosItems[0] = trVDR("yes");
	hideChannelLogosItems[1] = trVDR("no");
	logoExtensionItems[0] = "png";
	logoExtensionItems[1] = "jpg";
	Set();
}

void cMenuSetupScreenLayout::Set(void) {
	int currentItem = Current();
	Clear();
	if (themes.NumThemes())
		Add(new cMenuEditStraItem(tr("Theme"), &tmpTvguideConfig->themeIndex, themes.NumThemes(), themes.Descriptions()));
	Add(new cMenuEditBoolItem(tr("Use color gradients"), &tmpTvguideConfig->useBlending));
	Add(new cMenuEditBoolItem(tr("Rounded Corners"), &tmpTvguideConfig->roundedCorners));
	Add(new cMenuEditIntItem(tr("Width of Timeline"), &tmpTvguideConfig->timeColWidth, 50, 300));
	Add(new cMenuEditIntItem(tr("Height of Header"), &tmpTvguideConfig->headerHeight, 50, 300));
	Add(new cMenuEditIntItem(tr("Height of Footer"), &tmpTvguideConfig->footerHeight, 50, 300));
	
	const char *indent = "    ";
	Add(new cMenuEditStraItem(tr("Show Channel Logos"), &tmpTvguideConfig->hideChannelLogos, 2,  hideChannelLogosItems));	
	if (!tmpTvguideConfig->hideChannelLogos) {
		Add(InfoItem(tr("Logo Path used"), *tvguideConfig.logoPath));
		Add(new cMenuEditStraItem(*cString::sprintf("%s%s", indent, tr("Logo Extension")), &tmpTvguideConfig->logoExtension, 2,  logoExtensionItems));
		Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("Logo width")), &tmpTvguideConfig->logoWidth, 0, 350));
		Add(new cMenuEditIntItem(*cString::sprintf("%s%s", indent, tr("Logo height")), &tmpTvguideConfig->logoHeight, 0, 250));
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
	Add(new cMenuEditIntItem(tr("Channel Header Font Size"), &tmpTvguideConfig->fontHeaderSize, 10, 70));
	Add(new cMenuEditIntItem(tr("Grid Font Size"), &tmpTvguideConfig->fontGridSize, 10, 70));
	Add(new cMenuEditIntItem(tr("Grid Font Small Size"), &tmpTvguideConfig->fontGridSmallSize, 10, 70));
	Add(new cMenuEditIntItem(tr("Timeline Weekday Font Size"), &tmpTvguideConfig->fontTimeLineWeekdaySize, 10, 70));
	Add(new cMenuEditIntItem(tr("Timeline Date Font Size"), &tmpTvguideConfig->fontTimeLineDateSize, 10, 70));
	Add(new cMenuEditIntItem(tr("Timeline Time Font Size"), &tmpTvguideConfig->fontTimeLineTimeSize, 10, 70));
	Add(new cMenuEditIntItem(tr("Button Font Size"), &tmpTvguideConfig->fontButtonSize, 10, 70));
	Add(new cMenuEditIntItem(tr("Detail EPG View Font Size"), &tmpTvguideConfig->fontDetailViewSize, 10, 70));
	Add(new cMenuEditIntItem(tr("Detail EPG View Header Font Size"), &tmpTvguideConfig->fontDetailHeaderSize, 10, 70));
	Add(new cMenuEditIntItem(tr("Message Font Size"), &tmpTvguideConfig->fontMessageBoxSize, 10, 70));
	Add(new cMenuEditIntItem(tr("Message Large Font Size"), &tmpTvguideConfig->fontMessageBoxLargeSize, 10, 70));
	
	SetCurrent(Get(currentItem));
	Display();
}