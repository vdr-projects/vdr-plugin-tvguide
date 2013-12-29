#include <math.h>
#include <vdr/remote.h>
#include "imageloader.h"
#include "imagecache.h"
#include "tools.h"
#include "recmenuitem.h"

// --- cRecMenuItem  -------------------------------------------------------------

cRecMenuItem::cRecMenuItem(void) {
    height = 0;
    action = rmsNotConsumed;
    drawn = false;
    font = fontManager.FontRecMenuItem;
    fontSmall = fontManager.FontRecMenuItemSmall;
}

cRecMenuItem::~cRecMenuItem(void) {
}

void cRecMenuItem::SetGeometry(int x, int y, int width) {
    this->x = x;
    this->y = y;
    this->width = width;
    
}

void cRecMenuItem::SetPixmaps(void) {
    if (!pixmap)
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
    else
        pixmap->SetViewPort(cRect(x, y, width, height));
}

void cRecMenuItem::setBackground(void) {
    if (tvguideConfig.style == eStyleGraphical) {
        drawBackgroundGraphical(bgButton, active);
        colorTextBack = clrTransparent;
        colorText = (active)?theme.Color(clrFontActive):theme.Color(clrFont);
    } else {
        if (active) {
            color = theme.Color(clrHighlight);
            colorBlending = theme.Color(clrHighlightBlending);
            colorText = theme.Color(clrFontActive);
        } else {
            color = theme.Color(clrGrid1);
            colorBlending = theme.Color(clrGrid1Blending);
            colorText = theme.Color(clrFont);
        }
        colorTextBack = (tvguideConfig.style == eStyleFlat)?color:clrTransparent;
        drawBackground();
        drawBorder();
    }
}

// --- cRecMenuItemButton  -------------------------------------------------------

cRecMenuItemButton::cRecMenuItemButton(const char *text, eRecMenuState action, bool active, bool halfWidth, bool alignLeft) {
    selectable = true;
    this->text = text;
    this->action = action;
    this->active = active;
    height = 3 * font->Height() / 2;
    this->halfWidth = halfWidth;
    this->alignLeft = alignLeft;
}

cRecMenuItemButton::~cRecMenuItemButton(void) {
}

int cRecMenuItemButton::GetWidth(void) {
    return font->Width(*text);    
}

void cRecMenuItemButton::SetPixmaps(void) {
    if (halfWidth) {
        x += width / 4;
        width = width / 2;
    }
    if (!pixmap)
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
    else
        pixmap->SetViewPort(cRect(x, y, width, height));
}

void cRecMenuItemButton::Draw(void) {
    int y = (height - font->Height()) / 2;
    int x;
    if (!alignLeft)
        x = (width - font->Width(*text)) / 2;
    else
        x = 10;
    pixmap->DrawText(cPoint(x, y), *text, colorText, colorTextBack, font);
}

eRecMenuState cRecMenuItemButton::ProcessKey(eKeys Key) {
    switch (Key & ~k_Repeat) {
        case kOk:
            return action;
            break;
        default:
            break;
    }
    return rmsNotConsumed;
}


// --- cRecMenuItemButtonYesNo  -------------------------------------------------------
cRecMenuItemButtonYesNo::cRecMenuItemButtonYesNo(cString textYes,
                                                 cString textNo,
                                                 eRecMenuState actionYes, 
                                                 eRecMenuState actionNo,
                                                 bool active) {
    selectable = true;
    this->textYes = textYes;
    this->textNo = textNo;
    this->action = actionYes;
    this->actionNo = actionNo;
    this->active = active;
    yesActive = true;
    height = 3 * font->Height() / 2;
    pixmapNo = NULL;
}

cRecMenuItemButtonYesNo::~cRecMenuItemButtonYesNo(void) {
    if (pixmapNo)
        delete pixmapNo;
}

void cRecMenuItemButtonYesNo::SetPixmaps(void) {
    int buttonWidth = 44 * width / 100;
    int yesX = x + width / 25;
    int noX = x + 52 * width / 100;
    int yPixmaps = y + geoManager.borderRecMenus / 2;
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(yesX, yPixmaps, buttonWidth, height));
        pixmapNo = new cStyledPixmap(osdManager.requestPixmap(4, cRect(noX, yPixmaps, buttonWidth, height)));    
    } else {
        pixmap->SetViewPort(cRect(x, yPixmaps, width, height));
        pixmapNo->SetViewPort(cRect(x, yPixmaps, width, height));
    }
}

void cRecMenuItemButtonYesNo::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapNo->SetLayer(-1);
}

void cRecMenuItemButtonYesNo::Show(void) { 
    pixmap->SetLayer(4);
    pixmapNo->SetLayer(4);
}

void cRecMenuItemButtonYesNo::setBackground() {
    if (tvguideConfig.style == eStyleGraphical) {
        drawBackgroundGraphical(bgButton, yesActive&&active);
        colorTextBack = clrTransparent;
        colorTextNoBack = clrTransparent;
        colorText = (active&&yesActive)?theme.Color(clrFontActive):theme.Color(clrFont);
        colorTextNo = (active&&!yesActive)?theme.Color(clrFontActive):theme.Color(clrFont);
        pixmapNo->drawBackgroundGraphical(bgButton, active&&!yesActive);
    } else {
        tColor colorNoBack;
        if (active) {
            if (yesActive) {
                color = theme.Color(clrHighlight);
                colorBlending = theme.Color(clrHighlightBlending);
                colorText = theme.Color(clrFontActive);
                colorNoBack = theme.Color(clrGrid1);
                pixmapNo->setColor( colorNoBack,
                                    theme.Color(clrGrid1Blending));
                colorTextNo = theme.Color(clrFont);
            } else {
                color = theme.Color(clrGrid1);
                colorBlending = theme.Color(clrGrid1Blending);
                colorText = theme.Color(clrFont);
                colorNoBack = theme.Color(clrHighlight);
                pixmapNo->setColor( colorNoBack,
                                    theme.Color(clrHighlightBlending));
                colorTextNo = theme.Color(clrFontActive);
            }
        } else {
            color = theme.Color(clrGrid1);
            colorBlending = theme.Color(clrGrid1Blending);
            colorText = theme.Color(clrFont);
            colorNoBack = theme.Color(clrGrid1);
            pixmapNo->setColor( colorNoBack,
                                theme.Color(clrGrid1Blending));
            colorTextNo = theme.Color(clrFont);
        }
        colorTextBack = (tvguideConfig.style == eStyleFlat)?color:clrTransparent;
        colorTextNoBack = (tvguideConfig.style == eStyleFlat)?colorNoBack:clrTransparent;
        drawBackground();
        drawBorder();
        pixmapNo->drawBackground();
        pixmapNo->drawBorder();
    }
}

void cRecMenuItemButtonYesNo::Draw(void) {
    int textYesX = (pixmap->ViewPort().Width() - font->Width(*textYes)) / 2;
    int textNoX = (pixmapNo->Width() - font->Width(*textNo)) / 2;
    int textY = (height - font->Height()) / 2;
    pixmap->DrawText(cPoint(textYesX, textY), *textYes, colorText, colorTextBack, font);
    pixmapNo->DrawText(cPoint(textNoX, textY), *textNo, colorTextNo, colorTextNoBack, font);
}

eRecMenuState cRecMenuItemButtonYesNo::ProcessKey(eKeys Key) {
    switch (Key & ~k_Repeat) {
        case kLeft:
            if (!yesActive) {
                yesActive = true;
                setBackground();
                Draw();
                return rmsConsumed;
            } else
                return rmsNotConsumed;
            break;
        case kRight:
            if (yesActive) {
                yesActive = false;
                setBackground();
                Draw();
                return rmsConsumed;
            } else
                return rmsNotConsumed;
            break;
        case kOk:
            if (yesActive)
                return action;
            return actionNo;
            break;
        default:
            break;
    }
    return rmsNotConsumed;
}

// --- cRecMenuItemInfo  -------------------------------------------------------
cRecMenuItemInfo::cRecMenuItemInfo(const char *text) {
    selectable = false;
    active = false;
    this->text = text;
    border = 10;
}

cRecMenuItemInfo::~cRecMenuItemInfo(void) {
}

void cRecMenuItemInfo::CalculateHeight(int textWidth) {
    wrapper.Set(*text, font, textWidth);
    height = font->Height() * wrapper.Lines() + 2*border;
}

void cRecMenuItemInfo::setBackground(void) {
    pixmap->Fill(clrTransparent);
    cRecMenuItem::setBackground();
}

void cRecMenuItemInfo::Draw(void) {
    int lines = wrapper.Lines();
    int lineHeight = font->Height();
    int x = 0;
    int y = border;
    for (int i = 0; i < lines; i++) {
        x = (width - font->Width(wrapper.GetLine(i))) / 2;
        pixmap->DrawText(cPoint(x,y), wrapper.GetLine(i), theme.Color(clrFont), clrTransparent, font);
        y += lineHeight;
    }
}

// --- cRecMenuItemInt  -------------------------------------------------------
cRecMenuItemInt::cRecMenuItemInt(cString text,
                                int initialVal,
                                int minVal,
                                int maxVal,
                                bool active) {
    selectable = true;
    this->text = text;
    this->currentVal = initialVal;
    this->minVal = minVal;
    this->maxVal = maxVal;
    this->active = active;
    height = 3 * font->Height() / 2;
    pixmapVal = NULL;
    fresh = true;
}

cRecMenuItemInt::~cRecMenuItemInt(void) {
    if (pixmapVal)
        osdManager.releasePixmap(pixmapVal);
}

void cRecMenuItemInt::SetPixmaps(void) {
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
        pixmapVal = osdManager.requestPixmap(5, cRect(x, y, width, height));
    } else {
        pixmap->SetViewPort(cRect(x, y, width, height));
        pixmapVal->SetViewPort(cRect(x, y, width, height));
    }
}

void cRecMenuItemInt::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapVal->SetLayer(-1);
}

void cRecMenuItemInt::Show(void) { 
    pixmap->SetLayer(4);
    pixmapVal->SetLayer(5);
}

void cRecMenuItemInt::setBackground() {
    cRecMenuItem::setBackground();
    fresh = true;
}

void cRecMenuItemInt::Draw(void) {
    int textY = (height - font->Height()) / 2;
    pixmap->DrawText(cPoint(10, textY), *text, colorText, colorTextBack, font);
    DrawValue();    
}

void cRecMenuItemInt::DrawValue(void) {
    pixmapVal->Fill(clrTransparent);
    cString textVal = cString::sprintf("%d", currentVal);
    int textX = width - font->Width(*textVal) - 10;
    int textY = (height - font->Height()) / 2;
    pixmapVal->DrawText(cPoint(textX, textY), *textVal, colorText, clrTransparent, font);
}

eRecMenuState cRecMenuItemInt::ProcessKey(eKeys Key) {
    int oldValue = currentVal;
    switch (Key & ~k_Repeat) {
        case kLeft:
            fresh = true;
            if (currentVal > minVal) {
                currentVal--;
                DrawValue();
            }
            return rmsConsumed;
            break;
        case kRight:
            fresh = true;
            if (currentVal < maxVal) {
                currentVal++;
                DrawValue();
            }
            return rmsConsumed;
            break;
        case k0 ... k9:
            if (fresh) {
               currentVal = 0;
               fresh = false;
            }
            currentVal = currentVal * 10 + (Key - k0);
            if (!((currentVal >= minVal) &&  (currentVal <= maxVal)))
                currentVal = oldValue;
            DrawValue();
            return rmsConsumed;
            break;
        default:
            break;
    }
    return rmsNotConsumed;
}

// --- cRecMenuItemBool  -------------------------------------------------------
cRecMenuItemBool::cRecMenuItemBool(cString text,
                                   bool initialVal,
                                   bool refresh,
                                   bool active) {
    selectable = true;
    this->text = text;
    this->yes = initialVal;
    this->refresh = refresh;
    this->active = active;
    height = 3 * font->Height() / 2;
    pixmapVal = NULL;
}

cRecMenuItemBool::~cRecMenuItemBool(void) {
    if (pixmapVal)
        osdManager.releasePixmap(pixmapVal);
}

void cRecMenuItemBool::SetPixmaps(void) {
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
        pixmapVal = osdManager.requestPixmap(5, cRect(x, y, width, height));
    } else {
        pixmap->SetViewPort(cRect(x, y, width, height));
        pixmapVal->SetViewPort(cRect(x, y, width, height));
    }
}

void cRecMenuItemBool::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapVal->SetLayer(-1);
}

void cRecMenuItemBool::Show(void) { 
    pixmap->SetLayer(4);
    pixmapVal->SetLayer(5);
}

void cRecMenuItemBool::Draw(void) {
    int textY = (height - font->Height()) / 2;
    pixmap->DrawText(cPoint(10, textY), *text, colorText, colorTextBack, font);
    DrawValue();    
}

void cRecMenuItemBool::DrawValue(void) {
    pixmapVal->Fill(clrTransparent);
    std::string strIcon = yes?"yes":"no";
    int iconSize = height - 8;
    int iconX = width - iconSize - 10;
    int iconY = (height - iconSize) / 2;
    
    cImage *imgYesNo = imgCache.GetIcon(strIcon, iconSize, iconSize);
    if (imgYesNo) {
        pixmapVal->DrawImage(cPoint(iconX, iconY), *imgYesNo);
    }
}

eRecMenuState cRecMenuItemBool::ProcessKey(eKeys Key) {
    switch (Key & ~k_Repeat) {
        case kLeft:
        case kRight:
            yes = !yes;
            DrawValue();
            if (refresh)
                return rmsRefresh;
            else
                return rmsConsumed;
            break;
        default:
            break;
    }
    return rmsNotConsumed;
}

// --- cRecMenuItemSelect  -------------------------------------------------------
cRecMenuItemSelect::cRecMenuItemSelect(cString text,
                                       const char * const *Strings,
                                       int initialVal,
                                       int numValues,
                                       bool active) {
    selectable = true;
    this->text = text;
    strings = Strings;
    this->numValues = numValues;
    if ((initialVal < 0) || (initialVal > numValues-1))
        this->currentVal = 0;
    else
        this->currentVal = initialVal;
    this->active = active;
    height = 3 * font->Height() / 2;
    pixmapVal = NULL;
}

cRecMenuItemSelect::~cRecMenuItemSelect(void) {
    if (pixmapVal)
        osdManager.releasePixmap(pixmapVal);
}

void cRecMenuItemSelect::SetPixmaps(void) {
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
        pixmapVal = osdManager.requestPixmap(5, cRect(x, y, width, height));
    } else {
        pixmap->SetViewPort(cRect(x, y, width, height));
        pixmapVal->SetViewPort(cRect(x, y, width, height));
    }
}

void cRecMenuItemSelect::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapVal->SetLayer(-1);
}

void cRecMenuItemSelect::Show(void) { 
    pixmap->SetLayer(4);
    pixmapVal->SetLayer(5);
}

void cRecMenuItemSelect::Draw(void) {
    int textY = (height - font->Height()) / 2;
    pixmap->DrawText(cPoint(10, textY), *text, colorText, colorTextBack, font);
    DrawValue();    
}

void cRecMenuItemSelect::DrawValue(void) {
    pixmapVal->Fill(clrTransparent);
    const char *textVal = strings[currentVal];
    int iconSize = min(128, height);
    int textX = width - font->Width(textVal) - iconSize;
    int textY = (height - font->Height()) / 2;
    pixmapVal->DrawText(cPoint(textX, textY), textVal, colorText, clrTransparent, font);
    int iconLeftX = textX - iconSize;
    int iconRightX = width - iconSize;
    int iconY = (height - iconSize) / 2;
    
    cImage *imgLeft = imgCache.GetIcon("arrow_left", iconSize, iconSize);
    if (imgLeft) {
        pixmapVal->DrawImage(cPoint(iconLeftX, iconY), *imgLeft);
    }
    cImage *imgRight = imgCache.GetIcon("arrow_right", iconSize, iconSize);
    if (imgRight) {
        pixmapVal->DrawImage(cPoint(iconRightX, iconY), *imgRight);
    }
}

eRecMenuState cRecMenuItemSelect::ProcessKey(eKeys Key) {
    int oldValue = currentVal;
    switch (Key & ~k_Repeat) {
        case kLeft:
            currentVal--;
            if (currentVal<0)
                currentVal = numValues - 1;
            DrawValue();
            return rmsConsumed;
            break;
        case kRight:
            currentVal = (currentVal+1)%numValues;
            DrawValue();
            return rmsConsumed;
            break;
        default:
            break;
    }
    return rmsNotConsumed;
}

// --- cRecMenuItemText  -------------------------------------------------------
cRecMenuItemText::cRecMenuItemText(cString title,
                                   char *initialVal,
                                   int length,
                                   bool active) {
    selectable = true;
    this->title = title;
    value = initialVal;
    this->active = active;
    height = 3 * font->Height();
    pixmapVal = NULL;
    pixmapKeyboard = NULL;
    pixmapKeyboardHighlight = NULL;
    pixmapKeyboardIcons = NULL;
    keyboardWidth = 50;
    gridHeight = 3 * fontSmall->Height();
    keyboardHeight = 5 * gridHeight;
    
    this->length = length;
    allowed = trVDR(FileNameChars);
    pos = -1;
    offset = 0;
    insert = uppercase = false;
    newchar = true;
    lengthUtf8 = 0;
    valueUtf8 = NULL;
    allowedUtf8 = NULL;
    charMapUtf8 = NULL;
    currentCharUtf8 = NULL;
    lastKey = kNone;
    keyboardDrawn = false;
}

cRecMenuItemText::~cRecMenuItemText(void) {
    if (pixmapVal)
        osdManager.releasePixmap(pixmapVal);
    if (pixmapKeyboard)
        delete pixmapKeyboard;
    if (pixmapKeyboardHighlight)
        osdManager.releasePixmap(pixmapKeyboardHighlight);
    if (pixmapKeyboardIcons)
        osdManager.releasePixmap(pixmapKeyboardIcons);    
    delete[] valueUtf8;
    delete[] allowedUtf8;
    delete[] charMapUtf8;
}

void cRecMenuItemText::SetPixmaps(void) {
    int xPixmapVal = x + 20;
    int yPixmapVal = y + height / 2 + 10;
    int widthPixmapVal = width - 40;
    int heightPixmapVal = height / 2 - 20;
    int keyboardX = x + (100 - keyboardWidth)*width / 100;
    int keyboardY = y + height;
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
        pixmapVal = osdManager.requestPixmap(5, cRect(xPixmapVal, yPixmapVal, widthPixmapVal, heightPixmapVal));
        pixmapKeyboard = new cStyledPixmap(osdManager.requestPixmap(-1, cRect(keyboardX, keyboardY, keyboardWidth*width/100, keyboardHeight)));
        pixmapKeyboardHighlight = osdManager.requestPixmap(-1, cRect(keyboardX, keyboardY, keyboardWidth*width/100, keyboardHeight));
        pixmapKeyboardIcons = osdManager.requestPixmap(-1, cRect(keyboardX, keyboardY, keyboardWidth*width/100, keyboardHeight));
    } else {
        pixmap->SetViewPort(cRect(x, y, width, height));
        pixmapVal->SetViewPort(cRect(xPixmapVal, yPixmapVal, widthPixmapVal, heightPixmapVal));
        pixmapKeyboard->SetViewPort(cRect(keyboardX, keyboardY, keyboardWidth*width/100, keyboardHeight));
        pixmapKeyboardHighlight->SetViewPort(cRect(keyboardX, keyboardY, keyboardWidth*width/100, keyboardHeight));
        pixmapKeyboardIcons->SetViewPort(cRect(keyboardX, keyboardY, keyboardWidth*width/100, keyboardHeight));
    }
    pixmapKeyboardIcons->Fill(clrTransparent);
}

void cRecMenuItemText::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapVal->SetLayer(-1);
    pixmapKeyboard->SetLayer(-1);
    pixmapKeyboardHighlight->SetLayer(-1);
    pixmapKeyboardIcons->SetLayer(-1);
}

void cRecMenuItemText::Show(void) { 
    pixmap->SetLayer(4);
    pixmapVal->SetLayer(5);
}

void cRecMenuItemText::setBackground() {
    if (!active) {
        DeactivateKeyboard();
    }
    cRecMenuItem::setBackground();
}

void cRecMenuItemText::Draw(void) {
    int textY = (height/2 - font->Height()) / 2;
    pixmap->DrawText(cPoint(10, textY), *title, colorText, colorTextBack, font);
    DrawValue(value);    
}

void cRecMenuItemText::DrawValue(char *newValue) {
    tColor clrBack = InEditMode()?theme.Color(clrRecMenuTextActiveBack):theme.Color(clrRecMenuTextBack);
    pixmapVal->Fill(clrBack);
    int textX = pixmapVal->DrawPort().Width() - font->Width(newValue) - 10;
    int textY = (pixmapVal->DrawPort().Height() - font->Height()) / 2;
    pixmapVal->DrawText(cPoint(textX, textY), newValue, colorText, clrTransparent, font);
}

void cRecMenuItemText::ActivateKeyboard(void) {
    pixmapKeyboard->SetLayer(6);
    pixmapKeyboardHighlight->SetLayer(6);
    pixmapKeyboardIcons->SetLayer(6);
    pixmapKeyboardHighlight->Fill(clrTransparent);
    
    if (keyboardDrawn)
        return;
    
    pixmapKeyboard->setColor(theme.Color(clrRecMenuKeyboardBack), theme.Color(clrRecMenuKeyboardBack));
    pixmapKeyboard->drawBackground();
       
    int widthKeyBoard = pixmapKeyboard->Width();
    gridWidth = widthKeyBoard / 3;
        
    pixmapKeyboard->DrawRectangle(cRect(0, 0, widthKeyBoard, keyboardHeight), theme.Color(clrRecMenuKeyboardBorder));
    int num = 1;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 3; col++) {
            int X = col*gridWidth;
            int Y = row*gridHeight;
            tColor clrBack = theme.Color(clrRecMenuKeyboardBack);
            if (num==13)
                clrBack = theme.Color(clrButtonRedKeyboard);
            else if (num==14)
                clrBack = theme.Color(clrButtonGreenKeyboard);
            else if (num==15)
                clrBack = theme.Color(clrButtonYellowKeyboard);
            pixmapKeyboard->DrawRectangle(cRect(X+2, Y+2, gridWidth-4, gridHeight-4), clrBack);
            pixmapKeyboard->DrawEllipse(cRect(X, Y, 20, 20), theme.Color(clrRecMenuKeyboardBorder),-2);
            pixmapKeyboard->DrawEllipse(cRect(X, Y+gridHeight-20, 20, 20), theme.Color(clrRecMenuKeyboardBorder),-3);
            pixmapKeyboard->DrawEllipse(cRect(X+gridWidth-20, Y+gridHeight-20, 20, 20), theme.Color(clrRecMenuKeyboardBorder),-4);
            pixmapKeyboard->DrawEllipse(cRect(X+gridWidth-20, Y, 20, 20), theme.Color(clrRecMenuKeyboardBorder),-1);
            bool draw = false;
            bool drawIcon = false;
            cString strNum;
            std::string strIcon;
            if (num<10) {
                strNum = *cString::sprintf("%d", num);
                draw = true;
            } else if (num == 11) {
                strNum = *cString::sprintf("%d", 0);
                draw = true;
            } else if (num==13) {
                strIcon = "icon_shift";
                drawIcon = true;
            } else if (num==14) {
                strIcon = "icon_del_ins";
                drawIcon = true;
            } else if (num==15) {
                strIcon = "icon_backspace";
                drawIcon = true;
            }
            if (draw) {
                int numX = X + (gridWidth - font->Width(*strNum))/2;
                int numY = Y + font->Height() / 4;
                pixmapKeyboard->DrawText(cPoint(numX, numY), *strNum, colorText, colorTextBack, font);
                char *smsKeys = GetSMSKeys(num);
                int smsKeysX = X + (gridWidth - fontSmall->Width(smsKeys))/2;
                int smsKeysY = Y + gridHeight - fontSmall->Height() - 10;
                pixmapKeyboard->DrawText(cPoint(smsKeysX, smsKeysY), smsKeys, theme.Color(clrRecMenuKeyboardBorder), colorTextBack, fontSmall);
                delete[] smsKeys;
            }
            if (drawIcon) {
                int iconSize = gridHeight - 10;
                cImage *imgIcon = imgCache.GetIcon(strIcon, iconSize, iconSize);
                if (imgIcon) {
                    int iconX = X + (gridWidth - iconSize) / 2;
                    pixmapKeyboardIcons->DrawImage(cPoint(iconX, Y + 5), *imgIcon);
                }
            }
            num++;
        }
    }
    keyboardDrawn = true;
}

void cRecMenuItemText::DeactivateKeyboard(void) {
    pixmapKeyboard->SetLayer(-1);
    pixmapKeyboardHighlight->SetLayer(-1);
    pixmapKeyboardIcons->SetLayer(-1);
}

void cRecMenuItemText::HighlightSMSKey(int num) {
    int x = 0;
    int y = 0;
    if (num == 0) {
        x = gridWidth;
        y = 3 * gridHeight;
    } else {
        x = (num-1)%3 * gridWidth;
        y = (num-1)/3 * gridHeight;
    }
    pixmapKeyboardHighlight->DrawRectangle(cRect(x, y, gridWidth, gridHeight), theme.Color(clrRecMenuKeyboardHigh));
}

void cRecMenuItemText::ClearSMSKey(void) {
    pixmapKeyboardHighlight->Fill(clrTransparent);    
}


char *cRecMenuItemText::GetSMSKeys(int num) {
    if (num == 11)
        num = 0;
    if (num > 9)
        return NULL;
    
    currentCharUtf8 = charMapUtf8;
    int pos = num;
    while (pos > 0 && *currentCharUtf8) {
        if (*currentCharUtf8++ == '\t')
            pos--;
    }
    while (*currentCharUtf8 && *currentCharUtf8 != '\t' && !IsAllowed(*currentCharUtf8)) {
        currentCharUtf8++;
    }
    uint *startCharUtf8 = currentCharUtf8;
    int numChars = 0;
    bool specialChar = false;
    while(*currentCharUtf8 && *currentCharUtf8 != '\t' && IsAllowed(*currentCharUtf8)) {
        currentCharUtf8++;
        if (Utf8CharSet(*currentCharUtf8) > 1)
            specialChar = true;
        numChars++;
    }
    char buf[100];
    char *p = buf;
    int addition = 0;
    if (specialChar)
        addition = 1;
    Utf8FromArray(startCharUtf8, p, numChars+addition);
    int maxChars = min(numChars+1+addition, 8);
    char *smskey = new char[maxChars];
    Utf8Strn0Cpy(smskey, p, maxChars);
    return smskey;
}

void cRecMenuItemText::EnterEditMode(void) {
    if (!valueUtf8) {
        valueUtf8 = new uint[length];
        lengthUtf8 = Utf8ToArray(value, valueUtf8, length);
        int l = strlen(allowed) + 1;
        allowedUtf8 = new uint[l];
        Utf8ToArray(allowed, allowedUtf8, l);
        const char *charMap = trVDR("CharMap$ 0\t-.,1#~\\^$[]|()*+?{}/:%@&\tabc2\tdef3\tghi4\tjkl5\tmno6\tpqrs7\ttuv8\twxyz9");
        l = strlen(charMap) + 1;
        charMapUtf8 = new uint[l];
        Utf8ToArray(charMap, charMapUtf8, l);
        currentCharUtf8 = charMapUtf8;
        AdvancePos();
    }
}

void cRecMenuItemText::LeaveEditMode(bool SaveValue) {
    if (valueUtf8) {
        if (SaveValue) {
            Utf8FromArray(valueUtf8, value, length);
            stripspace(value);
        }
        lengthUtf8 = 0;
        delete[] valueUtf8;
        valueUtf8 = NULL;
        delete[] allowedUtf8;
        allowedUtf8 = NULL;
        delete[] charMapUtf8;
        charMapUtf8 = NULL;
        pos = -1;
        offset = 0;
        newchar = true;
    }
}

void cRecMenuItemText::AdvancePos(void) {
    if (pos < length - 2 && pos < lengthUtf8) {
        if (++pos >= lengthUtf8) {
            if (pos >= 2 && valueUtf8[pos - 1] == ' ' && valueUtf8[pos - 2] == ' ')
                pos--; // allow only two blanks at the end
            else {
                valueUtf8[pos] = ' ';
                valueUtf8[pos + 1] = 0;
                lengthUtf8++;
            }
        }
    }
    newchar = true;
    if (!insert && Utf8is(alpha, valueUtf8[pos]))
        uppercase = Utf8is(upper, valueUtf8[pos]);
}

uint cRecMenuItemText::Inc(uint c, bool Up) {
    uint *p = IsAllowed(c);
    if (!p)
        p = allowedUtf8;
    if (Up) {
        if (!*++p)
            p = allowedUtf8;
    } else if (--p < allowedUtf8) {
        p = allowedUtf8;
        while (*p && *(p + 1))
            p++;
    }
    return *p;
}

void cRecMenuItemText::Type(uint c) {
    if (insert && lengthUtf8 < length - 1)
        Insert();
    valueUtf8[pos] = c;
    if (pos < length - 2)
        pos++;
    if (pos >= lengthUtf8) {
        valueUtf8[pos] = ' ';
        valueUtf8[pos + 1] = 0;
        lengthUtf8 = pos + 1;
    }
}

void cRecMenuItemText::Insert(void) {
    memmove(valueUtf8 + pos + 1, valueUtf8 + pos, (lengthUtf8 - pos + 1) * sizeof(*valueUtf8));
    lengthUtf8++;
    valueUtf8[pos] = ' ';
}

void cRecMenuItemText::Delete(void) {
    memmove(valueUtf8 + pos, valueUtf8 + pos + 1, (lengthUtf8 - pos) * sizeof(*valueUtf8));
    lengthUtf8--;
}

uint *cRecMenuItemText::IsAllowed(uint c) {
    if (allowedUtf8) {
        for (uint *a = allowedUtf8; *a; a++) {
            if (c == *a)
                return a;
        }
    }
    return NULL;
}

void cRecMenuItemText::SetText(void) {
    if (InEditMode()) {
        int textAreaWidth = width - 20;
        textAreaWidth -= font->Width("[]");
        textAreaWidth -= font->Width("<>"); // reserving this anyway make the whole thing simpler
        if (pos < offset)
            offset = pos;
        int WidthFromOffset = 0;
        int EndPos = lengthUtf8;
        for (int i = offset; i < lengthUtf8; i++) {
            WidthFromOffset += font->Width(valueUtf8[i]);
            if (WidthFromOffset > textAreaWidth) {
                if (pos >= i) {
                    do {
                        WidthFromOffset -= font->Width(valueUtf8[offset]);
                        offset++;
                    } while (WidthFromOffset > textAreaWidth && offset < pos);
                    EndPos = pos + 1;
                } else {
                    EndPos = i;
                    break;
                }
            }
        }
        char buf[1000];
        char *p = buf;
        if (offset)
            *p++ = '<';
        p += Utf8FromArray(valueUtf8 + offset, p, sizeof(buf) - (p - buf), pos - offset);
        *p++ = '[';
        if (insert && newchar)
            *p++ = ']';
        p += Utf8FromArray(&valueUtf8[pos], p, sizeof(buf) - (p - buf), 1);
        if (!(insert && newchar))
            *p++ = ']';
        p += Utf8FromArray(&valueUtf8[pos + 1], p, sizeof(buf) - (p - buf), EndPos - pos - 1);
        if (EndPos != lengthUtf8)
            *p++ = '>';
        *p = 0;
        DrawValue(buf);
    } else {
        DrawValue(value);
    }
}

eRecMenuState cRecMenuItemText::ProcessKey(eKeys Key) {
    bool consumed = false;
    bool SameKey = NORMALKEY(Key) == lastKey;
    ClearSMSKey();
    if (Key != kNone) {
        lastKey = NORMALKEY(Key);
    } else if (!newchar && k0 <= lastKey && lastKey <= k9 && autoAdvanceTimeout.TimedOut()) {
        AdvancePos();
        newchar = true;
        currentCharUtf8 = NULL;
        SetText();
        return rmsConsumed;
    }

    switch ((int)Key) {
        case kRed:   // Switch between upper- and lowercase characters
            if (InEditMode()) {
                if (!insert || !newchar) {
                    uppercase = !uppercase;
                    valueUtf8[pos] = uppercase ? Utf8to(upper, valueUtf8[pos]) : Utf8to(lower, valueUtf8[pos]);
                }
                consumed = true;
            }
            break;
        case kGreen: // Toggle insert/overwrite modes
            if (InEditMode()) {
                insert = !insert;
                newchar = true;
                consumed = true;
            }
            break;
        case kYellow|k_Repeat:
        case kYellow: // Remove the character at the current position; in insert mode it is the character to the right of the cursor
            if (InEditMode()) {
                if (lengthUtf8 > 1) {
                    if (!insert || pos < lengthUtf8 - 1)
                        Delete();
                    else if (insert && pos == lengthUtf8 - 1)
                        valueUtf8[pos] = ' '; // in insert mode, deleting the last character replaces it with a blank to keep the cursor position
                    // reduce position, if we removed the last character
                    if (pos == lengthUtf8)
                        pos--;
                } else if (lengthUtf8 == 1)
                    valueUtf8[0] = ' '; // This is the last character in the string, replace it with a blank
                if (Utf8is(alpha, valueUtf8[pos]))
                    uppercase = Utf8is(upper, valueUtf8[pos]);
                newchar = true;
                consumed = true;
            }
            break;
        case kLeft|k_Repeat:
        case kLeft:
            
            if (pos > 0) {
                if (!insert || newchar)
                    pos--;
                newchar = true;
                if (!insert && Utf8is(alpha, valueUtf8[pos]))
                    uppercase = Utf8is(upper, valueUtf8[pos]);
            }
            consumed = true;
            break;
        case kRight|k_Repeat:
        case kRight: 
            if (InEditMode()) {
                AdvancePos();
            } else {
                EnterEditMode();
                ActivateKeyboard();
            }
            consumed = true;
            break;
        case kUp|k_Repeat:
        case kUp:
        case kDown|k_Repeat:
        case kDown:  
            if (InEditMode()) {
                if (insert && newchar) {
                // create a new character in insert mode
                    if (lengthUtf8 < length - 1)
                        Insert();
                    }
                    if (uppercase)
                        valueUtf8[pos] = Utf8to(upper, Inc(Utf8to(lower, valueUtf8[pos]), NORMALKEY(Key) == kUp));
                    else
                        valueUtf8[pos] =               Inc(              valueUtf8[pos],  NORMALKEY(Key) == kUp);
                        newchar = false;
                consumed = true;
            }
            break;
        case k0|k_Repeat ... k9|k_Repeat:
        case k0 ... k9: {
            if (InEditMode()) {
                if (Setup.NumberKeysForChars) {
                    HighlightSMSKey(NORMALKEY(Key) - k0);
                    if (!SameKey) {
                        if (!newchar)
                            AdvancePos();
                        currentCharUtf8 = NULL;
                    }
                    if (!currentCharUtf8 || !*currentCharUtf8 || *currentCharUtf8 == '\t') {
                    // find the beginning of the character map entry for Key
                        int n = NORMALKEY(Key) - k0;
                        currentCharUtf8 = charMapUtf8;
                        while (n > 0 && *currentCharUtf8) {
                            if (*currentCharUtf8++ == '\t')
                                n--;
                        }
                        // find first allowed character
                        while (*currentCharUtf8 && *currentCharUtf8 != '\t' && !IsAllowed(*currentCharUtf8))
                            currentCharUtf8++;
                    }
                    if (*currentCharUtf8 && *currentCharUtf8 != '\t') {
                        if (insert && newchar) {
                        // create a new character in insert mode
                            if (lengthUtf8 < length - 1)
                                Insert();
                        }
                        valueUtf8[pos] = *currentCharUtf8;
                        if (uppercase)
                            valueUtf8[pos] = Utf8to(upper, valueUtf8[pos]);
                        // find next allowed character
                        do {
                            currentCharUtf8++;
                        } while (*currentCharUtf8 && *currentCharUtf8 != '\t' && !IsAllowed(*currentCharUtf8));
                        newchar = false;
                        autoAdvanceTimeout.Set(AUTO_ADVANCE_TIMEOUT);
                    }
                } else {
                    Type('0' + NORMALKEY(Key) - k0);
                }
                consumed = true;
            }                
            break; }
        case kBack:
        case kOk:    
            if (InEditMode()) {
                LeaveEditMode(Key == kOk);
                DeactivateKeyboard();
            } else {
	    	EnterEditMode();
		ActivateKeyboard();
	    }
	    consumed = true;
	    break;
        default:
            if (InEditMode() && BASICKEY(Key) == kKbd) {
                int c = KEYKBD(Key);
                if (c <= 0xFF) { 
                    if (IsAllowed(Utf8to(lower, c)))
                        Type(c);
                    else {
                        switch (c) {
                            case 0x7F: // backspace
                                if (pos > 0) {
                                    pos--;
                                    ProcessKey(kYellow);
                                }
                                break;
                            default: ;
                        }
                    }
                } else {
                    switch (c) {
                        case kfHome: pos = 0; break;
                        case kfEnd:  pos = lengthUtf8 - 1; break;
                        case kfIns:  ProcessKey(kGreen);
                        case kfDel:  ProcessKey(kYellow);
                        default: ;
                    }
                }
                consumed = true;
            }
            break;
    }
    SetText();
    if (consumed)
        return rmsConsumed;
    return rmsNotConsumed;
}

// --- cRecMenuItemTime  -------------------------------------------------------
cRecMenuItemTime::cRecMenuItemTime(cString text,
                                   int initialVal,
                                   bool active) {
    selectable = true;
    this->text = text;
    this->value = initialVal;
    hh = value / 100;
    mm = value % 100;
    pos = 0;
    fresh = true;
    this->active = active;
    height = 3 * font->Height() / 2;
    pixmapVal = NULL;
}

cRecMenuItemTime::~cRecMenuItemTime(void) {
    if (pixmapVal)
        osdManager.releasePixmap(pixmapVal);
}

void cRecMenuItemTime::SetPixmaps(void) {
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
        pixmapVal = osdManager.requestPixmap(5, cRect(x, y, width, height));
    } else {
        pixmap->SetViewPort(cRect(x, y, width, height));
        pixmapVal->SetViewPort(cRect(x, y, width, height));
    }
}

void cRecMenuItemTime::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapVal->SetLayer(-1);
}

void cRecMenuItemTime::Show(void) { 
    pixmap->SetLayer(4);
    pixmapVal->SetLayer(5);
}

void cRecMenuItemTime::Draw(void) {
    int textY = (height - font->Height()) / 2;
    pixmap->DrawText(cPoint(10, textY), *text, colorText, colorTextBack, font);
    DrawValue();    
}

void cRecMenuItemTime::DrawValue(void) {
    pixmapVal->Fill(clrTransparent);
    char buf[10];
    switch (pos) {
        case 1:  snprintf(buf, sizeof(buf), "%01d-:--", hh / 10); break;
        case 2:  snprintf(buf, sizeof(buf), "%02d:--", hh); break;
        case 3:  snprintf(buf, sizeof(buf), "%02d:%01d-", hh, mm / 10); break;
        default: snprintf(buf, sizeof(buf), "%02d:%02d", hh, mm);
    }
    int textX = width - font->Width(buf) - 10;
    int textY = (height - font->Height()) / 2;
    pixmapVal->DrawText(cPoint(textX, textY), buf, colorText, clrTransparent, font);
}

eRecMenuState cRecMenuItemTime::ProcessKey(eKeys Key) {
    switch (Key & ~k_Repeat) {
        case kLeft|k_Repeat:
        case kLeft: {
            if (--mm < 0) {
                mm = 59;
                if (--hh < 0)
                    hh = 23;
            }
            fresh = true;
            value = hh * 100 + mm;
            DrawValue();
            return rmsConsumed;
            break; }
        case kRight|k_Repeat:
        case kRight: {
            if (++mm > 59) {
                mm = 0;
                if (++hh > 23)
                    hh = 0;
            }
            fresh = true;
            value = hh * 100 + mm;
            DrawValue();
            return rmsConsumed;
            break; }
        case k0|k_Repeat ... k9|k_Repeat:
        case k0 ... k9: {
            if (fresh || pos > 3) {
                pos = 0;
                fresh = false;
            }
            int n = Key - k0;
            switch (pos) {
                case 0: 
                    if (n <= 2) {
                        hh = n * 10;
                        mm = 0;
                        pos++;
                    }
                    break;
                case 1: 
                    if (hh + n <= 23) {
                        hh += n;
                        pos++;
                    }
                  break;
                case 2: 
                    if (n <= 5) {
                        mm += n * 10;
                        pos++;
                    }
                    break;
                case 3: 
                    if (mm + n <= 59) {
                        mm += n;
                        pos++;
                    }
                    break;
                default: ;
            }
            value = hh * 100 + mm;
            DrawValue();
            return rmsConsumed;
            break; }
        default:
            break;
    }
    return rmsNotConsumed;
}

// --- cRecMenuItemDay  -------------------------------------------------------
cRecMenuItemDay::cRecMenuItemDay(cString text,
                                 time_t initialVal,
                                 bool active) {
    selectable = true;
    this->text = text;
    this->currentVal = cTimer::SetTime(initialVal, 0);
    this->active = active;
    height = 3 * font->Height() / 2;
    pixmapVal = NULL;
}

cRecMenuItemDay::~cRecMenuItemDay(void) {
    if (pixmapVal)
        osdManager.releasePixmap(pixmapVal);
}

void cRecMenuItemDay::SetPixmaps(void) {
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
        pixmapVal = osdManager.requestPixmap(5, cRect(x, y, width, height));
    } else {
        pixmap->SetViewPort(cRect(x, y, width, height));
        pixmapVal->SetViewPort(cRect(x, y, width, height));
    }
}

void cRecMenuItemDay::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapVal->SetLayer(-1);
}

void cRecMenuItemDay::Show(void) { 
    pixmap->SetLayer(4);
    pixmapVal->SetLayer(5);
}

void cRecMenuItemDay::Draw(void) {
    int textY = (height - font->Height()) / 2;
    pixmap->DrawText(cPoint(10, textY), *text, colorText, colorTextBack, font);
    DrawValue();    
}

void cRecMenuItemDay::DrawValue(void) {
    pixmapVal->Fill(clrTransparent);
    cString textVal = DateString(currentVal);
    int textX = width - font->Width(*textVal) - 10;
    int textY = (height - font->Height()) / 2;
    pixmapVal->DrawText(cPoint(textX, textY), *textVal, colorText, clrTransparent, font);
}

eRecMenuState cRecMenuItemDay::ProcessKey(eKeys Key) {
    switch (Key & ~k_Repeat) {
        case kLeft:
            currentVal -= 60*60*24;
            DrawValue();
            return rmsConsumed;
            break;
        case kRight:
            currentVal += 60*60*24;
            DrawValue();
            return rmsConsumed;
            break;
        default:
            break;
    }
    return rmsNotConsumed;
}

// --- cRecMenuItemTimer  -------------------------------------------------------
cRecMenuItemTimer::cRecMenuItemTimer(const cTimer *timer, 
                                     eRecMenuState action1, 
                                     eRecMenuState action2,
                                     eRecMenuState action3,
                                     eRecMenuState action4,
                                     time_t conflictStart,
                                     time_t conflictStop,
                                     time_t overlapStart,
                                     time_t overlapStop,
                                     bool active) {
    selectable = true;
    this->timer = timer;
    this->action = action1;
    this->action2 = action2;
    this->action3 = action3;
    this->action4 = action4;
    iconActive = 0;
    this->conflictStart = conflictStart;
    this->conflictStop = conflictStop;
    this->overlapStart = overlapStart;
    this->overlapStop = overlapStop;
    this->active = active;
    height = 3 * font->Height();
    pixmapIcons = NULL;
}

cRecMenuItemTimer::~cRecMenuItemTimer(void) {
    if (pixmapIcons)
        osdManager.releasePixmap(pixmapIcons);
    if (pixmapStatus)
        osdManager.releasePixmap(pixmapStatus);
}

void cRecMenuItemTimer::SetPixmaps(void) {
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
        pixmapStatus = osdManager.requestPixmap(5, cRect(x, y, width, height));
        pixmapIcons = osdManager.requestPixmap(6, cRect(x, y, width, height));
    } else {
        pixmap->SetViewPort(cRect(x, y, width, height));
        pixmapStatus->SetViewPort(cRect(x, y, width, height));
        pixmapIcons->SetViewPort(cRect(x, y, width, height));
    }
    pixmapStatus->Fill(clrTransparent);
    pixmapIcons->Fill(clrTransparent);
}

void cRecMenuItemTimer::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapStatus->SetLayer(-1);
    pixmapIcons->SetLayer(-1);
}

void cRecMenuItemTimer::Show(void) { 
    pixmap->SetLayer(4);
    pixmapStatus->SetLayer(5);
    pixmapIcons->SetLayer(6);
}

void cRecMenuItemTimer::Draw(void) {
    const cChannel *channel = timer->Channel();
    int channelTransponder = 0;
    cString channelName = "";
    if (channel) {
        channelTransponder = channel->Transponder();
        channelName = channel->Name();
    }
    int logoX = DrawIcons();
    int logoWidth = height * tvguideConfig.logoWidthRatio / tvguideConfig.logoHeightRatio;
    cImageLoader imgLoader;
    if (!tvguideConfig.hideChannelLogos) {
        if (imgLoader.LoadLogo(channel, logoWidth, height)) {
            cImage logo = imgLoader.GetImage();
            pixmapIcons->DrawImage(cPoint(logoX, 0), logo);
            logoX += logoWidth + 5;
        }
    }
    int textX = logoX;
    int textHeightLine1 = (height/2 - font->Height()) / 2;
    int textHeightLine2 = height/2 - 5 + (height/4 - fontSmall->Height()) / 2;
    int textHeightLine3 = 3*height/4 - 5 + (height/4 - fontSmall->Height()) / 2;
    const cEvent *event = timer->Event();
    std::string timerTitle = "";
    if (event) {
        timerTitle = event->Title();
        timerTitle = CutText(timerTitle, (70 * width / 100) - textX, font);
    }
    cString timeStart = DayDateTime(timer->StartTime());
    cString timeEnd = TimeString(timer->StopTime());
    cString timerTime = cString::sprintf("%s - %s", *timeStart, *timeEnd);
    cString channelInfo = cString::sprintf("%s, %s %d", *channelName, tr("Transp."), channelTransponder); 
    pixmapIcons->DrawText(cPoint(textX, textHeightLine1), timerTitle.c_str(), colorText, colorTextBack, font);
    pixmapIcons->DrawText(cPoint(textX, textHeightLine2), *timerTime, colorText, colorTextBack, fontSmall);
    pixmapIcons->DrawText(cPoint(textX, textHeightLine3), *channelInfo, colorText, colorTextBack, fontSmall);

    DrawTimerConflict();
}

int cRecMenuItemTimer::DrawIcons(void) {
    int iconsX = 10;
    int iconSize = height/2;
    int iconY = (height - iconSize) / 2;
    std::string iconInfo, iconDelete, iconEdit, iconSearch;
    if (active) {
        iconInfo = (iconActive==0)?"info_active":"info_inactive";
        iconDelete = (iconActive==1)?"delete_active":"delete_inactive";
        iconEdit = (iconActive==2)?"edit_active":"edit_inactive";
        iconSearch = (iconActive==3)?"search_active":"search_inactive";
    } else {
        iconInfo = "info_inactive";
        iconDelete = "delete_inactive";
        iconEdit = "edit_inactive";
        iconSearch = "search_inactive";
    }
    
    cImage *imgInfo = imgCache.GetIcon(iconInfo, iconSize, iconSize);
    if (imgInfo) {
        pixmapIcons->DrawImage(cPoint(iconsX, iconY), *imgInfo);
        iconsX += iconSize + 5;
    }
    cImage *imgDelete = imgCache.GetIcon(iconDelete, iconSize, iconSize);
    if (imgDelete) {
        pixmapIcons->DrawImage(cPoint(iconsX, iconY), *imgDelete);
        iconsX += iconSize + 5;
    }
    cImage *imgEdit = imgCache.GetIcon(iconEdit, iconSize, iconSize);
    if (imgEdit) {
        pixmapIcons->DrawImage(cPoint(iconsX, iconY), *imgEdit);
        iconsX += iconSize + 5;
    }
    cImage *imgSearch = imgCache.GetIcon(iconSearch, iconSize, iconSize);
    if (imgSearch) {
        pixmapIcons->DrawImage(cPoint(iconsX, iconY), *imgSearch);
        iconsX += iconSize + 5;
    }
    return iconsX;
}

void cRecMenuItemTimer::DrawTimerConflict(void) {
    int widthConfl = 30 * width / 100;
    int xConfl = width - widthConfl;
    int heightConflBar = height / 4;
    int yConflBar = (height - heightConflBar) / 2;
    pixmapStatus->DrawRectangle(cRect(xConfl, 0, widthConfl, height), theme.Color(clrRecMenuTimerConflictBackground));
    
    int completeWidthSecs = conflictStop - conflictStart;
    int xConfBar = xConfl + (timer->StartTime() - conflictStart) * widthConfl / completeWidthSecs;
    int widthConfBar = (timer->StopTime() - timer->StartTime()) * widthConfl / completeWidthSecs;
    pixmapStatus->DrawRectangle(cRect(xConfBar, yConflBar, widthConfBar, heightConflBar), theme.Color(clrRecMenuTimerConflictBar));

    int xOverlap = xConfl + (overlapStart - conflictStart) * widthConfl / completeWidthSecs;
    int widthOverlap = (overlapStop - overlapStart) * widthConfl / completeWidthSecs;
    
    pixmapIcons->DrawRectangle(cRect(xOverlap, 0, widthOverlap, height), theme.Color(clrRecMenuTimerConflictOverlap));

    pixmapStatus->DrawRectangle(cRect(xConfl-3,2,1,height-4), theme.Color(clrBorder));
    pixmapStatus->DrawRectangle(cRect(xConfl-2,0,2,height), theme.Color(clrBackground));
}

eRecMenuState cRecMenuItemTimer::ProcessKey(eKeys Key) {
    switch (Key & ~k_Repeat) {
        case kLeft:
            if (iconActive > 0) {
                iconActive--;
                DrawIcons();
                return rmsConsumed;
            } else 
                return rmsNotConsumed;
            break;
        case kRight:
            if (iconActive < 3) {
                iconActive++;
                DrawIcons();
                return rmsConsumed;
            } else 
                return rmsNotConsumed;
            break;
        case kOk:
            if (iconActive == 0)
                return action;
            else if (iconActive == 1)
                return action2;
            else if (iconActive == 2)
                return action3;
            else if (iconActive == 3)
                return action4;
            break;
        default:
            break;
    }
    return rmsNotConsumed;
}

// --- cRecMenuItemTimerConflictHeader -------------------------------------------------------

cRecMenuItemTimerConflictHeader::cRecMenuItemTimerConflictHeader(time_t conflictStart,
                                                                 time_t conflictStop,
                                                                 time_t overlapStart,
                                                                 time_t overlapStop) {
    selectable = false;
    active = false;
    this->conflictStart = conflictStart;
    this->conflictStop = conflictStop;
    this->overlapStart = overlapStart;
    this->overlapStop = overlapStop;
    height = 3*font->Height()/2;
    pixmapStatus = NULL;
}

cRecMenuItemTimerConflictHeader::~cRecMenuItemTimerConflictHeader(void) {
    if (pixmapStatus)
        osdManager.releasePixmap(pixmapStatus);
}

void cRecMenuItemTimerConflictHeader::SetPixmaps(void) {
    pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
    pixmapStatus = osdManager.requestPixmap(5, cRect(x, y, width, height));
    pixmapStatus->Fill(clrTransparent);
}

void cRecMenuItemTimerConflictHeader::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapStatus->SetLayer(-1);
}

void cRecMenuItemTimerConflictHeader::Show(void) { 
    pixmap->SetLayer(4);
    pixmapStatus->SetLayer(5);
}

void cRecMenuItemTimerConflictHeader::setBackground(void) {
    pixmap->Fill(clrTransparent);
    cRecMenuItem::setBackground();
}

void cRecMenuItemTimerConflictHeader::Draw(void) {
    int widthConfl = 30 * width / 100;
    int xConfl = width - widthConfl;
    cString headerText = tr("Timer Conflict");
    int xHeader = (xConfl - font->Width(*headerText)) / 2;
    int yHeader = (height - font->Height()) / 2;
    pixmap->DrawText(cPoint(xHeader, yHeader), *headerText, theme.Color(clrFont), colorTextBack, font);
    
    pixmap->DrawRectangle(cRect(xConfl, 0, widthConfl, height), theme.Color(clrRecMenuTimerConflictBackground));
    
    int completeWidthSecs = conflictStop - conflictStart;
    int xOverlap = xConfl + (overlapStart - conflictStart) * widthConfl / completeWidthSecs;
    int yOverlap = height - fontSmall->Height();
    int widthOverlap = (overlapStop - overlapStart) * widthConfl / completeWidthSecs;
    
    pixmapStatus->DrawRectangle(cRect(xOverlap, yOverlap, widthOverlap, height), theme.Color(clrRecMenuTimerConflictOverlap));

    cString strConflStart = TimeString(conflictStart);
    cString strConflStop = TimeString(conflictStop);
    cString strOverlapStart = TimeString(overlapStart);
    cString strOverlapStop = TimeString(overlapStop);
    int y1 = 5;
    int y2 = yOverlap;
    int xConflStart = xConfl + 2;
    int xConflStop = width - fontSmall->Width(*strConflStop) - 2;
    int xOverlapStart = xOverlap - fontSmall->Width(*strOverlapStart) - 2;
    int xOverlapStop = xOverlap + widthOverlap + 2;
    pixmap->DrawText(cPoint(xConflStart, y1), *strConflStart, theme.Color(clrRecMenuTimerConflictBar), theme.Color(clrRecMenuTimerConflictBackground), fontSmall);
    pixmap->DrawText(cPoint(xConflStop, y1), *strConflStop, theme.Color(clrRecMenuTimerConflictBar), theme.Color(clrRecMenuTimerConflictBackground), fontSmall);
    pixmap->DrawText(cPoint(xOverlapStart, y2), *strOverlapStart, theme.Color(clrRecMenuTimerConflictOverlap), theme.Color(clrRecMenuTimerConflictBackground), fontSmall);
    pixmap->DrawText(cPoint(xOverlapStop, y2), *strOverlapStop, theme.Color(clrRecMenuTimerConflictOverlap), theme.Color(clrRecMenuTimerConflictBackground), fontSmall);
}

// --- cRecMenuItemEvent  -------------------------------------------------------
cRecMenuItemEvent::cRecMenuItemEvent(const cEvent *event, 
                                     eRecMenuState action1, 
                                     eRecMenuState action2, 
                                     bool active) {
    selectable = true;
    this->event = event;
    this->action = action1;
    this->action2 = action2;
    iconActive = 0;
    this->active = active;
    height = font->Height() + 2*fontSmall->Height() + 10;
    pixmapText = NULL;
    pixmapIcons = NULL;
}

cRecMenuItemEvent::~cRecMenuItemEvent(void) {
    if (pixmapIcons)
        osdManager.releasePixmap(pixmapIcons);
    if (pixmapText)
        osdManager.releasePixmap(pixmapText);
}

void cRecMenuItemEvent::SetPixmaps(void) {
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
        pixmapText = osdManager.requestPixmap(5, cRect(x, y, width, height));
        pixmapText->Fill(clrTransparent);
        pixmapIcons = osdManager.requestPixmap(6, cRect(x, y, width, height));
        pixmapIcons->Fill(clrTransparent);
    } else {
        pixmap->SetViewPort(cRect(x, y, width, height));
        pixmapText->SetViewPort(cRect(x, y, width, height));
        pixmapIcons->SetViewPort(cRect(x, y, width, height));
    }
}

void cRecMenuItemEvent::Draw(void) {
    if (!event)
        return;
    int logoX = DrawIcons();
    const cChannel *channel = Channels.GetByChannelID(event->ChannelID());
    cString channelName = "";
    if (channel)
        channelName = channel->Name();
    int logoWidth = height * tvguideConfig.logoWidthRatio / tvguideConfig.logoHeightRatio;
    cImageLoader imgLoader;
    if (!tvguideConfig.hideChannelLogos) {
        if (imgLoader.LoadLogo(channel, logoWidth, height)) {
            cImage logo = imgLoader.GetImage();
            pixmapText->DrawImage(cPoint(logoX, 0), logo);
            logoX += logoWidth + 5;
        }
    }
    
    int textX = logoX;
    int textHeightLine1 = 5;
    int textHeightLine2 = 5 + fontSmall->Height();
    int textHeightLine3 = height - fontSmall->Height() - 5;

    cString title = event->Title();
    cString desc = event->ShortText();
    cString start = DayDateTime(event->StartTime());
    cString end = event->GetEndTimeString();

    colorText = active?theme.Color(clrFontActive):theme.Color(clrFont);
    cString info = cString::sprintf("%s - %s, %s", *start, *end, *channelName);
    pixmapText->DrawText(cPoint(textX, textHeightLine1), *info, colorText, clrTransparent, fontSmall);
    pixmapText->DrawText(cPoint(textX, textHeightLine2), *title, colorText, clrTransparent, font);
    pixmapText->DrawText(cPoint(textX, textHeightLine3), *desc, colorText, clrTransparent, fontSmall);
}

int cRecMenuItemEvent::DrawIcons(void) {
    pixmapIcons->Fill(clrTransparent);
    int iconsX = 10;
    int iconSize = height / 2;
    int iconY = (height - iconSize) / 2;
    std::string iconInfo, iconRecord;
    if (active) {
        iconInfo = (iconActive==0)?"info_active":"info_inactive";
        if (action2 != rmsDisabled)
            iconRecord = (iconActive==1)?"record_active":"record_inactive";
    } else {
        iconInfo = "info_inactive";
        if (action2 != rmsDisabled)
            iconRecord = "record_inactive";
    }
    cImage *imgInfo = imgCache.GetIcon(iconInfo, iconSize, iconSize);
    if (imgInfo) {
        pixmapIcons->DrawImage(cPoint(iconsX, iconY), *imgInfo);
        iconsX += iconSize + 5;
    }
    if (action2 != rmsDisabled) {
        cImage *imgRec = imgCache.GetIcon(iconRecord, iconSize, iconSize);
        if (imgRec) {
            pixmapIcons->DrawImage(cPoint(iconsX, iconY), *imgRec);
            iconsX += iconSize + 5;
        }
    }    
    return iconsX;
}

void cRecMenuItemEvent::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapText->SetLayer(-1);
    pixmapIcons->SetLayer(-1);    
}

void cRecMenuItemEvent::Show(void) { 
    pixmap->SetLayer(4);
    pixmapText->SetLayer(5);
    pixmapIcons->SetLayer(6);    
}

eRecMenuState cRecMenuItemEvent::ProcessKey(eKeys Key) {
    bool consumed = false;
    switch (Key & ~k_Repeat) {
        case kLeft:
            if (action2 == rmsDisabled)
                return rmsNotConsumed;
            if (iconActive == 1) {
                iconActive = 0;
                consumed = true;
            } 
            DrawIcons();
            if (consumed)
                return rmsConsumed;
            else
                return rmsNotConsumed;
            break;
        case kRight: {
            if (action2 == rmsDisabled)
                return rmsNotConsumed;
            if (iconActive == 0) {
                iconActive = 1;
                consumed = true;
            }   
            DrawIcons();
            if (consumed)
                return rmsConsumed;
            else
                return rmsNotConsumed;
            break; }
        case kOk:
            if (iconActive == 0)
                return action;
            else if (iconActive == 1)
                return action2;
            break;
        default:
            break;
    }
    return rmsNotConsumed;
}

// --- cRecMenuItemChannelChooser -------------------------------------------------------
cRecMenuItemChannelChooser::cRecMenuItemChannelChooser(cString text,
                                                       cChannel *initialChannel,
                                                       bool active) {
    selectable = true;
    this->text = text;
    this->channel = initialChannel;
    if (initialChannel)
        initialChannelSet = true;
    else
        initialChannelSet = false;
    channelNumber = 0;
    fresh = true;
    this->active = active;
    height = 2 * font->Height();
    pixmapChannel = NULL;
}

cRecMenuItemChannelChooser::~cRecMenuItemChannelChooser(void) {
    if (pixmapChannel)
        osdManager.releasePixmap(pixmapChannel);
}

void cRecMenuItemChannelChooser::SetPixmaps(void) {
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
        pixmapChannel = osdManager.requestPixmap(5, cRect(x, y, width, height));
    } else {
        pixmap->SetViewPort(cRect(x, y, width, height));
        pixmapChannel->SetViewPort(cRect(x, y, width, height));
    }
}

void cRecMenuItemChannelChooser::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapChannel->SetLayer(-1);
}

void cRecMenuItemChannelChooser::Show(void) { 
    pixmap->SetLayer(4);
    pixmapChannel->SetLayer(5);
}

void cRecMenuItemChannelChooser::Draw(void) {
    int textY = (height - font->Height()) / 2;
    pixmap->DrawText(cPoint(10, textY), *text, colorText, colorTextBack, font);
    DrawValue();    
}

void cRecMenuItemChannelChooser::DrawValue(void) {
    pixmapChannel->Fill(clrTransparent);
    int textY = (height - font->Height()) / 2;
    if (channel) {
        cString textVal = cString::sprintf("%d - %s", channel->Number(), channel->Name());
        int textX = width - font->Width(*textVal) - 10;
        pixmapChannel->DrawText(cPoint(textX, textY), *textVal, colorText, clrTransparent, font);
        int logoWidth = height * tvguideConfig.logoWidthRatio / tvguideConfig.logoHeightRatio;
        int logoX = textX - logoWidth - 10;
        cImageLoader imgLoader;
        if (imgLoader.LoadLogo(channel, logoWidth, height)) {
            cImage logo = imgLoader.GetImage();
            pixmapChannel->DrawImage(cPoint(logoX, 0), logo);
        }
    } else {
        cString textVal = tr("all Channels");
        int textX = width - font->Width(*textVal) - 10;
        pixmapChannel->DrawText(cPoint(textX, textY), *textVal, colorText, clrTransparent, font);
    }
}

int cRecMenuItemChannelChooser::GetIntValue(void) { 
    if (channel)
        return channel->Number(); 
    return 0;
}


eRecMenuState cRecMenuItemChannelChooser::ProcessKey(eKeys Key) {
    switch (Key & ~k_Repeat) {
        case kLeft: {
            fresh = true;
            if (!channel)
                return rmsConsumed;
            cChannel *prev = channel;
            cChannel *firstChannel = Channels.First();
            if(firstChannel->GroupSep())
                firstChannel = Channels.Next(firstChannel);
            if (prev == firstChannel) {
                if (!initialChannelSet)
                    channel = NULL;
            } else {
                while (prev = Channels.Prev(prev)) {
                    if(!prev->GroupSep()) {
                        channel = prev;
                        break;
                    }    
                }
            }
            DrawValue();
            return rmsConsumed;
            break; }
        case kRight: {
            fresh = true;
            if (!channel) {
                channel = Channels.First();
                if(channel->GroupSep())
                    channel = Channels.Next(channel);
            } else {
                cChannel *next = channel;
                while (next = Channels.Next(next)) {
                    if(!next->GroupSep()) {
                        channel = next;
                        break;
                    }    
                }
            }
            DrawValue();
            return rmsConsumed;
            break; }
        case k0 ... k9: {
            if (fresh) {
               channelNumber = 0;
               fresh = false;
            }
            channelNumber = channelNumber * 10 + (Key - k0);
            cChannel *chanNew = Channels.GetByNumber(channelNumber);
            if (chanNew) {
                channel = chanNew;
                DrawValue();
            }
            return rmsConsumed;
            break; }
        default:
            break;
    }
    return rmsNotConsumed;
}

// --- cRecMenuItemDayChooser -------------------------------------------------------
cRecMenuItemDayChooser::cRecMenuItemDayChooser(cString text,
                                               int weekdays,
                                               bool active) {
    selectable = true;
    this->text = text;
    this->weekdays = weekdays;
    this->active = active;
    height = 2 * font->Height();
    selectedDay = 0;
    pixmapWeekdays = NULL;
    pixmapWeekdaysSelect = NULL;
}

cRecMenuItemDayChooser::~cRecMenuItemDayChooser(void) {
    if (pixmapWeekdays)
        osdManager.releasePixmap(pixmapWeekdays);
    if (pixmapWeekdaysSelect)
        osdManager.releasePixmap(pixmapWeekdaysSelect);
}

void cRecMenuItemDayChooser::SetPixmaps(void) {
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
        pixmapWeekdays = osdManager.requestPixmap(5, cRect(x, y, width, height));
        pixmapWeekdaysSelect = osdManager.requestPixmap(6, cRect(x, y, width, height));
    } else {
        pixmap->SetViewPort(cRect(x, y, width, height));
        pixmapWeekdays->SetViewPort(cRect(x, y, width, height));
        pixmapWeekdaysSelect->SetViewPort(cRect(x, y, width, height));
    }
    SetSizes();
}

void cRecMenuItemDayChooser::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapWeekdays->SetLayer(-1);
    pixmapWeekdaysSelect->SetLayer(-1);
}

void cRecMenuItemDayChooser::Show(void) { 
    pixmap->SetLayer(4);
    pixmapWeekdays->SetLayer(5);
    pixmapWeekdaysSelect->SetLayer(6);
}

void cRecMenuItemDayChooser::SetSizes(void) {
    days = trVDR("MTWTFSS");
    int maxWidth = 0;
    for (unsigned i=0; i<days.length(); ++i) {
        int charWidth = font->Width(days.at(i));
        if (charWidth > maxWidth)
            maxWidth = charWidth;
    }    
    daysSize = min(maxWidth + 15, height-4);
    daysX = width - 10 - 7*daysSize;
    daysY = (height  - daysSize) / 2;
}

void cRecMenuItemDayChooser::setBackground() {
    cRecMenuItem::setBackground();
    if (active) {
        DrawHighlight(selectedDay);
    } else {
        DrawHighlight(-1);
    }
}

void cRecMenuItemDayChooser::Draw(void) {
    int textY = (height - font->Height()) / 2;
    pixmap->DrawText(cPoint(10, textY), *text, colorText, colorTextBack, font);
    DrawDays();
}

void cRecMenuItemDayChooser::DrawDays(void) {
    pixmapWeekdays->Fill(clrTransparent);
    int textY = (height - font->Height()) / 2;
    pixmapWeekdays->DrawRectangle(cRect(daysX, daysY, 7*daysSize, daysSize), theme.Color(clrBorder));
    int currentX = daysX;
    for (unsigned day=0; day<days.length(); ++day) {
        cString strDay = cString::sprintf("%c", days.at(day));
        pixmapWeekdays->DrawRectangle(cRect(currentX+2, daysY+2, daysSize-4, daysSize-4), theme.Color(clrBackground));
        tColor colorDay = WeekDaySet(day)?theme.Color(clrRecMenuDayActive)
                                         :theme.Color(clrRecMenuDayInactive);
        int textX = currentX + (daysSize - font->Width(*strDay)) / 2;
        pixmapWeekdays->DrawText(cPoint(textX, textY), *strDay, colorDay, clrTransparent, font);
        currentX += daysSize;
    }
}

void cRecMenuItemDayChooser::DrawHighlight(int day) {
    pixmapWeekdaysSelect->Fill(clrTransparent);
    if (day > -1) {
        int currentX = daysX + day*daysSize;
        pixmapWeekdaysSelect->DrawRectangle(cRect(currentX, daysY, daysSize, daysSize), theme.Color(clrRecMenuDayHighlight));
    }
}

bool cRecMenuItemDayChooser::WeekDaySet(unsigned day) {
    return weekdays & (1 << day);
}

void cRecMenuItemDayChooser::ToggleDay(void) {
    bool dayActive = WeekDaySet(selectedDay);
    int dayBit = pow(2, selectedDay);
    if (dayActive) {
        weekdays -= dayBit;
    } else {
        weekdays += dayBit;
    }
}

eRecMenuState cRecMenuItemDayChooser::ProcessKey(eKeys Key) {
    switch (Key & ~k_Repeat) {
        case kLeft: {
            selectedDay--;
            if (selectedDay<0)
                selectedDay += 7;
            DrawHighlight(selectedDay);
            return rmsConsumed;
            break; }
        case kRight: {
            selectedDay = (selectedDay+1)%7;
            DrawHighlight(selectedDay);
            return rmsConsumed;
            break; }
        case kOk:
            ToggleDay();
            DrawDays();
            return rmsConsumed;
            break;
        default:
            break;
    }
    return rmsNotConsumed;
}

// --- cRecMenuItemRecording  -------------------------------------------------------
cRecMenuItemRecording::cRecMenuItemRecording(cRecording *recording, bool active) {
    selectable = true;
    this->recording = recording;
    this->active = active;
    height = font->Height() + 2*fontSmall->Height() + 10;
    pixmapText = NULL;
}

cRecMenuItemRecording::~cRecMenuItemRecording(void) {
    if (pixmapText)
        osdManager.releasePixmap(pixmapText);
}

void cRecMenuItemRecording::SetPixmaps(void) {
    if (!pixmap) {
        pixmap = osdManager.requestPixmap(4, cRect(x, y, width, height));
        pixmapText = osdManager.requestPixmap(5, cRect(x, y, width, height));
        pixmapText->Fill(clrTransparent);
    } else {
        pixmap->SetViewPort(cRect(x, y, width, height));
        pixmapText->SetViewPort(cRect(x, y, width, height));
    }
}

void cRecMenuItemRecording::Draw(void) {
    if (!recording)
        return;
    const cRecordingInfo *recInfo = recording->Info();
    cChannel *channel = Channels.GetByChannelID(recInfo->ChannelID());
    cString channelName = tr("unknown channel");
    if (channel)
        channelName = channel->Name();
    cString name = recording->Name();
    cString dateTime = cString::sprintf("%s, %s", *DateString(recording->Start()), *TimeString(recording->Start()));
    
    int recDuration = recording->LengthInSeconds() / 60;
    
    cString recDetails = cString::sprintf("%s: %d %s, %s %s %s \"%s\"", tr("Duration"), recDuration, tr("min"), tr("recorded at"), *dateTime, tr("from"), *channelName);
    recDetails = CutText(*recDetails, width - 40, fontSmall).c_str();
    int text1Y = (height/2 - font->Height()) / 2 + 5;
    int text2Y = height/2 + (height/2 - fontSmall->Height())/2 - 5;
    colorText = active?theme.Color(clrFontActive):theme.Color(clrFont);
    pixmapText->DrawText(cPoint(10, text1Y), *name, colorText, clrTransparent, font);
    pixmapText->DrawText(cPoint(10, text2Y), *recDetails, colorText, clrTransparent, fontSmall);
}

void cRecMenuItemRecording::Hide(void) { 
    pixmap->SetLayer(-1);
    pixmapText->SetLayer(-1);
}

void cRecMenuItemRecording::Show(void) { 
    pixmap->SetLayer(4);
    pixmapText->SetLayer(5);
}
