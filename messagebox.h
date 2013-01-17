#ifndef __TVGUIDE_MESSAGEBOX_H
#define __TVGUIDE_MESSAGEBOX_H

class cMessageBoxThreadPool;

// --- cMessageBox  -------------------------------------------------------------

class cMessageBoxThread : public cThread {
private:
	cPixmap *content;
	int FadeTime;
	int FrameTime;
	int displayTime;
	virtual void Action(void);
public:
	cMessageBoxThread(cPixmap *content, int displayTime);
	virtual ~cMessageBoxThread(void);
};

class cMessageBox {
private:
	static cMutex mutex;
	static cMessageBoxThread *msgboxThread;
	static cPixmap *content;
public:
	static bool Start(int displayTime, cString msg);
	static void Stop(void);
	static void Destroy(void);
};

#endif //__TVGUIDE_MESSAGEBOX_H