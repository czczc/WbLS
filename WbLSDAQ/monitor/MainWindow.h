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
    
    string fLastFile;
    double fCHymin[4];
    double fCHymax[4];
    TH1F* hCH0;
    TH1F* hCH1;
    TH1F* hCH2;
    TH1F* hCH3;
    
    int s_nTriggers;
    
    ClassDef(MainWindow, 0)
};

#endif