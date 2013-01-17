#ifndef __TVGUIDE_OSDMANAGER_H
#define __TVGUIDE_OSDMANAGER_H

class cOsdManager {
	private:
		cOsd *osd;
		int activePixmaps;
	public:
		cOsdManager(void);
		bool setOsd();
		void setBackground();
		void flush() {osd->Flush();};
		cPixmap *requestPixmap(int Layer, const cRect &ViewPort, const cRect &DrawPort = cRect::Null, const char *caller = "anonymous");
		void releasePixmap(cPixmap *pixmap, const char *caller = "anonymous");
		void deleteOsd() {delete osd;};
		int Width() { return osd->Width(); };
		int Height() { return osd->Height(); };
};

#endif //__TVGUIDE_OSDMANAGER_H

cOsdManager::cOsdManager(void) {
	activePixmaps = 0;
}

bool cOsdManager::setOsd() {
	osd = cOsdProvider::NewOsd(cOsd::OsdLeft(), cOsd::OsdTop());
	if (osd) {
		tArea Area = { 0, 0, cOsd::OsdWidth(), cOsd::OsdHeight(),  32 };
		if (osd->SetAreas(&Area, 1) == oeOk) {	
			return true;
		}
	}
	return false;
}

void cOsdManager::setBackground() {
	osd->DrawRectangle(0, 0, cOsd::OsdWidth(), cOsd::OsdHeight(), theme.Color(clrBackgroundOSD));
}
cPixmap *cOsdManager::requestPixmap(int Layer, const cRect &ViewPort, const cRect &DrawPort, const char *caller) {
	if (activePixmaps >= 64)
		return NULL;
	activePixmaps++;
	//esyslog("tvguide: Pixmap angefordert von %s, verwendet: %d", caller, activePixmaps);
	return osd->CreatePixmap(Layer, ViewPort, DrawPort);
	}

void cOsdManager::releasePixmap(cPixmap *pixmap, const char *caller) {
	activePixmaps--;
	//esyslog("tvguide: Pixmap geloescht von %s, verwendet: %d", caller, activePixmaps);	
	osd->DestroyPixmap(pixmap);
}