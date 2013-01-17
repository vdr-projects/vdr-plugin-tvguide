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
  bool LoadLogo(const char *logo);
  bool LoadEPGImage(int eventID);
private:
  Image osdImage;
};

#endif //_TVGUIDE_IMAGELOADER_H
