#ifndef _TVGUIDE_IMAGELOADER_H
#define _TVGUIDE_IMAGELOADER_H

#define X_DISPLAY_MISSING

#include <vdr/osd.h>
#include <vdr/skins.h>
#include <Magick++.h>

using namespace Magick;
 
class cImageLoader {
public:
    cImageLoader();
    ~cImageLoader();
    cImage GetImage();
    bool LoadLogo(const char *logo, int width, int height);
    bool LoadEPGImage(int eventID);
    bool LoadAdditionalEPGImage(cString name);
    bool LoadPoster(const char *poster, int width, int height);
    bool LoadIcon(const char *cIcon, int size);
    bool DrawBackground(tColor back, tColor blend, int width, int height);
private:
    Image buffer;
    Color Argb2Color(tColor col);
    void toLowerCase(std::string &str);
    bool LoadImage(cString FileName, cString Path, cString Extension);
    bool LoadImage(const char *fullpath);
};

#endif //_TVGUIDE_IMAGELOADER_H
