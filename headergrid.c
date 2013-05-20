#include "headergrid.h"

cHeaderGrid::cHeaderGrid(void) : cGrid(NULL) {
    pixmap = NULL;
    pixmapLogo = NULL;
}

cHeaderGrid::~cHeaderGrid(void) {
    osdManager.releasePixmap(pixmapLogo);
}

void cHeaderGrid::createBackground(int num) {
    color = theme.Color(clrHeader);
    colorBlending = theme.Color(clrHeaderBlending);
    pixmap = osdManager.requestPixmap(2, cRect( tvguideConfig.timeColWidth + num*tvguideConfig.colWidth, 
                                                tvguideConfig.statusHeaderHeight, 
                                                tvguideConfig.colWidth, 
                                                tvguideConfig.headerHeight)
                                       , cRect::Null);
	if (!pixmap) {
		return;
	}
    pixmapLogo = osdManager.requestPixmap(3, cRect( tvguideConfig.timeColWidth + num*tvguideConfig.colWidth, 
                                                    tvguideConfig.statusHeaderHeight, 
                                                    tvguideConfig.colWidth, 
                                                    tvguideConfig.headerHeight)
                                           , cRect::Null);
    if (!pixmapLogo) {
		return;
	}
    pixmapLogo->Fill(clrTransparent);
	drawBackground();
}

void cHeaderGrid::drawChannel(const cChannel *channel) {
    cTextWrapper tw;
	cString headerText = cString::sprintf("%d - %s", channel->Number(), channel->Name());
	tw.Set(*headerText, tvguideConfig.FontHeader, tvguideConfig.colWidth - 8);
	int lines = tw.Lines();
	int lineHeight = tvguideConfig.FontHeader->Height();
	int yStart = (tvguideConfig.headerHeight - lines*lineHeight)/2 + 8;
	if (!tvguideConfig.hideChannelLogos) {
		cImageLoader imgLoader;
		if (imgLoader.LoadLogo(channel->Name())) {
				cImage logo = imgLoader.GetImage();
				int logoX = (tvguideConfig.colWidth - tvguideConfig.logoWidth)/2;
				pixmapLogo->DrawImage(cPoint(logoX, 5), logo);
		}
		yStart = tvguideConfig.logoHeight + 8;
	}
	for (int i=0; i<lines; i++) {
		int textWidth = tvguideConfig.FontHeader->Width(tw.GetLine(i));
		int xText = (tvguideConfig.colWidth - textWidth) / 2;
		if (xText < 0) 
			xText = 0;
		pixmap->DrawText(cPoint(xText, yStart + i*lineHeight), tw.GetLine(i), theme.Color(clrFontHeader), clrTransparent, tvguideConfig.FontHeader);
	}
	drawBorder();
}

void cHeaderGrid::setPosition(int num) {
	pixmap->SetViewPort(cRect(      tvguideConfig.timeColWidth + num*tvguideConfig.colWidth, 
                                    tvguideConfig.statusHeaderHeight, 
                                    tvguideConfig.colWidth, 
                                    tvguideConfig.headerHeight));
	pixmapLogo->SetViewPort(cRect(  tvguideConfig.timeColWidth + num*tvguideConfig.colWidth, 
                                    tvguideConfig.statusHeaderHeight, 
                                    tvguideConfig.colWidth, 
                                    tvguideConfig.headerHeight));
}
