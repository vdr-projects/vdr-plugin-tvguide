#ifndef __TVGUIDE_CHANNELCOLUMN_H
#define __TVGUIDE_CHANNELCOLUMN_H

class cEpgGrid;
// --- cChannelColumn  -------------------------------------------------------------

class cChannelColumn : public cListObject, public cStyledPixmap {
friend class cEpgGrid;
private:
    cPixmap *pixmapLogo;
	cMyTime *myTime;
	int num;
	cChannel *channel;
	cList<cEpgGrid> grids;
	cSchedulesLock schedulesLock;
	const cSchedules *schedules;
	bool hasTimer;
public:
	cChannelColumn(int num, cChannel *channel, cMyTime *myTime);
	virtual ~cChannelColumn(void);
	void createHeader();
	void drawHeader();
	bool readGrids();
	void drawGrids();
	int getX();
	cChannel * getChannel() {return channel;}
	cEpgGrid * getActive();
	cEpgGrid * getNext(cEpgGrid *activeGrid);
	cEpgGrid * getPrev(cEpgGrid *activeGrid);
	cEpgGrid * getNeighbor(cEpgGrid *activeGrid);
	void AddNewGridsAtStart();
	void AddNewGridsAtEnd();
	void ClearOutdatedStart();
	void ClearOutdatedEnd();
	int GetNum() {return num;};
	void SetNum(int num) {this->num = num;};
	void setTimer() {hasTimer = true;};
	void clearGrids();
	void dumpGrids();
};

#endif //__TVGUIDE_CHANNELCOLUMN_H