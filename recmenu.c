#include "recmenu.h"

// --- cRecMenu  -------------------------------------------------------------

cRecMenu::cRecMenu(void) {
    border = geoManager.borderRecMenus;
    height = 2*border;
    headerHeight = 0;
    footerHeight = 0;
    scrollHeight = 0;
    scrollItemHeight = 0;
    scrollable = false;
    scrollbarWidth = 3 * border;
    pixmapScrollBar = NULL;
    imgScrollBar = NULL;
    startIndex = 0;
    stopIndex = 0;
    numItems = 0;
    header = NULL;
    footer = NULL;
}

cRecMenu::~cRecMenu(void) {
    if (header)
        delete header;
    menuItems.Clear();
    if (footer)
        delete footer;
    if (pixmapScrollBar)
        osdManager.releasePixmap(pixmapScrollBar);
    if (imgScrollBar)
        delete imgScrollBar;
}

void cRecMenu::SetWidthPercent(int percentOSDWidth) {
    width = geoManager.osdWidth * percentOSDWidth / 100; 
    x = (geoManager.osdWidth - width) / 2;
}

void cRecMenu::SetWidthPixel(int pixel) {
    width = pixel;
    x = (geoManager.osdWidth - width) / 2;
}

int cRecMenu::CalculateOptimalWidth(void) {
    int optWidth = 0;
    for (cRecMenuItem *item = menuItems.First(); item; item = menuItems.Next(item)) {
        int itemWidth = item->GetWidth();
        if (itemWidth > optWidth)
            optWidth = itemWidth;
    }
    return optWidth;
}


void cRecMenu::AddMenuItem(cRecMenuItem *item, cRecMenuItem *before) {
    if (!before)
        menuItems.Add(item);
    else
        menuItems.Ins(item, before);
}

void cRecMenu::AddMenuItemScroll(cRecMenuItem *item) {
    scrollHeight += item->GetHeight();
    stopIndex++;
    numItems++;
    if (scrollItemHeight == 0)
        scrollItemHeight = item->GetHeight();
    menuItems.Add(item);
}

bool cRecMenu::CheckHeight(void) {
    int nextHeight = headerHeight + footerHeight + scrollHeight + 2*border + 150;
    if (nextHeight > geoManager.osdHeight) {
        scrollable = true;
        return false;
    }
    return true;
}

void cRecMenu::CalculateHeight(void) {
    height = 2*border;
    if (header)
        height += headerHeight;
    for (cRecMenuItem *item = menuItems.First(); item; item = menuItems.Next(item)) {
        height += item->GetHeight(); 
    }
    if (footer)
        height += footerHeight;
    y = (geoManager.osdHeight - height) / 2;
    
    if (scrollable) {
        width += scrollbarWidth + border;
    }
}

void cRecMenu::CreatePixmap(void) {
    pixmap = osdManager.requestPixmap(3, cRect(x, y, width, height));
    if (scrollable) {
        int scrollBarX = x + width - scrollbarWidth - border;
        int scrollBarY = y + border + headerHeight;
        int scrollBarHeight = height - headerHeight - footerHeight - 2 * border;
        pixmapScrollBar = osdManager.requestPixmap(4, cRect(scrollBarX, scrollBarY, scrollbarWidth, scrollBarHeight));
    }
}

void cRecMenu::SetFooter(cRecMenuItem *footer) {
    this->footer = footer;
    footerHeight = footer->GetHeight();
    height += footerHeight;
}

void cRecMenu::SetHeader(cRecMenuItem *header) { 
    this->header = header; 
    headerHeight = header->GetHeight();
    height += headerHeight;
}

cRecMenuItem *cRecMenu::GetActiveMenuItem(void) {
    for (cRecMenuItem *item = menuItems.First(); item; item = menuItems.Next(item)) {
        if (item->isActive())
            return item;
    }
    if (footer && footer->isActive())
        return footer;
    return NULL;
}

int cRecMenu::GetActive(bool withOffset) {
    int numActive = withOffset?startIndex:0;
    int i = 0;
    for (cRecMenuItem *item = menuItems.First(); item; item = menuItems.Next(item)) {
        if (item->isActive()) {
            numActive += i;
            break;
        }
        i++;
    }
    return numActive;
}

bool cRecMenu::ActivatePrev(void) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    if (!scrollable && footer && footer->isActive()) {
        Activate(footer, menuItems.Last());
        return true;
    } else if (activeItem) {
        cRecMenuItem *prev = NULL;
        for (cRecMenuItem *item = menuItems.Prev(activeItem); item; item = menuItems.Prev(item)) {
            if (item->isSelectable()) {
                prev = item;
                break;
            }
        }    
        if (prev) {
            Activate(activeItem , prev);
            return true;
        }
    }
    return false;
}

bool cRecMenu::ActivateNext(void) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    if (activeItem) {
        cRecMenuItem *next = NULL;
        for (cRecMenuItem *item = menuItems.Next(activeItem); item; item = menuItems.Next(item)) {
            if (item->isSelectable()) {
                next = item;
                break;
            }
        }    
        if (next) {
            Activate(activeItem , next);
            return true;
        } else if (!scrollable && footer && footer->isSelectable()) {
            Activate(activeItem , footer);
            return true;
        }
    } 
    return false;
}

void cRecMenu::Activate(cRecMenuItem *itemOld, cRecMenuItem *item) {
    itemOld->setInactive();
    itemOld->setBackground();
    itemOld->Draw();
    item->setActive();
    item->setBackground();
    item->Draw();
}

void cRecMenu::ScrollUp(void) {
    if (footer && footer->isActive()) {
        Activate(footer, menuItems.Last());
    } else {
        //get perv x items
        int numNewItems = numItems / 2;
        int numAdded = 0;
        cRecMenuItem *newItem = NULL;
        while (newItem = GetMenuItem(startIndex-1)) {
            AddMenuItem(newItem, menuItems.First());
            menuItems.Del(menuItems.Last(), true);
            stopIndex--;
            startIndex--;
            numAdded++;
            if (numAdded >= numNewItems)
                break;
        }
        if (numAdded != 0) {
            Arrange(true);
            Display(true);
            ActivatePrev();
        }
    }
}

void cRecMenu::ScrollDown(void) {
    //get next x items
    int numNewItems = numItems / 2;
    int numAdded = 0;
    cRecMenuItem *newItem = NULL;
    while (newItem = GetMenuItem(stopIndex)) {
        menuItems.Add(newItem);
        menuItems.Del(menuItems.First(), true);
        stopIndex++;
        startIndex++;
        numAdded++;
        if (numAdded >= numNewItems)
            break;
    }
    if (numAdded != 0) {
        Arrange(true);
        Display(true);
        ActivateNext();
    } else {
        //last item reached, activate footer
        if (footer) {
            cRecMenuItem *activeItem = GetActiveMenuItem();
            Activate(activeItem , footer);
        }
    }
}

void cRecMenu::JumpBegin(void) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    if (!scrollable) {
        cRecMenuItem *firstSelectable= NULL;
        for (cRecMenuItem *item = menuItems.First(); item; item = menuItems.Next(item)) {
            if (item->isSelectable()) {
                firstSelectable = item;
                break;
            }
        }
        if (activeItem && firstSelectable) {
            Activate(activeItem , firstSelectable);
        }
    } else {
        activeItem->setInactive();
        activeItem->setBackground();
        if (footer)
            footer->Draw();
        menuItems.Clear();
        int currentItem = 0;
        cRecMenuItem *newItem = NULL;
        while (newItem = GetMenuItem(currentItem)) {
            AddMenuItem(newItem);
            currentItem++;
            if (currentItem >= numItems)
                break;
        }
        Arrange(true);
        startIndex = 0;
        stopIndex = numItems-1;
        menuItems.First()->setActive();
        menuItems.First()->setBackground();
        menuItems.First()->Draw();
        Display(true);    
    }
}

void cRecMenu::JumpEnd(void) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    if (!activeItem)
        return;
    if (!scrollable) {
        cRecMenuItem *lastSelectable= NULL;
        if (footer && footer->isSelectable()) {
            lastSelectable = footer;
        } else {
            for (cRecMenuItem *item = menuItems.Last(); item; item = menuItems.Prev(item)) {
                if (item->isSelectable()) {
                    lastSelectable = item;
                    break;
                }
            }
        }
        if (lastSelectable) {
            Activate(activeItem , lastSelectable);
        }
    } else {
        activeItem->setInactive();
        activeItem->setBackground();
        menuItems.Clear();
        int totalNumItems = GetTotalNumMenuItems();
        int currentItem = totalNumItems-1;
        int itemsAdded = 0;
        cRecMenuItem *newItem = NULL;
        while (newItem = GetMenuItem(currentItem)) {
            AddMenuItem(newItem, menuItems.First());
            currentItem--;
            itemsAdded++;
            if (itemsAdded >= numItems)
                break;
        }
        Arrange(true);
        stopIndex = totalNumItems;
        startIndex = stopIndex - numItems;
        if (footer) {
            footer->setActive();
            footer->setBackground();
            footer->Draw();
        } else {
            menuItems.Last()->setActive();
            menuItems.Last()->setBackground();
            menuItems.Last()->Draw();
        }
        Display(true);
    }
}

void cRecMenu::Arrange(bool scroll) {
    int xElement = x + border;
    int yElement = y + border;
    int widthElement = width - 2 * border;
    if (scrollable)
        widthElement -= scrollbarWidth + border;

    if (header) {
        if (!scroll) {
            header->SetGeometry(xElement, yElement, widthElement);
            header->SetPixmaps();
            header->setBackground();
        }
        yElement += header->GetHeight();
    }
    for (cRecMenuItem *item = menuItems.First(); item; item = menuItems.Next(item)) {
        item->SetGeometry(xElement, yElement, widthElement);
        item->SetPixmaps();
        yElement += item->GetHeight();
    }
    if (footer && !scroll) {
        footer->SetGeometry(xElement, yElement, widthElement);
        footer->SetPixmaps();
        footer->setBackground();
    }
}

void cRecMenu::Display(bool scroll) {
    if (tvguideConfig.style == eStyleGraphical) {
        drawBackgroundGraphical(bgRecMenuBack);
    } else {
        pixmap->Fill(theme.Color(clrBackground));
        drawBorder();
    }
    if (header && !scroll) {
        header->setBackground();
        header->Draw();
    }
    for (cRecMenuItem *item = menuItems.First(); item; item = menuItems.Next(item)) {
        item->setBackground();
        item->Draw();
    }
    if (footer && !scroll) {
        footer->setBackground();
        footer->Draw();
    }
    if (scrollable)
        DrawScrollBar();
}

void cRecMenu::Hide(void) {
    pixmap->SetLayer(-1);
    if (pixmapScrollBar)
        pixmapScrollBar->SetLayer(-1);
    if (header)
        header->Hide();
    if (footer)
        footer->Hide();
    for (cRecMenuItem *item = menuItems.First(); item; item = menuItems.Next(item)) {
        item->Hide();
    }
}

void cRecMenu::Show(void) {
    pixmap->SetLayer(3);
    if (pixmapScrollBar)
        pixmapScrollBar->SetLayer(3);
    if (header)
        header->Show();
    if (footer)
        footer->Show();
    for (cRecMenuItem *item = menuItems.First(); item; item = menuItems.Next(item)) {
        item->Show();
    }
}

void cRecMenu::DrawScrollBar(void) {
    pixmapScrollBar->Fill(theme.Color(clrBorder));
    pixmapScrollBar->DrawRectangle(cRect(2,2,pixmapScrollBar->ViewPort().Width()-4, pixmapScrollBar->ViewPort().Height() - 4), theme.Color(clrBackground));
    
    int totalNumItems = GetTotalNumMenuItems();
    if (imgScrollBar == NULL) {
        int scrollBarImgHeight = (pixmapScrollBar->ViewPort().Height() - 8) * numItems / totalNumItems;
        imgScrollBar = createScrollbar(pixmapScrollBar->ViewPort().Width()-8, scrollBarImgHeight, theme.Color(clrHighlight), theme.Color(clrHighlightBlending));
    }
    int offset = (pixmapScrollBar->ViewPort().Height() - 8) * startIndex / totalNumItems;
    pixmapScrollBar->DrawImage(cPoint(4, 2 + offset), *imgScrollBar);
}

int cRecMenu::GetIntValue(int itemNumber) {
    cRecMenuItem *item = NULL;
    item = menuItems.Get(itemNumber);
    if (item) {
        return item->GetIntValue();
    }
    return -1;
}

time_t cRecMenu::GetTimeValue(int itemNumber) {
    cRecMenuItem *item = NULL;
    item = menuItems.Get(itemNumber);
    if (item) {
        return item->GetTimeValue();
    }
    return 0;
}

bool cRecMenu::GetBoolValue(int itemNumber) {
    cRecMenuItem *item = NULL;
    item = menuItems.Get(itemNumber);
    if (item) {
        return item->GetBoolValue();
    }
    return false;
}

cString cRecMenu::GetStringValue(int itemNumber) {
    cRecMenuItem *item = NULL;
    item = menuItems.Get(itemNumber);
    if (item) {
        return item->GetStringValue();
    }
    return cString("");
}

const cEvent *cRecMenu::GetEventValue(int itemNumber) {
    cRecMenuItem *item = NULL;
    item = menuItems.Get(itemNumber);
    if (item) {
        return item->GetEventValue();
    }
    return NULL;
}

cTimer *cRecMenu::GetTimerValue(int itemNumber) {
    cRecMenuItem *item = NULL;
    item = menuItems.Get(itemNumber);
    if (item) {
        return item->GetTimerValue();
    }
    return NULL;
}

eRecMenuState cRecMenu::ProcessKey(eKeys Key) {
    cRecMenuItem *activeItem = GetActiveMenuItem();
    eRecMenuState state = rmsContinue;
    if (!activeItem)
        return state;

    state = activeItem->ProcessKey(Key);
    if (state == rmsRefresh) {
        CreateMenuItems();
        Display();
    } else if (state == rmsNotConsumed) {
        switch (Key & ~k_Repeat) {
            case kUp:       
                if (!ActivatePrev() && scrollable)
                    ScrollUp();
                    state = rmsConsumed;
                break;
            case kDown:     
                if (!ActivateNext() && scrollable)
                    ScrollDown();
                    state = rmsConsumed;
                break;
            case kLeft:
                JumpBegin();
                state = rmsConsumed;
                break;
            case kRight:
                JumpEnd();
                state = rmsConsumed;
                break;
            default:
                break;
        }
    }
    return state;
}

cImage *cRecMenu::createScrollbar(int width, int height, tColor clrBgr, tColor clrBlend) {
    cImage *image = new cImage(cSize(width, height));
    image->Fill(clrBgr);
    if (tvguideConfig.style != eStyleFlat) {
        int numSteps = 64;
        int alphaStep = 0x03;
        if (height < 30)
            return image;
        else if (height < 100) {
            numSteps = 32;
            alphaStep = 0x06;
        }
        int stepY = 0.5*height / numSteps;
        if (stepY == 0)
            stepY = 1;
        int alpha = 0x40;
        tColor clr;
        for (int i = 0; i<numSteps; i++) {
            clr = AlphaBlend(clrBgr, clrBlend, alpha);
            for (int y = i*stepY; y < (i+1)*stepY; y++) {
                for (int x=0; x<width; x++) {
                    image->SetPixel(cPoint(x,y), clr);
                }
            }
            alpha += alphaStep;
        }
    }
    return image;
}