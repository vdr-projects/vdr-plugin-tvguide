#ifndef __TVGUIDE_TVGUIDEOSD_H
#define __TVGUIDE_TVGUIDEOSD_H

// --- cTvGuideOsd -------------------------------------------------------------

class cTvGuideOsd : public cOsdObject {
private:
  cMyTime *myTime;
  cList<cChannelColumn> columns;
  cGrid *activeGrid;
  cStatusHeader *statusHeader;
  cDetailView *detailView;
  cTimeLine *timeLine;
  cChannelGroups *channelGroups;
  cFooter *footer;
  cRecMenuManager *recMenuManager;
  bool detailViewActive;
  void drawOsd();
  void readChannels(const cChannel *channelStart);
  void drawGridsChannelJump(int offset = 0);
  void drawGridsTimeJump();
  void processKeyUp();
  void processKeyDown();
  void processKeyLeft();
  void processKeyRight();
  void processKeyRed();
  void processKeyGreen();
  void processKeyYellow();
  eOSState processKeyBlue();
  eOSState processKeyOk();
  void processKey1();
  void processKey3();
  void processKey4();
  void processKey6();
  void processKey7();
  void processKey9();
  void setNextActiveGrid(cGrid *next);
  void channelForward();
  void channelBack();
  void timeForward();
  void timeBack();
  void ScrollForward();
  void ScrollBack();
  eOSState ChannelSwitch();
  void DetailedEPG();
  void SetTimers();
  void dump();
public:
  cTvGuideOsd(void);
  virtual ~cTvGuideOsd(void);
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
};

#endif //__TVGUIDE_TVGUIDEOSD_H
