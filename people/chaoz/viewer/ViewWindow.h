#ifndef VIEW_WINDOW_H
#define VIEW_WINDOW_H

#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"

class MCEvent;
class TCanvas;
class TH2F;
class TBox;
class TEllipse;
class TPad;
class TH1F;
class TMarker;

class ViewWindow: public TRootEmbeddedCanvas
{
public:
    ViewWindow(const TGWindow *p, int w,int h);
    virtual ~ViewWindow();

    enum LIMITS {
        MAXTRACK = 100,
        MAXPMT = 20000,
        MAXHIT = 50000,
    };

    struct CUTS {
        int tc_min; int tc_max;  // hit time (vertex corrected) [ns]
    } cuts;

    TCanvas* Canvas() { return GetCanvas(); };
    TPad* TopPanel() { return fTopPanel; }
    TPad* BotPanel() { return fBotPanel; }
    void SetCurrentEvent(int eventNo) { currentEvent = eventNo; }
    void Open(const char* fileName);

    void UpdateHitTime();
    void UpdatePMT(int id);
    void UpdatePMTMap();
    void UpdateCanvas();

    int currentEvent;
    MCEvent *fEvent;

    TH2F *hContainer;
    TBox *WCMain;
    TEllipse *WCTop, *WCBottom;
    TH1F *hHitTime;
    TMarker* pmtMarker[MAXPMT];
    int pmtPE[MAXPMT];

private:
    TPad *fBotPanel;
    TPad *fTopPanel;

    void InitCuts();
    void InitHist();
    void InitCanvas();
    void InitStyle();
    void InitPMT(int id);
    void InitDrawHist();

    ClassDef(ViewWindow, 0)
};

#endif
