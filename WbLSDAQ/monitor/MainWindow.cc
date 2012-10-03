#include "MainWindow.h"
#include "WblsDaqEvent.h"

#include "TSystem.h"
#include "TTimer.h"
#include "TCanvas.h"
#include "TRootEmbeddedCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TTree.h"
#include "TDirectory.h"

#include <iostream>
#include <fstream>
using namespace std;

MainWindow::MainWindow(const TGWindow *p, int w,int h)
    : TGMainFrame(p, w, h)
{
    fTimer = new TTimer(this, 2000);
    fTimer->TurnOn();
    
    // fTimer->SetCommand("gMainWindow->HandleTimer(0)");
    
    hCH0 = new TH1F("hCH0", "Channel 0", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH1 = new TH1F("hCH1", "Channel 1", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH2 = new TH1F("hCH2", "Channel 2", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH3 = new TH1F("hCH3", "Channel 3", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    InitSpillTree();
        
    fEcanvas = new TRootEmbeddedCanvas("Ecanvas", this, 1200, 800);
    AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
    fCanvas = fEcanvas->GetCanvas();
    fCanvas->Divide(4, 3);
    
    // initialize window
    fLastFile = GetLastFile();
    // fLastFile = "../data/test/rootoutputfile409cosmicsWBLS7percent_shortcable_1315V.root";
    LoadLastSpill();
    FixCHAxisRange();
    SetProperties();
    DoDraw();
    
    // set main window stats
    SetWindowName("WbLS DAQ Monitor");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
    
}

MainWindow::~MainWindow()
{
    Cleanup();
    
    delete hCH0;
    delete hCH1;
    delete hCH2;
    delete hCH3;
    
}

void MainWindow::InitSpillTree()
{
    fSpillTree = new TTree("spillTree", "Stats of Each Spill");
    fSpillTree->Branch("nTriggers", &s_nTriggers);
}

Bool_t MainWindow::HandleTimer(TTimer *t)
{
    // Handle timer event and draws function graphics
    Update();
    fCanvas->Modified();
    fCanvas->Update();
    // gSystem->ProcessEvents();
    fTimer->Reset();
    // cout << "updating ..." << endl;
    return kTRUE;
}

void MainWindow::FixCHAxisRange()
{
    fCHymin[0] = hCH0->GetMinimum(100)*0.95; fCHymax[0] = hCH0->GetMaximum()*1.05;
    fCHymin[1] = hCH1->GetMinimum(100)*0.95; fCHymax[1] = hCH1->GetMaximum()*1.05;
    fCHymin[2] = hCH2->GetMinimum(100)*0.95; fCHymax[2] = hCH2->GetMaximum()*1.05;
    fCHymin[3] = hCH3->GetMinimum(100)*0.95; fCHymax[3] = hCH3->GetMaximum()*1.05;
    
}

void MainWindow::SetProperties()
{    
    hCH0->GetXaxis()->SetTitle("[ns]");
    hCH1->GetXaxis()->SetTitle("[ns]");
    hCH2->GetXaxis()->SetTitle("[ns]");
    hCH3->GetXaxis()->SetTitle("[ns]");
    
    for (int i=1; i!=5; i++) {
        fCanvas->cd(i);
        gPad->SetGridx();
        gPad->SetGridy();
    }

}

int MainWindow::LoadLastSpill()
{
    for (unsigned bin=0; bin<WblsDaq::NFADCBins; ++bin) {
        hCH0->SetBinContent(bin, 0);
        hCH1->SetBinContent(bin, 0);
        hCH2->SetBinContent(bin, 0);
        hCH3->SetBinContent(bin, 0);
    }
    
    WblsDaq::Spinner spinner(fLastFile);
    s_nTriggers = spinner.entries();
    cout << fLastFile << ": entries: " << s_nTriggers << endl;
    
    for (int i=0; i < s_nTriggers; ++i) {
        // int nbytes = spinner.load(i);
        WblsDaq::Event* evt = spinner.event(i);    
        if (!evt) {
            cerr << "Failed to get event at entry #" << i << endl;
            return 2;
        }
        for (unsigned bin=0; bin<WblsDaq::NFADCBins; ++bin) {
            hCH0->SetBinContent(bin, evt->ch0[bin] + hCH0->GetBinContent(bin));
            hCH1->SetBinContent(bin, evt->ch1[bin] + hCH1->GetBinContent(bin));
            hCH2->SetBinContent(bin, evt->ch2[bin] + hCH2->GetBinContent(bin));
            hCH3->SetBinContent(bin, evt->ch3[bin] + hCH3->GetBinContent(bin));
        }
    }
    hCH0->Scale(1./s_nTriggers);
    hCH1->Scale(1./s_nTriggers);
    hCH2->Scale(1./s_nTriggers);
    hCH3->Scale(1./s_nTriggers);
    
    fSpillTree->Fill();
    
    return 1;
}

int MainWindow::Update()
{
    string lastfile = GetLastFile();
    if (lastfile == fLastFile) {
        // nothing to Update
        return 0;
    }
    fLastFile = lastfile;
    // cout << "new file: " << lastfile << endl;
    
    LoadLastSpill();
    DoDraw();
    return 1;
}

string MainWindow::GetLastFile()
{
    system("python monitor.py /Users/chaozhang/Projects/LBNE/WbLS/software/WbLSDAQ/data/test");
    ifstream in("lastfile.txt");
    string lastfile;
    in >> lastfile;
    in.close();
    return lastfile;
}

int MainWindow::DoDraw()
{
    fCanvas->cd(1);
    hCH0->GetYaxis()->SetRangeUser(fCHymin[0], fCHymax[0]);
    hCH0->Draw();

    fCanvas->cd(2);
    hCH1->GetYaxis()->SetRangeUser(fCHymin[1], fCHymax[1]);
    hCH1->Draw();
    
    fCanvas->cd(3);
    hCH2->GetYaxis()->SetRangeUser(fCHymin[2], fCHymax[2]);
    hCH2->Draw();
    
    fCanvas->cd(4);
    hCH3->GetYaxis()->SetRangeUser(fCHymin[3], fCHymax[3]);
    hCH3->Draw();
    
    fCanvas->cd(5);
    TH2F *h_nTriggers = (TH2F*)gDirectory->Get("h_nTriggers");
    if (h_nTriggers) delete h_nTriggers;
    fSpillTree->Draw("nTriggers:Entry$>>h_nTriggers");
    h_nTriggers = (TH2F*)gDirectory->Get("h_nTriggers");
    h_nTriggers->GetXaxis()->SetTitle("Spill #");
    h_nTriggers->SetTitle("Number of Triggers");
    h_nTriggers->Draw();
    
    return 1;
}

