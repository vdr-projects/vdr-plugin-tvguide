#ifndef __TVGUIDE_SETUP_H
#define __TVGUIDE_SETUP_H

#include <vdr/menuitems.h>
#include "config.h"

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
        cString indent;
    public:
        cMenuSetupSubMenu(const char *Title, cTvguideConfig *data);
};

class cMenuSetupGeneral : public cMenuSetupSubMenu {
    protected:
        virtual eOSState ProcessKey(eKeys Key);
        cThemes themes;
        const char * timeFormatItems[2];
        const char * jumpMode[2];
        const char * blueMode[2];
        const char * numMode[2];
        const char *useSubtitleRerunTexts[3];
        void Set(void);
    public:
        cMenuSetupGeneral(cTvguideConfig *data);
};

class cMenuSetupScreenLayout : public cMenuSetupSubMenu {
    protected:
        virtual eOSState ProcessKey(eKeys Key);
        const char * displayModeItems[2];
        const char * hideChannelLogosItems[2];
        const char * logoExtensionItems[2];
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

class cMenuSetupImageCache : public cMenuSetupSubMenu {
    protected:
        void Set(void);
    public:
        cMenuSetupImageCache(cTvguideConfig *data);
};

#endif //__TVGUIDE_SETUP_H