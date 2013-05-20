#ifndef __TVGUIDE_STYLEDPIXMAP_H
#define __TVGUIDE_STYLEDPIXMAP_H

// --- cStyledPixmap -------------------------------------------------------------

class cStyledPixmap {
private:
	void drawVerticalLine(int x, int yStart, int yStop, tColor col);
	void drawHorizontalLine(int y, int xStart, int xStop, tColor col);
protected:
	cPixmap *pixmap;
	tColor color;
	tColor colorBlending;
	void setPixmap(cPixmap *pixmap);
public:
  cStyledPixmap(void);
  cStyledPixmap(cPixmap *pixmap);
  virtual ~cStyledPixmap(void);
  void drawBackground();
  void drawBlendedBackground();
  void drawSparsedBackground();
  void drawBorder();
  void drawBoldBorder();
  void drawDefaultBorder(int width, int height);
  void drawRoundedCorners(int width, int height, int radius);
  void setColor(tColor color, tColor colorBlending) {this->color = color; this->colorBlending = colorBlending;};
  void SetAlpha(int alpha) {pixmap->SetAlpha(alpha);};
  void DrawText(const cPoint &Point, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font);
  void DrawImage(const cPoint &Point, const cImage &Image);
  void DrawRectangle(const cRect &Rect, tColor Color);
  int Width() {return pixmap->ViewPort().Width();};
  int Height() {return pixmap->ViewPort().Height();};
};

#endif //__TVGUIDE_STYLEDPIXMAP_H