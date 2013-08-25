#include "imageloader.h"
#include <math.h>
#include <string>
#include <dirent.h>
#include <iostream>

using namespace Magick;

cImageLoader::cImageLoader() {
    InitializeMagick(NULL);
}

cImageLoader::~cImageLoader() {
}

bool cImageLoader::LoadLogo(const char *logo, int width, int height) {

    if ((width == 0)||(height==0))
        return false;
    std::string logoLower = logo;
    toLowerCase(logoLower);
    cString extension;
    if (tvguideConfig.logoExtension == 0) {
        extension = "png";
    } else if (tvguideConfig.logoExtension == 1) {
        extension = "jpg";
    }
    if (!LoadImage(logoLower.c_str(), tvguideConfig.logoPath, extension))
        return false;
    buffer.sample( Geometry(width, height));
    return true;
}

bool cImageLoader::LoadEPGImage(int eventID) {
    int width = tvguideConfig.epgImageWidth;
    int height = tvguideConfig.epgImageHeight;
    if ((width == 0)||(height==0))
        return false;
    if (!LoadImage(cString::sprintf("%d", eventID), tvguideConfig.epgImagePath, "jpg"))
        return false;
    if (height != 0 || width != 0) {
        buffer.sample( Geometry(width, height));
    }
    return true;
}

bool cImageLoader::LoadAdditionalEPGImage(cString name) {
    int width = tvguideConfig.epgImageWidthLarge;
    int height = tvguideConfig.epgImageHeightLarge;
    if ((width == 0)||(height==0))
        return false;
    bool success = false;
    success = LoadImage(name, tvguideConfig.epgImagePath, "jpg");
    if (!success)
        return false;
    if (height != 0 || width != 0) {
        buffer.sample( Geometry(width, height));
    }
    return true;
}

bool cImageLoader::LoadPoster(const char *poster, int width, int height) {
    if (LoadImage(poster)) {
        buffer.sample(Geometry(width, height));
        return true;
    }
    return false;
}

bool cImageLoader::LoadIcon(const char *cIcon, int size) {
    if (size==0)
        return false;
    bool success = false;
    success = LoadImage(cString(cIcon), tvguideConfig.iconPath, "png");
    if (!success)
        return false;
    buffer.sample(Geometry(size, size));
    return true;
}

bool cImageLoader::DrawBackground(tColor back, tColor blend, int width, int height) {
    if ((width < 1) || (height < 1) || (width > 1920) || (height > 1080))
        return false;
    Color Back = Argb2Color(back);
    Color Blend = Argb2Color(blend);
    Image tmp(Geometry(width, height), Blend);
    double arguments[9] = {0.0,(double)height,0.0,-1*(double)width,0.0,0.0,1.5*(double)width,0.0,1.0};
    tmp.sparseColor(MatteChannel, BarycentricColorInterpolate, 9, arguments);
    Image tmp2(Geometry(width, height), Back);
    tmp.composite(tmp2, 0, 0, OverlayCompositeOp);
    buffer = tmp;
    return true;
}

cImage cImageLoader::GetImage() {
    int w, h;
    w = buffer.columns();
    h = buffer.rows();
    cImage image (cSize(w, h));
    const PixelPacket *pixels = buffer.getConstPixels(0, 0, w, h);
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

Color cImageLoader::Argb2Color(tColor col) {
    tIndex alpha = (col & 0xFF000000) >> 24;
    tIndex red = (col & 0x00FF0000) >> 16;
    tIndex green = (col & 0x0000FF00) >> 8;
    tIndex blue = (col & 0x000000FF);
    Color color(MaxRGB*red/255, MaxRGB*green/255, MaxRGB*blue/255, MaxRGB*(0xFF-alpha)/255);
    return color;
}

void cImageLoader::toLowerCase(std::string &str) {
    const int length = str.length();
    for(int i=0; i < length; ++i) {
        str[i] = std::tolower(str[i]);
    }
}

bool cImageLoader::LoadImage(cString FileName, cString Path, cString Extension) {
    try {
        cString File = cString::sprintf("%s%s.%s", *Path, *FileName, *Extension);
        //dsyslog("tvguide: trying to load: %s", *File);
        buffer.read(*File);
        //dsyslog("tvguide: %s sucessfully loaded", *File);
    } catch (...) {     
        return false;
    }
    return true;
}

bool cImageLoader::LoadImage(const char *fullpath) {
    try {
        //dsyslog("tvguide: trying to load: %s", fullpath);
        buffer.read(fullpath);
        //dsyslog("tvguide: %s sucessfully loaded", fullpath);
    } catch (...) {     
        return false;
    }
    return true;
}