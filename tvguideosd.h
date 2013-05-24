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
  cFooter *footer;
  bool detailViewActive;
  void drawOsd();
  void readChannels(const cChannel *channelStart);
  void drawGridsChannelJump();
  void drawGridsTimeJump();
  void processKeyUp();
  void processKeyDown();
  void processKeyLeft();
  void processKeyRight();
  void processKeyRed();
  void processKeyGreen();
  void processKeyYellow();
  eOSState processKeyBlue();
  void processKeyOk();
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
  void dump();
public:
  cTvGuideOsd(void);
  virtual ~cTvGuideOsd(void);
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
};

#endif //__TVGUIDE_TVGUIDEOSD_H
