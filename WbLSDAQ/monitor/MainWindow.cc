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
#include "TText.h"
#include "TTimeStamp.h"
#include "TLine.h"
#include "TGraph.h"

#include <iostream>
#include <fstream>
using namespace std;

MainWindow::MainWindow(const TGWindow *p, int w,int h)
    : TGMainFrame(p, w, h)
{
    // fIsFakeData = true;
    fIsFakeData = false;
    g_nSpills = 0;
    g_tLastTrigger = -1;
    g_processDone = true;
    SetFixedRanges();
    
    fTimer = new TTimer(this, 3500);
    fTimer->TurnOn();
    
    fText_run_number = new TText(0.3, 0.3, "Unknown");
    fText_run_number->SetNDC();
    fText_run_number->SetTextSize(0.1);
    
    fText_runtype = new TText(0.3, 0.3, "Unknown");
    fText_runtype->SetNDC();
    fText_runtype->SetTextSize(0.1);
    
    fText_sampletype = new TText(0.3, 0.3, "Unknown");
    fText_sampletype->SetNDC();
    fText_sampletype->SetTextSize(0.1);
    
    fText_runstart = new TText(0.3, 0.3, "Unknown");
    fText_runstart->SetNDC();
    fText_runstart->SetTextSize(0.1);
    
    fText_runstop = new TText(0.3, 0.3, "Unknown");
    fText_runstop->SetNDC();
    fText_runstop->SetTextSize(0.1);
    
    // fTimer->SetCommand("gMainWindow->HandleTimer(0)");
    
    hCH0 = new TH1F("hCH0", "Tub 1 (Teflon)", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH1 = new TH1F("hCH1", "Tub 2 (Al)", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH2 = new TH1F("hCH2", "Counter 1 (H1 + H3)", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH3 = new TH1F("hCH3", "Counter 2 (H2 + VC)", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    
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
    
    hDtTriggers = new TH1F("hDtTriggers", "Time Between Triggers", 100, r_dtTrigger_min, r_dtTrigger_max);
    
    h_nTriggers = new TGraph();
    h_meanCharge_Tub1 = new TGraph();
    h_meanCharge_Tub2 = new TGraph();
    h_meanCharge_Counter1Pulse1 = new TGraph();
    h_meanCharge_Counter1Pulse2 = new TGraph();
    h_meanCharge_Counter2Pulse1 = new TGraph();
    h_meanCharge_Counter2Pulse2 = new TGraph();
    
    
    // InitSpillTree();
    InitMaps();
    
    // fTempCanvas = new TCanvas("tempC", "tempC", 100, 100);
    fEcanvas = new TRootEmbeddedCanvas("Ecanvas", this, 1500, 800);
    AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
    fCanvas = fEcanvas->GetCanvas();
    fCanvas->Divide(5, 3);
    
    // initialize window
    fLastFile = GetLastFile();
    
    // fLastFile = "../data/test/rootoutputfile409cosmicsWBLS7percent_shortcable_1315V.root";
    LoadLastSpill();
    SetDynamicRanges();
    SetSignalWindow();
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
    
    delete hChargeTub1;
    delete hChargeTub2;
    delete hChargeCounter1Pulse1;
    delete hChargeCounter1Pulse2;
    delete hChargeCounter2Pulse1;
    delete hChargeCounter2Pulse2;

    delete hTDCTub1;
    delete hTDCTub2;
    delete hTDCCounter1Pulse1;
    delete hTDCCounter1Pulse2;
    delete hTDCCounter2Pulse1;
    delete hTDCCounter2Pulse2;
    
    delete hDtTriggers;
    
    delete h_nTriggers;
    delete h_meanCharge_Tub1;
    delete h_meanCharge_Tub2;
    delete h_meanCharge_Counter1Pulse1;
    delete h_meanCharge_Counter1Pulse2;
    delete h_meanCharge_Counter2Pulse1;
    delete h_meanCharge_Counter2Pulse2;
}

// void MainWindow::InitSpillTree()
// {
//     fSpillTree = new TTree("spillTree", "Stats of Each Spill");
//     fSpillTree->Branch("nTriggers", &s_nTriggers);
//     fSpillTree->Branch("meanCharge_Tub1", &s_meanCharge_Tub1);
//     fSpillTree->Branch("meanCharge_Tub2", &s_meanCharge_Tub2);
//     fSpillTree->Branch("meanCharge_Counter1Pulse1", &s_meanCharge_Counter1Pulse1);
//     fSpillTree->Branch("meanCharge_Counter1Pulse2", &s_meanCharge_Counter1Pulse2);
//     fSpillTree->Branch("meanCharge_Counter2Pulse1", &s_meanCharge_Counter2Pulse1);
//     fSpillTree->Branch("meanCharge_Counter2Pulse2", &s_meanCharge_Counter2Pulse2);
// }

void MainWindow::InitMaps()
{
    fRunTypeMap[0] = "Unknown";
    fRunTypeMap[1] = "Pedestal";
    fRunTypeMap[2] = "Calibration";
    fRunTypeMap[3] = "Cosmic";
    fRunTypeMap[4] = "Stand-alone";
    fRunTypeMap[5] = "Beam Low";
    fRunTypeMap[6] = "Beam Medium";
    fRunTypeMap[7] = "Beam High";
    
    fSampleTypeMap[0] = "Unknown";
    fSampleTypeMap[1] = "Air";
    fSampleTypeMap[2] = "Water";
    fSampleTypeMap[3] = "WbLS 1";
    fSampleTypeMap[4] = "WbLS 2";
    fSampleTypeMap[5] = "LS";
    fSampleTypeMap[6] = "Others";
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

void MainWindow::SetFixedRanges()
{    
    // display ranges of the waveforms
    r_CHymin[0] = 2000; r_CHymax[0] = 8300;
    r_CHymin[1] = 2000; r_CHymax[1] = 8300;
    r_CHymin[2] = 2000; r_CHymax[2] = 8300;
    r_CHymin[3] = 2000; r_CHymax[3] = 8300;
    
    // scaling for pulsetime count (to show on waveform plot)
    r_pulsetime_count_scaling = 200;
    
    // max mean charge of the pmts to show on the history plot
    r_meanCharge_Tub = 80000.;
    r_meanCharge_Counter1 = 100000.;
    r_meanCharge_Counter2 = 500000.;
    
    // display ranges of the log(dT/sec) between triggers
    r_dtTrigger_min = -3;
    r_dtTrigger_max = -2;
}

void MainWindow::SetDynamicRanges()
{
    // only set on the first spill 
    if (g_nSpills!=1) { return; }
    
    // max mean charge of the pmts to show on the history plot
    double scaling = 1.5;
    r_meanCharge_Tub = s_meanCharge_Tub1 > s_meanCharge_Tub2 ? s_meanCharge_Tub1*scaling : s_meanCharge_Tub2*scaling;
    r_meanCharge_Counter1 = s_meanCharge_Counter1Pulse1 > s_meanCharge_Counter1Pulse2 ? s_meanCharge_Counter1Pulse1*scaling : s_meanCharge_Counter1Pulse2*scaling;
    r_meanCharge_Counter2 = s_meanCharge_Counter2Pulse1 > s_meanCharge_Counter2Pulse2 ? s_meanCharge_Counter2Pulse1*scaling : s_meanCharge_Counter2Pulse2*scaling;
    
    // fCHymin[0] = hCH0->GetMinimum(100)*0.8; fCHymax[0] = 8200;
    // fCHymin[1] = hCH1->GetMinimum(100)*0.8; fCHymax[1] = 8200;
    // fCHymin[2] = hCH2->GetMinimum(100)*0.8; fCHymax[2] = 8200;
    // fCHymin[3] = hCH3->GetMinimum(100)*0.8; fCHymax[3] = 8200;
    
}

void MainWindow::SetSignalWindow()
{
    for (int i=0; i<=1; i++) {
        fLine_SigTub[i][0] = new TLine(g_tStartTub[i], r_CHymin[i], g_tStartTub[i], r_CHymax[i]);
        fLine_SigTub[i][1] = new TLine(g_tStopTub[i], r_CHymin[i], g_tStopTub[i], r_CHymax[i]);
        // cout << g_tStartTub[i] << ", " << r_CHymax[i]-1000<< ", " <<  g_tStopTub[i]<< ", " <<  r_CHymax[i] << endl;
        fLine_SigTub[i][0]->SetLineColor(kGreen);
        fLine_SigTub[i][1]->SetLineColor(kGreen);
        for (int j=0; j<=1; j++) {
            fLine_SigCounter[i][j][0] = new TLine(g_tStartCounterPulse[i][j], r_CHymin[i+2], g_tStartCounterPulse[i][j], r_CHymax[i+2]);
            fLine_SigCounter[i][j][1] = new TLine(g_tStopCounterPulse[i][j], r_CHymin[i+2], g_tStopCounterPulse[i][j], r_CHymax[i+2]);
            fLine_SigCounter[i][j][0]->SetLineColor(kGreen);
            fLine_SigCounter[i][j][1]->SetLineColor(kGreen);
        }
    }    
    
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
    
    hDtTriggers->GetXaxis()->SetTitle("log_{10}(#DeltaT_{triggers})");
    
    h_nTriggers->SetTitle("Number of Triggers");
    h_meanCharge_Tub1->SetTitle("Charge of Tub");
    h_meanCharge_Tub2->SetTitle("Charge of Tub");
    h_meanCharge_Counter1Pulse1->SetTitle("Charge of Counter 1");
    h_meanCharge_Counter1Pulse2->SetTitle("Charge of Counter 1");
    h_meanCharge_Counter2Pulse1->SetTitle("Charge of Counter 2");
    h_meanCharge_Counter2Pulse2->SetTitle("Charge of Counter 2");
    
    h_meanCharge_Tub1->SetMarkerColor(kRed);
    h_meanCharge_Tub2->SetMarkerColor(kBlue);
    h_meanCharge_Counter1Pulse1->SetMarkerColor(kRed);
    h_meanCharge_Counter1Pulse2->SetMarkerColor(kBlue);
    h_meanCharge_Counter2Pulse1->SetMarkerColor(kRed);
    h_meanCharge_Counter2Pulse2->SetMarkerColor(kBlue);
    
    
    for (int i=1; i!=5; i++) {
        fCanvas->cd(i);
        gPad->SetGridx();
        gPad->SetGridy();
    }
    for (int i=11; i!=15; i++) {
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
    // for (unsigned bin=0; bin<=100; ++bin) {
    //     hChargeTub1->SetBinContent(bin, 0);
    //     hChargeTub1->SetEntries(0);
    //     hChargeTub2->SetBinContent(bin, 0);
    //     hChargeTub2->SetEntries(0);
    //     
    //     hChargeCounter1Pulse1->SetBinContent(bin, 0);
    //     hChargeCounter1Pulse2->SetBinContent(bin, 0);
    //     hChargeCounter2Pulse1->SetBinContent(bin, 0);
    //     hChargeCounter2Pulse2->SetBinContent(bin, 0);
    // }
    
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
    // spinner.PrintRunInfo();
    WblsDaq::Header* f_header = spinner.header();
    WblsDaq::Footer* f_footer = spinner.footer();
    if (fIsFakeData) { f_run_number = g_nSpills; }
    else { f_run_number = f_header->run_number; }
    f_runtype = f_header->runtype;
    f_sampletype = f_header->sampletype;
    f_runstart = f_header->runstart;
    f_runstop = f_footer->runstop;
    
    g_processDone = false;
    for (int i=0; i < s_nTriggers; ++i) {
        WblsDaq::Event* evt = spinner.event(i);    
        if (!evt) {
            cerr << "Failed to get event at entry #" << i << endl;
            return 2;
        }
        
        if (g_tLastTrigger<0) {
            // g_tLastTrigger = evt->time;
            g_tLastTrigger = evt->time_count;
        }
        else {
            // hDtTriggers->Fill(TMath::Log10(evt->time - g_tLastTrigger));
            // g_tLastTrigger = evt->time;
            // cout << (evt->time_count - g_tLastTrigger)/f_header->perform_freq << endl;
            
            hDtTriggers->Fill(TMath::Log10( (evt->time_count - g_tLastTrigger)/f_header->perform_freq) );
            g_tLastTrigger = evt->time_count;
            // cout << g_tLastTrigger << endl;
            
        }
        for (unsigned bin=0; bin<WblsDaq::NFADCBins; ++bin) {
            hCH0->SetBinContent(bin, evt->ch0[bin] + hCH0->GetBinContent(bin));
            hCH1->SetBinContent(bin, evt->ch1[bin] + hCH1->GetBinContent(bin));
            hCH2->SetBinContent(bin, evt->ch2[bin] + hCH2->GetBinContent(bin));
            hCH3->SetBinContent(bin, evt->ch3[bin] + hCH3->GetBinContent(bin));
        }
        
        WFAnalyzer wf(evt);
        // get time window of each signal (same of every spill)
        if(g_nSpills==1) {
            for (int m=0; m<=1; m++) {
                g_tStartTub[m] = wf.g_tStartTub[m];
                g_tStopTub[m] = wf.g_tStopTub[m];
                for (int n=0; n<=1; n++) {
                    g_tStartCounterPulse[m][n] = wf.g_tStartCounterPulse[m][n];
                    g_tStopCounterPulse[m][n] = wf.g_tStopCounterPulse[m][n];
                }
            }
        }
        // wf analysis
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
    
    h_nTriggers->SetPoint(h_nTriggers->GetN(), f_run_number, s_nTriggers);
    h_meanCharge_Tub1->SetPoint(h_meanCharge_Tub1->GetN(), f_run_number, s_meanCharge_Tub1);
    h_meanCharge_Tub2->SetPoint(h_meanCharge_Tub2->GetN(), f_run_number, s_meanCharge_Tub2);
    h_meanCharge_Counter1Pulse1->SetPoint(h_meanCharge_Counter1Pulse1->GetN(), f_run_number, s_meanCharge_Counter1Pulse1);
    h_meanCharge_Counter1Pulse2->SetPoint(h_meanCharge_Counter1Pulse2->GetN(), f_run_number, s_meanCharge_Counter1Pulse2);
    h_meanCharge_Counter2Pulse1->SetPoint(h_meanCharge_Counter2Pulse1->GetN(), f_run_number, s_meanCharge_Counter2Pulse1);
    h_meanCharge_Counter2Pulse2->SetPoint(h_meanCharge_Counter2Pulse2->GetN(), f_run_number, s_meanCharge_Counter2Pulse2);
    // cout << s_meanCharge_Counter1Pulse1 << ", " << s_meanCharge_Counter1Pulse2 << endl;
    // fSpillTree->Fill();
    g_processDone = true;
    
    return 1;
}

int MainWindow::Update()
{
    // do not update if last file has not finished processing
    if (!g_processDone) return 0;
    
    string lastfile = GetLastFile();
    if (lastfile == fLastFile) {
        // nothing to Update
        // cout << "nothing to update: " << lastfile << endl;
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
    // system("python monitor.py /Users/chaozhang/Projects/LBNE/WbLS/software/WbLSDAQ/data/test");
    ifstream in("lastfile.txt");
    string lastfile;
    in >> lastfile;
    in.close();
    if (lastfile == "") {
        lastfile = fLastFile;
        cout << "wait for next updating" << endl;
    }
    return lastfile;
}

int MainWindow::DoDraw()
{
    for (unsigned bin=0; bin<=WblsDaq::NFADCBins; ++bin) {
        hTDCTub1->SetBinContent(bin, r_CHymin[0] + hTDCTub1->GetBinContent(bin)*r_pulsetime_count_scaling);
        hTDCTub2->SetBinContent(bin, r_CHymin[1] + hTDCTub2->GetBinContent(bin)*r_pulsetime_count_scaling);
        hTDCCounter1Pulse1->SetBinContent(bin, r_CHymin[2] + hTDCCounter1Pulse1->GetBinContent(bin)*r_pulsetime_count_scaling);
        hTDCCounter1Pulse2->SetBinContent(bin, r_CHymin[2] + hTDCCounter1Pulse2->GetBinContent(bin)*r_pulsetime_count_scaling);
        hTDCCounter2Pulse1->SetBinContent(bin, r_CHymin[3] + hTDCCounter2Pulse1->GetBinContent(bin)*r_pulsetime_count_scaling);
        hTDCCounter2Pulse2->SetBinContent(bin, r_CHymin[3] + hTDCCounter2Pulse2->GetBinContent(bin)*r_pulsetime_count_scaling);
    }
    
    fCanvas->cd(1);
    // hCH0->GetXaxis()->SetRangeUser(500, 2000);
    hCH0->GetYaxis()->SetRangeUser(r_CHymin[0], r_CHymax[0]);
    hCH0->Draw();
    hTDCTub1->Draw("same");
    fLine_SigTub[0][0]->Draw();
    fLine_SigTub[0][1]->Draw();
    
    fCanvas->cd(2);
    // hCH1->GetXaxis()->SetRangeUser(500, 2000);
    hCH1->GetYaxis()->SetRangeUser(r_CHymin[1], r_CHymax[1]);
    hCH1->Draw();
    hTDCTub2->Draw("same");
    fLine_SigTub[1][0]->Draw();
    fLine_SigTub[1][1]->Draw();
    
    fCanvas->cd(3);
    // hCH2->GetXaxis()->SetRangeUser(500, 2000);
    hCH2->GetYaxis()->SetRangeUser(r_CHymin[2], r_CHymax[2]);
    hCH2->Draw();
    hTDCCounter1Pulse1->Draw("same");
    hTDCCounter1Pulse2->Draw("same");
    fLine_SigCounter[0][0][0]->Draw();
    fLine_SigCounter[0][0][1]->Draw();
    fLine_SigCounter[0][1][0]->Draw();
    fLine_SigCounter[0][1][1]->Draw();
    
    fCanvas->cd(4);
    // hCH3->GetXaxis()->SetRangeUser(500, 2000);
    hCH3->GetYaxis()->SetRangeUser(r_CHymin[3], r_CHymax[3]);
    hCH3->Draw();
    hTDCCounter2Pulse1->Draw("same");
    hTDCCounter2Pulse2->Draw("same");
    fLine_SigCounter[1][0][0]->Draw();
    fLine_SigCounter[1][0][1]->Draw();
    fLine_SigCounter[1][1][0]->Draw();
    fLine_SigCounter[1][1][1]->Draw();
    
    fCanvas->cd(5);
    TString s = Form("Run Number: %i", f_run_number);
    fText_run_number->SetText(0.02, 0.8, s.Data());
    fText_run_number->Draw();
    TTimeStamp ts(f_runstart-4*3600);
    fText_runstart->SetText(0.02, 0.6, ts.AsString("s"));
    fText_runstart->Draw();    
    s.Form("Run Type: %s", fRunTypeMap[f_runtype].c_str());
    fText_runtype->SetText(0.02, 0.4, s.Data());
    fText_runtype->Draw();
    s.Form("Sample Type: %s", fSampleTypeMap[f_sampletype].c_str());
    fText_sampletype->SetText(0.02, 0.2, s.Data());
    fText_sampletype->Draw();
    
    fCanvas->cd(6);
    hChargeTub1->Draw();
    
    fCanvas->cd(7);
    hChargeTub2->Draw();
    
    fCanvas->cd(8);
    hChargeCounter1Pulse1->Draw();
    hChargeCounter1Pulse2->Draw("same");
    
    fCanvas->cd(9);
    hChargeCounter2Pulse1->Draw();
    hChargeCounter2Pulse2->Draw("same");
    
    fCanvas->cd(10);
    hDtTriggers->Draw();
    
    fCanvas->cd(11);
    h_nTriggers->GetXaxis()->SetTitle("Run Number");
    h_nTriggers->Draw("AP");    
    
    fCanvas->cd(12);
    h_meanCharge_Tub1->GetXaxis()->SetTitle("Run Number");
    h_meanCharge_Tub1->Draw("AP");
    h_meanCharge_Tub1->GetYaxis()->SetRangeUser(0, r_meanCharge_Tub);
    h_meanCharge_Tub2->Draw("P,same");
    
    fCanvas->cd(13);
    h_meanCharge_Counter1Pulse1->GetXaxis()->SetTitle("Run Number");
    h_meanCharge_Counter1Pulse1->Draw("AP");
    h_meanCharge_Counter1Pulse1->GetYaxis()->SetRangeUser(0, r_meanCharge_Counter1);
    h_meanCharge_Counter1Pulse2->Draw("P,same");
    
    fCanvas->cd(14);
    h_meanCharge_Counter2Pulse1->GetXaxis()->SetTitle("Run Number");
    h_meanCharge_Counter2Pulse1->Draw("AP");
    h_meanCharge_Counter2Pulse1->GetYaxis()->SetRangeUser(0, r_meanCharge_Counter2);
    // cout << r_meanCharge_Counter2 << endl;
    h_meanCharge_Counter2Pulse2->Draw("P,same");
    
    // fTempCanvas->cd();
    // TH2F *h_nTriggers = (TH2F*)gDirectory->Get("h_nTriggers");
    // if (h_nTriggers) delete h_nTriggers;
    // fSpillTree->Draw("nTriggers:Entry$>>h_nTriggers");
    // fCanvas->cd(11);
    // h_nTriggers = (TH2F*)gDirectory->Get("h_nTriggers");
    // h_nTriggers->GetXaxis()->SetTitle("Spill #");
    // h_nTriggers->SetTitle("Number of Triggers");
    // h_nTriggers->Draw();
    // 
    // fTempCanvas->cd();
    // TH2F *h_meanCharge_Tub1 = (TH2F*)gDirectory->Get("h_meanCharge_Tub1");
    // TH2F *h_meanCharge_Tub2 = (TH2F*)gDirectory->Get("h_meanCharge_Tub2");
    // if (h_meanCharge_Tub1) delete h_meanCharge_Tub1;
    // if (h_meanCharge_Tub2) delete h_meanCharge_Tub2;
    // fSpillTree->Draw("meanCharge_Tub1:Entry$>>h_meanCharge_Tub1");
    // fSpillTree->Draw("meanCharge_Tub2:Entry$>>h_meanCharge_Tub2");
    // fCanvas->cd(12);
    // TH2F *htemp = (TH2F*)gDirectory->Get("htemp");
    // if (htemp) delete htemp;
    // h_meanCharge_Tub1 = (TH2F*)gDirectory->Get("h_meanCharge_Tub1");
    // h_meanCharge_Tub2 = (TH2F*)gDirectory->Get("h_meanCharge_Tub2");
    // htemp = new TH2F("htemp", "htemp", 
    //     g_nSpills, 0, g_nSpills, 
    //     1000, 0, r_meanCharge_Tub);
    // htemp->GetXaxis()->SetTitle("Spill #");
    // htemp->SetTitle("Mean Charge of Tub");
    // htemp->Draw();
    // h_meanCharge_Tub1->SetMarkerColor(kRed);
    // h_meanCharge_Tub1->Draw("same");
    // h_meanCharge_Tub2->SetMarkerColor(kBlue);
    // h_meanCharge_Tub2->Draw("same");
    // 
    // fTempCanvas->cd();
    // TH2F *h_meanCharge_Counter1Pulse1 = (TH2F*)gDirectory->Get("h_meanCharge_Counter1Pulse1");
    // TH2F *h_meanCharge_Counter1Pulse2 = (TH2F*)gDirectory->Get("h_meanCharge_Counter1Pulse2");
    // if (h_meanCharge_Counter1Pulse1) delete h_meanCharge_Counter1Pulse1;
    // if (h_meanCharge_Counter1Pulse2) delete h_meanCharge_Counter1Pulse2;
    // fSpillTree->Draw("meanCharge_Counter1Pulse1:Entry$>>h_meanCharge_Counter1Pulse1");
    // fSpillTree->Draw("meanCharge_Counter1Pulse2:Entry$>>h_meanCharge_Counter1Pulse2");
    // fCanvas->cd(13);
    // TH2F *htemp2 = (TH2F*)gDirectory->Get("htemp2");
    // if (htemp2) delete htemp2;
    // h_meanCharge_Counter1Pulse1 = (TH2F*)gDirectory->Get("h_meanCharge_Counter1Pulse1");
    // h_meanCharge_Counter1Pulse2 = (TH2F*)gDirectory->Get("h_meanCharge_Counter1Pulse2");
    // htemp2 = new TH2F("htemp2", "htemp2", 
    //     g_nSpills, 0, g_nSpills, 
    //     1000, 0, r_meanCharge_Counter1);
    // htemp2->GetXaxis()->SetTitle("Spill #");
    // htemp2->SetTitle("Mean Charge of Counter 1");
    // htemp2->Draw();
    // h_meanCharge_Counter1Pulse1->SetMarkerColor(kRed);
    // h_meanCharge_Counter1Pulse1->Draw("same");
    // h_meanCharge_Counter1Pulse2->SetMarkerColor(kBlue);
    // h_meanCharge_Counter1Pulse2->Draw("same");
    // 
    // fTempCanvas->cd();
    // TH2F *h_meanCharge_Counter2Pulse1 = (TH2F*)gDirectory->Get("h_meanCharge_Counter2Pulse1");
    // TH2F *h_meanCharge_Counter2Pulse2 = (TH2F*)gDirectory->Get("h_meanCharge_Counter2Pulse2");
    // if (h_meanCharge_Counter2Pulse1) delete h_meanCharge_Counter2Pulse1;
    // if (h_meanCharge_Counter2Pulse2) delete h_meanCharge_Counter2Pulse2;
    // fSpillTree->Draw("meanCharge_Counter2Pulse1:Entry$>>h_meanCharge_Counter2Pulse1");
    // fSpillTree->Draw("meanCharge_Counter2Pulse2:Entry$>>h_meanCharge_Counter2Pulse2");
    // fCanvas->cd(14);
    // TH2F *htemp3 = (TH2F*)gDirectory->Get("htemp3");
    // if (htemp3) delete htemp3;
    // h_meanCharge_Counter2Pulse1 = (TH2F*)gDirectory->Get("h_meanCharge_Counter2Pulse1");
    // h_meanCharge_Counter2Pulse2 = (TH2F*)gDirectory->Get("h_meanCharge_Counter2Pulse2");
    // htemp3 = new TH2F("htemp3", "htemp3", 
    //     g_nSpills, 0, g_nSpills, 
    //     1000, 0, r_meanCharge_Counter2);
    // htemp3->GetXaxis()->SetTitle("Spill #");
    // htemp3->SetTitle("Mean Charge of Counter 2");
    // htemp3->Draw();
    // h_meanCharge_Counter2Pulse1->SetMarkerColor(kRed);
    // h_meanCharge_Counter2Pulse1->Draw("same");
    // h_meanCharge_Counter2Pulse2->SetMarkerColor(kBlue);
    // h_meanCharge_Counter2Pulse2->Draw("same");
    
    return 1;
}

