#ifndef __TVGUIDE_OSDMANAGER_H
#define __TVGUIDE_OSDMANAGER_H

class cOsdManager {
    private:
        cOsd *osd;
    public:
        cOsdManager(void);
        bool setOsd();
        void setBackground();
        void flush() {osd->Flush();};
        cPixmap *requestPixmap(int Layer, const cRect &ViewPort, const cRect &DrawPort = cRect::Null);
        void releasePixmap(cPixmap *pixmap);
        void deleteOsd() {delete osd;};
        int Width() { return osd->Width(); };
        int Height() { return osd->Height(); };
        int Top() { return osd->Top(); };
        int Left() { return osd->Left(); };
};

#endif //__TVGUIDE_OSDMANAGER_H

cOsdManager::cOsdManager(void) {
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
    if (tvguideConfig.displayStatusHeader && tvguideConfig.scaleVideo) {
        int widthStatus = cOsd::OsdWidth() - tvguideConfig.statusHeaderHeight * 16 / 9;
        osd->DrawRectangle(0, 0, widthStatus, tvguideConfig.statusHeaderHeight, theme.Color(clrBackgroundOSD));
        osd->DrawRectangle(0, tvguideConfig.statusHeaderHeight, Width(), Height(), theme.Color(clrBackgroundOSD));    
    }
    else
        osd->DrawRectangle(0, 0, Width(), Height(), theme.Color(clrBackgroundOSD));
}

cPixmap *cOsdManager::requestPixmap(int Layer, const cRect &ViewPort, const cRect &DrawPort) {
    return osd->CreatePixmap(Layer, ViewPort, DrawPort);
}

void cOsdManager::releasePixmap(cPixmap *pixmap) {
    if (!pixmap)
        return;
    osd->DestroyPixmap(pixmap);
}