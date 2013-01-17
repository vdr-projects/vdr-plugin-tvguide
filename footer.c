#include "footer.h"

cFooter::cFooter() {
	int buttonHeight= tvguideConfig.footerHeight - 20;
	textY = (buttonHeight - tvguideConfig.FontButton->Height())/2;
	int distanceX = 20;
	buttonWidth = (tvguideConfig.osdWidth - tvguideConfig.timeColWidth-5*distanceX)/4;
	int startX = tvguideConfig.timeColWidth + distanceX;
	int Y = tvguideConfig.osdHeight - tvguideConfig.footerHeight + (tvguideConfig.footerHeight - buttonHeight)/2;
	
	buttonRed = new cStyledPixmap(osdManager.requestPixmap(3, cRect(startX, Y, buttonWidth, buttonHeight), cRect::Null, "btnRed"), "btnRed");
	buttonGreen = new cStyledPixmap(osdManager.requestPixmap(3, cRect(startX + buttonWidth + distanceX, Y, buttonWidth, buttonHeight), cRect::Null, "btnGreen"), "btnGreen");
	buttonYellow = new cStyledPixmap(osdManager.requestPixmap(3, cRect(startX + 2*(buttonWidth + distanceX), Y, buttonWidth, buttonHeight), cRect::Null, "btnYellow"), "btnYellow");
	buttonBlue = new cStyledPixmap(osdManager.requestPixmap(3, cRect(startX + 3*(buttonWidth + distanceX), Y, buttonWidth, buttonHeight), cRect::Null, "btnBlue"), "btnBlue");
}

cFooter::~cFooter(void) {
	delete buttonRed;
	delete buttonGreen;
	delete buttonYellow;
	delete buttonBlue;
}

void cFooter::drawRedButton() {
	buttonRed->setColor(theme.Color(clrButtonRed), theme.Color(clrButtonRedBlending));
	buttonRed->drawBackground();
	buttonRed->drawBorder();
	cString text(tr("Set Timer"));
	int width = tvguideConfig.FontButton->Width(*(text));
	buttonRed->DrawText(cPoint((buttonWidth-width)/2, textY), *(text), theme.Color(clrFontButtons), clrTransparent, tvguideConfig.FontButton);

}

void cFooter::drawGreenButton() {
	buttonGreen->setColor(theme.Color(clrButtonGreen), theme.Color(clrButtonGreenBlending));
	buttonGreen->drawBackground();
	buttonGreen->drawBorder();
	cString text = cString::sprintf("%d %s", tvguideConfig.jumpChannels, tr("Channels back"));
	int width = tvguideConfig.FontButton->Width(*text);
	buttonGreen->DrawText(cPoint((buttonWidth-width)/2, textY), *text, theme.Color(clrFontButtons), clrTransparent, tvguideConfig.FontButton);
}

void cFooter::drawYellowButton() {
	buttonYellow->setColor(theme.Color(clrButtonYellow), theme.Color(clrButtonYellowBlending));
	buttonYellow->drawBackground();
	buttonYellow->drawBorder();
	cString text = cString::sprintf("%d %s", tvguideConfig.jumpChannels, tr("Channels forward"));
	int width = tvguideConfig.FontButton->Width(*text);
	buttonYellow->DrawText(cPoint((buttonWidth-width)/2, textY), *text, theme.Color(clrFontButtons), clrTransparent, tvguideConfig.FontButton);
}

void cFooter::drawBlueButton() {
	buttonBlue->setColor(theme.Color(clrButtonBlue), theme.Color(clrButtonBlueBlending));
	buttonBlue->drawBackground();
	buttonBlue->drawBorder();
	cString text(tr("Switch to Channel"));
	int width = tvguideConfig.FontButton->Width(*(text));
	buttonBlue->DrawText(cPoint((buttonWidth-width)/2, textY), *(text), theme.Color(clrFontButtons), clrTransparent, tvguideConfig.FontButton);
}
