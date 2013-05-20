#include <string>
#include <sstream>

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
        osd->DrawRectangle(0, 0, cOsd::OsdWidth() - tvguideConfig.statusHeaderHeight * 16 / 9, tvguideConfig.statusHeaderHeight, theme.Color(clrBackgroundOSD));
        osd->DrawRectangle(0, tvguideConfig.statusHeaderHeight, cOsd::OsdWidth(), cOsd::OsdHeight() - tvguideConfig.statusHeaderHeight, theme.Color(clrBackgroundOSD));    
    }
    else
        osd->DrawRectangle(0, 0, cOsd::OsdWidth(), cOsd::OsdHeight(), theme.Color(clrBackgroundOSD));
}

cPixmap *cOsdManager::requestPixmap(int Layer, const cRect &ViewPort, const cRect &DrawPort) {
	return osd->CreatePixmap(Layer, ViewPort, DrawPort);
}

void cOsdManager::releasePixmap(cPixmap *pixmap) {
    if (!pixmap)
        return;
	osd->DestroyPixmap(pixmap);
}

static std::string CutText(std::string text, int width, const cFont *font) {
    if (width <= font->Size())
        return text.c_str();
    if (font->Width(text.c_str()) < width)
        return text.c_str();
    cTextWrapper twText;
    twText.Set(text.c_str(), font, width);
    std::string cuttedTextNative = twText.GetLine(0);
    std::stringstream sstrText;
    sstrText << cuttedTextNative << "...";
    std::string cuttedText = sstrText.str();
    int actWidth = font->Width(cuttedText.c_str());
    if (actWidth > width) {
        int overlap = actWidth - width;
        int charWidth = font->Width(".");
        if (charWidth == 0)
            charWidth = 1;
        int cutChars = overlap / charWidth;
        if (cutChars > 0) {
            cuttedTextNative = cuttedTextNative.substr(0, cuttedTextNative.length() - cutChars);
            std::stringstream sstrText2;
            sstrText2 << cuttedTextNative << "...";
            cuttedText = sstrText2.str();
        }
    }
    return cuttedText;
}