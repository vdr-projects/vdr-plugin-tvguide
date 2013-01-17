#include "messagebox.h"

cMessageBoxThread::cMessageBoxThread(cPixmap *content, int displayTime) {
	this->content = content;
	FrameTime = 30; // ms
	FadeTime = 200; // ms
	this->displayTime = displayTime;
}

cMessageBoxThread::~cMessageBoxThread(void) {
	Cancel(0);
}

void cMessageBoxThread::Action(void) {
	uint64_t Start = cTimeMs::Now();
	while (Running()) {
		uint64_t Now = cTimeMs::Now();
		cPixmap::Lock();
		double t = min(double(Now - Start) / FadeTime, 1.0);
	    int Alpha = t * ALPHA_OPAQUE;
		if (content) {
			content->SetAlpha(Alpha);
			osdManager.flush();
		}
		cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Delta < FrameTime)
           cCondWait::SleepMs(FrameTime - Delta);
		if ((Now - Start) > FadeTime)
			break;
    }
	cCondWait::SleepMs(displayTime - 2*FadeTime);
	Start = cTimeMs::Now();
	while (Running()) {
		uint64_t Now = cTimeMs::Now();
		cPixmap::Lock();
		double t = min(double(Now - Start) / FadeTime, 1.0);
	    int Alpha = (1-t) * ALPHA_OPAQUE;
		if (content) {
			content->SetAlpha(Alpha);
			osdManager.flush();
		}
		cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Delta < FrameTime)
           cCondWait::SleepMs(FrameTime - Delta);
		if ((Now - Start) > FadeTime)
			break;
    }
	osdManager.flush();
}

//--cMessageBox-------------------------------------------------------------
cMutex cMessageBox::mutex;
cMessageBoxThread *cMessageBox::msgboxThread = NULL;
cPixmap *cMessageBox::content = NULL;

bool cMessageBox::Start(int displayTime, cString msg) {
	cMutexLock MutexLock(&mutex);
	int width = (tvguideConfig.osdWidth - 600)/2;
	if (!content) {
		int height = 400;
		content = osdManager.requestPixmap(5, cRect((tvguideConfig.osdWidth - width)/2, 
													(tvguideConfig.osdHeight- height)/2, 
													width, height), 
													cRect::Null, "msgbox");
	}
	if (msgboxThread) {
		delete msgboxThread;
		msgboxThread = NULL;
	}
	if (!msgboxThread) {
		msgboxThread = new cMessageBoxThread(content, displayTime);
		cTextWrapper message;
		message.Set(msg, tvguideConfig.FontMessageBox, width - 40);
		int textHeight = tvguideConfig.FontMessageBox->Height();
		int textLines = message.Lines();
		int height = textLines * (textHeight+20);
		cPixmap::Lock();
		content->SetViewPort(cRect((tvguideConfig.osdWidth - width)/2,(tvguideConfig.osdHeight- height)/2, width, height));
		content->SetAlpha(0);
		content->Fill(theme.Color(clrBorder));
		content->DrawRectangle(cRect(2,2,width-4, height-4), theme.Color(clrBackground));
		int textWidth = 0;
		for (int i=0; i<textLines; i++) {
			textWidth = tvguideConfig.FontMessageBox->Width(message.GetLine(i));
			content->DrawText(cPoint((width - textWidth)/2, 20 + i*textHeight), message.GetLine(i), theme.Color(clrFont), clrTransparent, tvguideConfig.FontMessageBox);
		}
		cPixmap::Unlock();
		msgboxThread->Start();
        return true;
	}
	return false;
}

void cMessageBox::Stop(void) {
	cMutexLock MutexLock(&mutex);
	if (msgboxThread) {
		delete msgboxThread;
		msgboxThread = NULL;
	}
}

void cMessageBox::Destroy(void) {
	cMutexLock MutexLock(&mutex);
	if (msgboxThread) {
		delete msgboxThread;
		msgboxThread = NULL;
	}
	if (content) {
		osdManager.releasePixmap(content, "msgboxDestroy");
		content = NULL;
	}
}
