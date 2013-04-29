#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "TGFrame.h"
#include "WblsDaqEvent.h"

#include <string>
#include <map>
#include <string>

class TTimer;
class TRootEmbeddedCanvas;
class TCanvas;
class TH1F;
class TTree;
class TText;
class TLine;
class TGraph;

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
    void SetFixedRanges();
    void SetDynamicRanges();
    void SetSignalWindow();
    void SetProperties();
    // void InitSpillTree();
    void InitMaps();
    
    TRootEmbeddedCanvas *fEcanvas;
    TCanvas *fCanvas;
    TTimer *fTimer;
    TTree* fSpillTree;
    // TCanvas *fTempCanvas;
    map<int, std::string> fRunTypeMap;
    map<int, std::string> fSampleTypeMap;
    bool fIsFakeData;
    
    string fLastFile;
    int f_run_number;
    int f_runtype;
    int f_sampletype;
    time_t f_runstart;
    time_t f_runstop;
    TText *fText_run_number;
    TText *fText_runtype;
    TText *fText_sampletype;
    TText *fText_runstart;
    TText *fText_runstop;
    TLine *fLine_SigTub[2][2];
    TLine *fLine_SigCounter[2][2][2];
    
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
    
    TH1F* hDtTriggers;
    
    TGraph* h_nTriggers;
    TGraph* h_meanCharge_Tub1;
    TGraph* h_meanCharge_Tub2;
    TGraph* h_meanCharge_Counter1Pulse1;
    TGraph* h_meanCharge_Counter1Pulse2;
    TGraph* h_meanCharge_Counter2Pulse1;
    TGraph* h_meanCharge_Counter2Pulse2;
    
    int s_nTriggers;
    Float_t s_meanCharge_Tub1; 
    Float_t s_meanCharge_Tub2; 
    Float_t s_meanCharge_Counter1Pulse1; 
    Float_t s_meanCharge_Counter1Pulse2; 
    Float_t s_meanCharge_Counter2Pulse1; 
    Float_t s_meanCharge_Counter2Pulse2;
    
    double r_CHymin[4];
    double r_CHymax[4];
    double r_pulsetime_count_scaling;
    double r_meanCharge_Tub;
    double r_meanCharge_Counter1;
    double r_meanCharge_Counter2;
    double r_dtTrigger_min;
    double r_dtTrigger_max;
    
    int g_nSpills;
    double g_tLastTrigger;
    bool g_processDone;
    
    double g_tStartTub[2]; 
    double g_tStopTub[2];
    double g_tStartCounterPulse[2][2];
    double g_tStopCounterPulse[2][2];
        
    ClassDef(MainWindow, 0)
};

#endif