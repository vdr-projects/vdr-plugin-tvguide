#include "styledpixmap.h"

cStyledPixmap::cStyledPixmap(void) {
    pixmap = NULL;
}

cStyledPixmap::cStyledPixmap(cPixmap *pixmap) {
    this->pixmap = pixmap;
}

cStyledPixmap::~cStyledPixmap(void) {
    if (pixmap)
        osdManager.releasePixmap(pixmap);
}

void cStyledPixmap::setPixmap(cPixmap *pixmap) {
    if (pixmap) {
        this->pixmap = pixmap;
    }
}

void cStyledPixmap::drawBackground() {
    if (tvguideConfig.useBlending == 1){
        drawBlendedBackground();
    } else if (tvguideConfig.useBlending == 2){
        drawSparsedBackground();
    } else {
        pixmap->Fill(color);
    }
}

void cStyledPixmap::drawBlendedBackground() {
    int width = pixmap->ViewPort().Width();
    int height = pixmap->ViewPort().Height();
    pixmap->Fill(color);
    int numSteps = 64;
    int alphaStep = 0x04;
    if (height < 30)
        return;
    else if (height < 100) {
        numSteps = 32;
        alphaStep = 0x08;
    }
    int stepY = 0.5*height / numSteps;
    if (stepY == 0) stepY = 1;
    int alpha = 0x00;
    tColor clr;
    for (int i = 0; i<numSteps; i++) {
        clr = AlphaBlend(color, colorBlending, alpha);
        pixmap->DrawRectangle(cRect(0,i*stepY,width,stepY), clr);
        alpha += alphaStep;
    }
}

void cStyledPixmap::drawSparsedBackground() {
    int width = pixmap->ViewPort().Width();
    int height = pixmap->ViewPort().Height();
    cImageLoader imgLoader;
    if (imgLoader.DrawBackground(colorBlending, color, width, height))
        pixmap->DrawImage(cPoint(0,0), imgLoader.GetImage());

}

void cStyledPixmap::drawBorder() {
    int width = pixmap->ViewPort().Width();
    int height = pixmap->ViewPort().Height();
    
    drawDefaultBorder(width, height);
    if (tvguideConfig.roundedCorners) {
        int borderRadius = 12;
        drawRoundedCorners(width, height, borderRadius);
    }
}

void cStyledPixmap::drawDefaultBorder(int width, int height) {
    pixmap->DrawRectangle(cRect(0,0,width,2), theme.Color(clrBackground));          //top
    pixmap->DrawRectangle(cRect(0,0,2,height), theme.Color(clrBackground));         //left
    pixmap->DrawRectangle(cRect(0,height-2,width,2), theme.Color(clrBackground));   //bottom
    pixmap->DrawRectangle(cRect(width-2,0,2,height), theme.Color(clrBackground));   //right
    
    pixmap->DrawRectangle(cRect(2,2,width-4,1), theme.Color(clrBorder));            //top
    pixmap->DrawRectangle(cRect(2,2,1,height-4), theme.Color(clrBorder));           //left
    pixmap->DrawRectangle(cRect(2,height-3,width-4,1), theme.Color(clrBorder));     //bottom
    pixmap->DrawRectangle(cRect(width-3,2,1,height-4), theme.Color(clrBorder));     //right
}

void cStyledPixmap::drawBoldBorder() {
    int width = pixmap->ViewPort().Width();
    int height = pixmap->ViewPort().Height();
    pixmap->DrawRectangle(cRect(0,0,width,2), theme.Color(clrBorder));          //top
    pixmap->DrawRectangle(cRect(0,0,2,height), theme.Color(clrBorder));         //left
    pixmap->DrawRectangle(cRect(0,height-2,width,2), theme.Color(clrBorder));   //bottom
    pixmap->DrawRectangle(cRect(width-2,0,2,height), theme.Color(clrBorder));   //right
}

void cStyledPixmap::drawRoundedCorners(int width, int height, int radius) {
    pixmap->DrawEllipse(cRect(2,2,radius,radius), theme.Color(clrBorder), -2);
    pixmap->DrawEllipse(cRect(1,1,radius,radius), theme.Color(clrBackground), -2);

    pixmap->DrawEllipse(cRect(width-radius - 2,2,radius,radius), theme.Color(clrBorder), -1);
    pixmap->DrawEllipse(cRect(width-radius - 1,1,radius,radius), theme.Color(clrBackground), -1);
    
    if( height > 2*radius) {
        pixmap->DrawEllipse(cRect(2,height-radius - 2,radius,radius), theme.Color(clrBorder), -3);
        pixmap->DrawEllipse(cRect(1,height-radius - 1,radius,radius), theme.Color(clrBackground), -3);
        
        pixmap->DrawEllipse(cRect(width-radius - 2,height-radius - 2,radius,radius), theme.Color(clrBorder), -4);
        pixmap->DrawEllipse(cRect(width-radius - 1,height-radius - 1,radius,radius), theme.Color(clrBackground), -4);
    }
}

void cStyledPixmap::drawVerticalLine(int x, int yStart, int yStop, tColor col) {
    for (int y = yStart; y <= yStop; y++) {
        pixmap->DrawPixel(cPoint(x,y), col);
    }
}

void cStyledPixmap::drawHorizontalLine(int y, int xStart, int xStop, tColor col) {
    for (int x = xStart; x <= xStop; x++) {
        pixmap->DrawPixel(cPoint(x,y), col);
    }
}

void cStyledPixmap::DrawText(const cPoint &Point, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font) {
    pixmap->DrawText(Point, s, ColorFg, ColorBg, Font);
}

void cStyledPixmap::DrawImage(const cPoint &Point, const cImage &Image) {
    pixmap->DrawImage(Point, Image);
}

void cStyledPixmap::DrawRectangle(const cRect &Rect, tColor Color) {
    pixmap->DrawRectangle(Rect,Color);
}
