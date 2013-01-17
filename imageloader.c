#include "imageloader.h"
#include <math.h>

using namespace Magick;

cImageLoader::cImageLoader() {
}

cImageLoader::~cImageLoader() {
}

bool cImageLoader::LoadLogo(const char *logo)
{
  try
  {
	int width = tvguideConfig.logoWidth;
	int height = tvguideConfig.logoHeight;
	cString extension;
	if (tvguideConfig.logoExtension == 0) {
		extension = "png";
	} else if (tvguideConfig.logoExtension == 1) {
		extension = "jpg";
	}
	cString Filename = cString::sprintf("%s%s.%s", *tvguideConfig.logoPath, logo, *extension);
    osdImage.read(*Filename);

	if (height != 0 || width != 0) {
		osdImage.sample( Geometry(width, height));
	
	}
	return true;
  }
  catch (...)
  {
    return false;
  }
}

bool cImageLoader::LoadEPGImage(int eventID)
{
  try
  {
	int width = tvguideConfig.epgImageWidth;
	int height = tvguideConfig.epgImageHeight;
	cString Filename = cString::sprintf("%s%d.jpg", *tvguideConfig.epgImagePath, eventID);
    osdImage.read(*Filename);

	if (height != 0 || width != 0)
	  osdImage.sample( Geometry(width, height));
	
	return true;
  }
  catch (...)
  {
    return false;
  }
}


cImage cImageLoader::GetImage()
{
  int w, h;
  w = osdImage.columns();
  h = osdImage.rows();
  cImage image (cSize(w, h));
  const PixelPacket *pixels = osdImage.getConstPixels(0, 0, w, h);
  for (int iy = 0; iy < h; ++iy) {
    for (int ix = 0; ix < w; ++ix) {
      tColor col = (~int(pixels->opacity * 255 / MaxRGB) << 24) 
	    | (int(pixels->green * 255 / MaxRGB) << 8) 
        | (int(pixels->red * 255 / MaxRGB) << 16) 
        | (int(pixels->blue * 255 / MaxRGB) );
      image.SetPixel(cPoint(ix, iy), col);
      ++pixels;
    }
  }
  return image;
}
