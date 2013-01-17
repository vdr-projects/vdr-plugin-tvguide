#ifndef __TVGUIDE_SETUP_H
#define __TVGUIDE_SETUP_H

class cTvguideSetup : public cMenuSetupPage {
	public:
		cTvguideSetup(void);
		virtual ~cTvguideSetup();		
	private:
		cTvguideConfig tmpTvguideConfig;
		void Setup(void);
	protected:
		virtual eOSState ProcessKey(eKeys Key);
		virtual void Store(void);

};

class cMenuSetupSubMenu : public cOsdMenu {
	protected:
		cTvguideConfig *tmpTvguideConfig;
		virtual eOSState ProcessKey(eKeys Key);
		virtual void Set(void) = 0;
		cOsdItem *InfoItem(const char *label, const char *value);
	public:
		cMenuSetupSubMenu(const char *Title, cTvguideConfig *data);
};

class cMenuSetupGeneral : public cMenuSetupSubMenu {
	protected:
		const char * timeFormatItems[2];
		void Set(void);
	public:
		cMenuSetupGeneral(cTvguideConfig *data);
};

class cMenuSetupScreenLayout : public cMenuSetupSubMenu {
	protected:
		virtual eOSState ProcessKey(eKeys Key);
		cThemes themes;
		const char * hideChannelLogosItems[2];
		const char * logoExtensionItems[2];
        const char * blendingMethods[3];
		void Set(void);
	public:
		cMenuSetupScreenLayout(cTvguideConfig *data);
};

class cMenuSetupFont : public cMenuSetupSubMenu {
	protected:
		cStringList fontNames;
		void Set(void);
	public:
		cMenuSetupFont(cTvguideConfig *data);
};

#endif //__TVGUIDE_SETUP_H