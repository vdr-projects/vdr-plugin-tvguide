#ifndef __TVGUIDE_STATUSHEADER_H
#define __TVGUIDE_STATUSHEADER_H

// --- cStatusHeader  -------------------------------------------------------------

class cStatusHeader : public cStyledPixmap {
private:
    cPixmap *pixmapText;
    cPixmap *pixmapTVFrame;
public:
    cStatusHeader(void);
    virtual ~cStatusHeader(void);
    void ScaleVideo(void);
    void DrawInfoText(cGrid *grid);
};

#endif //__TVGUIDE_STATUSHEADER_H