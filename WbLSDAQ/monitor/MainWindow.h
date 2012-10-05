#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "TGFrame.h"

#include <string>

class TTimer;
class TRootEmbeddedCanvas;
class TCanvas;
class TH1F;
class TTree;

class MainWindow: public TGMainFrame
{
public:
    MainWindow(const TGWindow *p, int w,int h);
    virtual ~MainWindow();
    
    void Reset();
    int LoadLastSpill();
    int Update();
    int DoDraw();
    
    Bool_t HandleTimer(TTimer *t);
    std::string GetLastFile();
    void SetProperties();
    void FixCHAxisRange();
    void InitSpillTree();
    
    TRootEmbeddedCanvas *fEcanvas;
    TCanvas *fCanvas;
    TTimer *fTimer;
    TTree* fSpillTree;
    TCanvas *fTempCanvas;
    
    string fLastFile;
    double fCHymin[4];
    double fCHymax[4];
    TH1F* hCH0;
    TH1F* hCH1;
    TH1F* hCH2;
    TH1F* hCH3;
    
    TH1F* hChargeTub1;
    TH1F* hChargeTub2;
    TH1F* hChargeCounter1Pulse1;
    TH1F* hChargeCounter1Pulse2;
    TH1F* hChargeCounter2Pulse1;
    TH1F* hChargeCounter2Pulse2;
    
    TH1F* hTDCTub1;
    TH1F* hTDCTub2;
    TH1F* hTDCCounter1Pulse1;
    TH1F* hTDCCounter1Pulse2;
    TH1F* hTDCCounter2Pulse1;
    TH1F* hTDCCounter2Pulse2;
    
    int s_nTriggers;
    Float_t s_meanCharge_Tub1; 
    Float_t s_meanCharge_Tub2; 
    Float_t s_meanCharge_Counter1Pulse1; 
    Float_t s_meanCharge_Counter1Pulse2; 
    Float_t s_meanCharge_Counter2Pulse1; 
    Float_t s_meanCharge_Counter2Pulse2;
    
    Float_t r_meanCharge_Tub;
    Float_t r_meanCharge_Counter1;
    Float_t r_meanCharge_Counter2;
    
    int g_nSpills;
    
    ClassDef(MainWindow, 0)
};

#endif