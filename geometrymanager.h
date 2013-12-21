#ifndef __TVGUIDE_GEOMETRYMANAGER_H
#define __TVGUIDE_GEOMETRYMANAGER_H

class cGeometryManager {
private:
public:
	cGeometryManager(void);
	~cGeometryManager();
	bool SetGeometry(int osdWidth, int osdHeight, bool force = false);
    //Common
	int osdWidth;
	int osdHeight;
	int statusHeaderHeight;
    int tvFrameWidth;
    int statusHeaderContentWidth;
    //ChannelGroups
	int	channelGroupsWidth;
    int channelGroupsHeight;
    //ContentHeader
    int channelHeaderWidth;
    int channelHeaderHeight;
    int logoWidth;
    int logoHeight;
    //Content
    int colWidth;
    int rowHeight;
    int minutePixel;
    int channelLogoWidth;
    int channelLogoHeight;
    //Timeline
    int timeLineWidth;
    int timeLineHeight;
    int timeLineGridWidth;
    int timeLineGridHeight;
    int dateVieverWidth;
    int dateVieverHeight;
    int clockWidth;
    int clockHeight;
    //Footer
    int footerHeight;
    int buttonWidth;
    int buttonHeight;
    int buttonBorder;
    //Detailed EPG View
    int epgViewHeaderHeight;
    //Recording Menus
    int borderRecMenus;
};

#endif //__TVGUIDE_GEOMETRYMANAGER_H