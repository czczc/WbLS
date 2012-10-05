#include "MainWindow.h"
#include "WblsDaqEvent.h"
#include "WFAnalyzer.h"

#include "TSystem.h"
#include "TTimer.h"
#include "TCanvas.h"
#include "TRootEmbeddedCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TMath.h"
#include "TRoot.h"

#include <iostream>
#include <fstream>
using namespace std;

MainWindow::MainWindow(const TGWindow *p, int w,int h)
    : TGMainFrame(p, w, h)
{
    g_nSpills = 0;
    fTimer = new TTimer(this, 1000);
    fTimer->TurnOn();
    
    // fTimer->SetCommand("gMainWindow->HandleTimer(0)");
    
    hCH0 = new TH1F("hCH0", "Tub 1", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH1 = new TH1F("hCH1", "Tub 2", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH2 = new TH1F("hCH2", "Counter 1", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH3 = new TH1F("hCH3", "Counter 2", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    
    hChargeTub1 = new TH1F("hChargeTub1", "Charge of Tub 1", 100, 1, 7);
    hChargeTub2 = new TH1F("hChargeTub2", "Charge of Tub 2", 100, 1, 7);
    hChargeCounter1Pulse1 = new TH1F("hChargeCounter1Pulse1", "Charge of Counter 1", 100, 1, 7);
    hChargeCounter1Pulse2 = new TH1F("hChargeCounter1Pulse2", "Charge of Counter 1", 100, 1, 7);
    hChargeCounter2Pulse1 = new TH1F("hChargeCounter2Pulse1", "Charge of Counter 2", 100, 1, 7);
    hChargeCounter2Pulse2 = new TH1F("hChargeCounter2Pulse2", "Charge of Counter 2", 100, 1, 7);
    
    hTDCTub1 = new TH1F("hTDCTub1", "Pulse Time of Tub 1", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hTDCTub2 = new TH1F("hTDCTub2", "Pulse Time of Tub 2", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hTDCCounter1Pulse1 = new TH1F("hTDCCounter1Pulse1", "", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);;
    hTDCCounter1Pulse2 = new TH1F("hTDCCounter1Pulse2", "", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);;
    hTDCCounter2Pulse1 = new TH1F("hTDCCounter2Pulse1", "", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);;
    hTDCCounter2Pulse2 = new TH1F("hTDCCounter2Pulse2", "", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);;
    InitSpillTree();
    
    r_meanCharge_Tub = 80000.;
    r_meanCharge_Counter1 = 100000.;
    r_meanCharge_Counter2 = 500000.;
    
    fTempCanvas = new TCanvas("tempC", "tempC", 100, 100);
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
    cout << "Initialized. First File: " << fLastFile << endl;
    
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
    fSpillTree->Branch("meanCharge_Tub1", &s_meanCharge_Tub1);
    fSpillTree->Branch("meanCharge_Tub2", &s_meanCharge_Tub2);
    fSpillTree->Branch("meanCharge_Counter1Pulse1", &s_meanCharge_Counter1Pulse1);
    fSpillTree->Branch("meanCharge_Counter1Pulse2", &s_meanCharge_Counter1Pulse2);
    fSpillTree->Branch("meanCharge_Counter2Pulse1", &s_meanCharge_Counter2Pulse1);
    fSpillTree->Branch("meanCharge_Counter2Pulse2", &s_meanCharge_Counter2Pulse2);
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
    fCHymin[0] = hCH0->GetMinimum(100)*0.8; fCHymax[0] = 8200;
    fCHymin[1] = hCH1->GetMinimum(100)*0.8; fCHymax[1] = 8200;
    fCHymin[2] = hCH2->GetMinimum(100)*0.8; fCHymax[2] = 8200;
    fCHymin[3] = hCH3->GetMinimum(100)*0.8; fCHymax[3] = 8200;
    
}

void MainWindow::SetProperties()
{    
    hCH0->GetXaxis()->SetTitle("[tdc]");
    hCH1->GetXaxis()->SetTitle("[tdc]");
    hCH2->GetXaxis()->SetTitle("[tdc]");
    hCH3->GetXaxis()->SetTitle("[tdc]");
    
    hChargeTub1->GetXaxis()->SetTitle("log_{10}(Charge)");
    hChargeTub2->GetXaxis()->SetTitle("log_{10}(Charge)");
    hChargeCounter1Pulse1->GetXaxis()->SetTitle("log_{10}(Charge)");
    hChargeCounter1Pulse2->GetXaxis()->SetTitle("log_{10}(Charge)");
    hChargeCounter2Pulse1->GetXaxis()->SetTitle("log_{10}(Charge)");
    hChargeCounter2Pulse2->GetXaxis()->SetTitle("log_{10}(Charge)");
    
    hChargeTub1->SetLineColor(kRed);
    hChargeTub2->SetLineColor(kBlue);
    hChargeCounter1Pulse1->SetLineColor(kRed);
    hChargeCounter1Pulse2->SetLineColor(kBlue);
    hChargeCounter2Pulse1->SetLineColor(kRed);
    hChargeCounter2Pulse2->SetLineColor(kBlue);
    
    hTDCTub1->SetLineColor(kRed);
    hTDCTub2->SetLineColor(kBlue);
    hTDCCounter1Pulse1->SetLineColor(kRed);
    hTDCCounter1Pulse2->SetLineColor(kBlue); 
    hTDCCounter2Pulse1->SetLineColor(kRed); 
    hTDCCounter2Pulse2->SetLineColor(kBlue); 
    
    
    for (int i=1; i!=5; i++) {
        fCanvas->cd(i);
        gPad->SetGridx();
        gPad->SetGridy();
    }
    for (int i=10; i!=13; i++) {
        fCanvas->cd(i);
        gPad->SetGridy();
    }

}

void MainWindow::Reset()
{
    for (unsigned bin=0; bin<=WblsDaq::NFADCBins; ++bin) {
        hCH0->SetBinContent(bin, 0);
        hCH1->SetBinContent(bin, 0);
        hCH2->SetBinContent(bin, 0);
        hCH3->SetBinContent(bin, 0);
        
        hTDCTub1->SetBinContent(bin, 0);
        hTDCTub2->SetBinContent(bin, 0);
        hTDCCounter1Pulse1->SetBinContent(bin, 0);
        hTDCCounter1Pulse2->SetBinContent(bin, 0);
        hTDCCounter2Pulse1->SetBinContent(bin, 0);
        hTDCCounter2Pulse2->SetBinContent(bin, 0);
    }
    for (unsigned bin=0; bin<=100; ++bin) {
        hChargeTub1->SetBinContent(bin, 0);
        hChargeTub1->SetEntries(0);
        hChargeTub2->SetBinContent(bin, 0);
        hChargeTub2->SetEntries(0);
        
        hChargeCounter1Pulse1->SetBinContent(bin, 0);
        hChargeCounter1Pulse2->SetBinContent(bin, 0);
        hChargeCounter2Pulse1->SetBinContent(bin, 0);
        hChargeCounter2Pulse2->SetBinContent(bin, 0);
    }
    
    s_meanCharge_Tub1 = 0;
    s_meanCharge_Tub2 = 0;
    s_meanCharge_Counter1Pulse1 = 0;
    s_meanCharge_Counter1Pulse2 = 0;
    s_meanCharge_Counter2Pulse1 = 0;
    s_meanCharge_Counter2Pulse2 = 0;
}

int MainWindow::LoadLastSpill()
{
    Reset();
    g_nSpills++;
    
    WblsDaq::Spinner spinner(fLastFile);
    s_nTriggers = spinner.entries();
    cout << fLastFile << ": entries: " << s_nTriggers << endl;
    
    for (int i=0; i < s_nTriggers; ++i) {
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
        
        WFAnalyzer wf(evt);
        wf.Process();
        hChargeTub1->Fill(TMath::Log10(wf.fCharge_Tub[0]));
        hChargeTub2->Fill(TMath::Log10(wf.fCharge_Tub[1]));
        if (wf.fCharge_Counter[0][0]>0) hChargeCounter1Pulse1->Fill(TMath::Log10(wf.fCharge_Counter[0][0]));
        if (wf.fCharge_Counter[0][1]>0) hChargeCounter1Pulse2->Fill(TMath::Log10(wf.fCharge_Counter[0][1]));
        if (wf.fCharge_Counter[1][0]>0) hChargeCounter2Pulse1->Fill(TMath::Log10(wf.fCharge_Counter[1][0]));
        if (wf.fCharge_Counter[1][1]>0) hChargeCounter2Pulse2->Fill(TMath::Log10(wf.fCharge_Counter[1][1]));
        
        hTDCTub1->Fill(wf.fTDC_Tub[0]);
        hTDCTub2->Fill(wf.fTDC_Tub[1]);
        hTDCCounter1Pulse1->Fill(wf.fTDC_Counter[0][0]);
        hTDCCounter1Pulse2->Fill(wf.fTDC_Counter[0][1]);
        hTDCCounter2Pulse1->Fill(wf.fTDC_Counter[1][0]);
        hTDCCounter2Pulse2->Fill(wf.fTDC_Counter[1][1]);
        
        s_meanCharge_Tub1 += wf.fCharge_Tub[0];
        s_meanCharge_Tub2 += wf.fCharge_Tub[1];
        s_meanCharge_Counter1Pulse1 += wf.fCharge_Counter[0][0];
        s_meanCharge_Counter1Pulse2 += wf.fCharge_Counter[0][1];
        s_meanCharge_Counter2Pulse1 += wf.fCharge_Counter[1][0];
        s_meanCharge_Counter2Pulse2 += wf.fCharge_Counter[1][1];
    }
    hCH0->Scale(1./s_nTriggers);
    hCH1->Scale(1./s_nTriggers);
    hCH2->Scale(1./s_nTriggers);
    hCH3->Scale(1./s_nTriggers);
    s_meanCharge_Tub1 /= s_nTriggers;
    s_meanCharge_Tub2 /= s_nTriggers;
    s_meanCharge_Counter1Pulse1 /= s_nTriggers;
    s_meanCharge_Counter1Pulse2 /= s_nTriggers;
    s_meanCharge_Counter2Pulse1 /= s_nTriggers;
    s_meanCharge_Counter2Pulse2 /= s_nTriggers;
    // cout << s_meanCharge_Counter2Pulse1 << ", " << s_meanCharge_Counter2Pulse2 << endl;
    
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
    for (unsigned bin=0; bin<=WblsDaq::NFADCBins; ++bin) {
        hTDCTub1->SetBinContent(bin, fCHymin[0] + hTDCTub1->GetBinContent(bin)*100);
        hTDCTub2->SetBinContent(bin, fCHymin[1] + hTDCTub2->GetBinContent(bin)*100);
        hTDCCounter1Pulse1->SetBinContent(bin, fCHymin[2] + hTDCCounter1Pulse1->GetBinContent(bin)*100);
        hTDCCounter1Pulse2->SetBinContent(bin, fCHymin[2] + hTDCCounter1Pulse2->GetBinContent(bin)*100);
        hTDCCounter2Pulse1->SetBinContent(bin, fCHymin[3] + hTDCCounter2Pulse1->GetBinContent(bin)*100);
        hTDCCounter2Pulse2->SetBinContent(bin, fCHymin[3] + hTDCCounter2Pulse2->GetBinContent(bin)*100);
    }
    
    fCanvas->cd(1);
    hCH0->GetXaxis()->SetRangeUser(500, 2000);
    hCH0->GetYaxis()->SetRangeUser(fCHymin[0], fCHymax[0]);
    hCH0->Draw();
    hTDCTub1->Draw("same");
    
    fCanvas->cd(2);
    hCH1->GetXaxis()->SetRangeUser(500, 2000);
    hCH1->GetYaxis()->SetRangeUser(fCHymin[1], fCHymax[1]);
    hCH1->Draw();
    hTDCTub2->Draw("same");
    
    fCanvas->cd(3);
    hCH2->GetXaxis()->SetRangeUser(500, 2000);
    hCH2->GetYaxis()->SetRangeUser(fCHymin[2], fCHymax[2]);
    hCH2->Draw();
    hTDCCounter1Pulse1->Draw("same");
    hTDCCounter1Pulse2->Draw("same");
    
    fCanvas->cd(4);
    hCH3->GetXaxis()->SetRangeUser(500, 2000);
    hCH3->GetYaxis()->SetRangeUser(fCHymin[3], fCHymax[3]);
    hCH3->Draw();
    hTDCCounter2Pulse1->Draw("same");
    hTDCCounter2Pulse2->Draw("same");
    
    fCanvas->cd(5);
    hChargeTub1->Draw();
    
    fCanvas->cd(6);
    hChargeTub2->Draw();
    
    fCanvas->cd(7);
    hChargeCounter1Pulse1->Draw();
    hChargeCounter1Pulse2->Draw("same");
    
    fCanvas->cd(8);
    hChargeCounter2Pulse1->Draw();
    hChargeCounter2Pulse2->Draw("same");
        
    fTempCanvas->cd();
    TH2F *h_nTriggers = (TH2F*)gDirectory->Get("h_nTriggers");
    if (h_nTriggers) delete h_nTriggers;
    fSpillTree->Draw("nTriggers:Entry$>>h_nTriggers");
    fCanvas->cd(9);
    h_nTriggers = (TH2F*)gDirectory->Get("h_nTriggers");
    h_nTriggers->GetXaxis()->SetTitle("Spill #");
    h_nTriggers->SetTitle("Number of Triggers");
    h_nTriggers->Draw();
    
    fTempCanvas->cd();
    TH2F *h_meanCharge_Tub1 = (TH2F*)gDirectory->Get("h_meanCharge_Tub1");
    TH2F *h_meanCharge_Tub2 = (TH2F*)gDirectory->Get("h_meanCharge_Tub2");
    if (h_meanCharge_Tub1) delete h_meanCharge_Tub1;
    if (h_meanCharge_Tub2) delete h_meanCharge_Tub2;
    fSpillTree->Draw("meanCharge_Tub1:Entry$>>h_meanCharge_Tub1");
    fSpillTree->Draw("meanCharge_Tub2:Entry$>>h_meanCharge_Tub2");
    fCanvas->cd(10);
    TH2F *htemp = (TH2F*)gDirectory->Get("htemp");
    if (htemp) delete htemp;
    h_meanCharge_Tub1 = (TH2F*)gDirectory->Get("h_meanCharge_Tub1");
    h_meanCharge_Tub2 = (TH2F*)gDirectory->Get("h_meanCharge_Tub2");
    htemp = new TH2F("htemp", "htemp", 
        g_nSpills, 0, g_nSpills, 
        1000, 0, r_meanCharge_Tub);
    htemp->GetXaxis()->SetTitle("Spill #");
    htemp->SetTitle("Mean Charge of Tub");
    htemp->Draw();
    h_meanCharge_Tub1->SetMarkerColor(kRed);
    h_meanCharge_Tub1->Draw("same");
    h_meanCharge_Tub2->SetMarkerColor(kBlue);
    h_meanCharge_Tub2->Draw("same");
    
    fTempCanvas->cd();
    TH2F *h_meanCharge_Counter1Pulse1 = (TH2F*)gDirectory->Get("h_meanCharge_Counter1Pulse1");
    TH2F *h_meanCharge_Counter1Pulse2 = (TH2F*)gDirectory->Get("h_meanCharge_Counter1Pulse2");
    if (h_meanCharge_Counter1Pulse1) delete h_meanCharge_Counter1Pulse1;
    if (h_meanCharge_Counter1Pulse2) delete h_meanCharge_Counter1Pulse2;
    fSpillTree->Draw("meanCharge_Counter1Pulse1:Entry$>>h_meanCharge_Counter1Pulse1");
    fSpillTree->Draw("meanCharge_Counter1Pulse2:Entry$>>h_meanCharge_Counter1Pulse2");
    fCanvas->cd(11);
    TH2F *htemp2 = (TH2F*)gDirectory->Get("htemp2");
    if (htemp2) delete htemp2;
    h_meanCharge_Counter1Pulse1 = (TH2F*)gDirectory->Get("h_meanCharge_Counter1Pulse1");
    h_meanCharge_Counter1Pulse2 = (TH2F*)gDirectory->Get("h_meanCharge_Counter1Pulse2");
    htemp2 = new TH2F("htemp2", "htemp2", 
        g_nSpills, 0, g_nSpills, 
        1000, 0, r_meanCharge_Counter1);
    htemp2->GetXaxis()->SetTitle("Spill #");
    htemp2->SetTitle("Mean Charge of Counter 1");
    htemp2->Draw();
    h_meanCharge_Counter1Pulse1->SetMarkerColor(kRed);
    h_meanCharge_Counter1Pulse1->Draw("same");
    h_meanCharge_Counter1Pulse2->SetMarkerColor(kBlue);
    h_meanCharge_Counter1Pulse2->Draw("same");
    
    fTempCanvas->cd();
    TH2F *h_meanCharge_Counter2Pulse1 = (TH2F*)gDirectory->Get("h_meanCharge_Counter2Pulse1");
    TH2F *h_meanCharge_Counter2Pulse2 = (TH2F*)gDirectory->Get("h_meanCharge_Counter2Pulse2");
    if (h_meanCharge_Counter2Pulse1) delete h_meanCharge_Counter2Pulse1;
    if (h_meanCharge_Counter2Pulse2) delete h_meanCharge_Counter2Pulse2;
    fSpillTree->Draw("meanCharge_Counter2Pulse1:Entry$>>h_meanCharge_Counter2Pulse1");
    fSpillTree->Draw("meanCharge_Counter2Pulse2:Entry$>>h_meanCharge_Counter2Pulse2");
    fCanvas->cd(12);
    TH2F *htemp3 = (TH2F*)gDirectory->Get("htemp3");
    if (htemp3) delete htemp3;
    h_meanCharge_Counter2Pulse1 = (TH2F*)gDirectory->Get("h_meanCharge_Counter2Pulse1");
    h_meanCharge_Counter2Pulse2 = (TH2F*)gDirectory->Get("h_meanCharge_Counter2Pulse2");
    htemp3 = new TH2F("htemp3", "htemp3", 
        g_nSpills, 0, g_nSpills, 
        1000, 0, r_meanCharge_Counter2);
    htemp3->GetXaxis()->SetTitle("Spill #");
    htemp3->SetTitle("Mean Charge of Counter 2");
    htemp3->Draw();
    h_meanCharge_Counter2Pulse1->SetMarkerColor(kRed);
    h_meanCharge_Counter2Pulse1->Draw("same");
    h_meanCharge_Counter2Pulse2->SetMarkerColor(kBlue);
    h_meanCharge_Counter2Pulse2->Draw("same");
    
    return 1;
}

