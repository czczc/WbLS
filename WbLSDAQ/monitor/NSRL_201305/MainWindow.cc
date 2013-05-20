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
    
    hCH0 = new TH1F("hCH0", "(Beam) PMT A (Magic Box)",    WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH1 = new TH1F("hCH1", "(Beam) PMT B (Magic Box)",    WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH2 = new TH1F("hCH2", "(Beam) Counter 1 (H1 + LED)", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH3 = new TH1F("hCH3", "(Beam) Counter 2 (H2 + VC)",  WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH0->GetXaxis()->SetRangeUser(200, 1500);
    hCH1->GetXaxis()->SetRangeUser(200, 1500);
    hCH2->GetXaxis()->SetRangeUser(200, 1500);
    hCH3->GetXaxis()->SetRangeUser(200, 1500);
    
    hCH0LED = new TH1F("hCH0LED", "(LED) PMT A (Magic Box)",    WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH1LED = new TH1F("hCH1LED", "(LED) PMT B (Magic Box)",    WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH2LED = new TH1F("hCH2LED", "(LED) Counter 1 (H1 + LED)", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH3LED = new TH1F("hCH3LED", "(LED) Counter 2 (H2 + VC)",  WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    hCH0LED->GetXaxis()->SetRangeUser(200, 1500);
    hCH1LED->GetXaxis()->SetRangeUser(200, 1500);
    hCH2LED->GetXaxis()->SetRangeUser(200, 1500);
    hCH3LED->GetXaxis()->SetRangeUser(200, 1500);
    hCH0LED->GetYaxis()->SetRangeUser(8120, 8160);
    hCH1LED->GetYaxis()->SetRangeUser(8160, 8200);
    
    
    hChargeTub1 = new TH1F("hChargeTub1", "Charge of Beam A/B", 100, 1, 7);
    hChargeTub2 = new TH1F("hChargeTub2", "Charge of Beam A/B", 100, 1, 7);
    hChargeLED1 = new TH1F("hChargeLED1", "Charge of LED A/B", 100, 1, 7);
    hChargeLED2 = new TH1F("hChargeLED2", "Charge of LED A/B", 100, 1, 7);
    
    hPeakTub1 = new TH1F("hPeakTub1", "Peak of Beam A/B", 500, 0, 5000);
    hPeakTub2 = new TH1F("hPeakTub2", "Peak of Beam A/B", 500, 0, 5000);
    hPeakLED1 = new TH1F("hPeakLED1", "Peak of LED A/B", 100, 0, 100);
    hPeakLED2 = new TH1F("hPeakLED2", "Peak of LED A/B", 100, 0, 100);
    
    hPeakCounter1Pulse1 = new TH1F("hPeakCounter1Pulse1", "Peak of Counter 1", 700, 0, 7000);
    hPeakCounter1Pulse2 = new TH1F("hPeakCounter1Pulse2", "Peak of Counter 1", 700, 0, 7000);
    hPeakCounter2Pulse1 = new TH1F("hPeakCounter2Pulse1", "Peak of Counter 2", 700, 0, 7000);
    hPeakCounter2Pulse2 = new TH1F("hPeakCounter2Pulse2", "Peak of Counter 2", 700, 0, 7000);
    
    h2Tub1Tub2 =  new TH2F("h2Tub1Tub2", "MB vs MA", 700, 0, 7000, 700, 0, 7000);
    h2H1H2 =  new TH2F("h2H1H2", "H2 vs H1", 700, 0, 7000, 700, 0, 7000);
    h2MAH1 =  new TH2F("h2MAH1", "H1 vs MA", 700, 0, 7000, 700, 0, 7000);
    h2MBH2 =  new TH2F("h2MBH2", "H2 vs MB", 700, 0, 7000, 700, 0, 7000);
    
    // hTDCTub1 = new TH1F("hTDCTub1", "Pulse Time of Tub 1", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    // hTDCTub2 = new TH1F("hTDCTub2", "Pulse Time of Tub 2", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    // hTDCCounter1Pulse1 = new TH1F("hTDCCounter1Pulse1", "", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    // hTDCCounter1Pulse2 = new TH1F("hTDCCounter1Pulse2", "", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    // hTDCCounter2Pulse1 = new TH1F("hTDCCounter2Pulse1", "", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    // hTDCCounter2Pulse2 = new TH1F("hTDCCounter2Pulse2", "", WblsDaq::NFADCBins, 0, WblsDaq::NFADCBins);
    
    // hDtTriggers = new TH1F("hDtTriggers", "Time Between Triggers", 100, r_dtTrigger_min, r_dtTrigger_max);
    hDtTriggers = new TH1F("hDtTriggers", "Time Between Triggers", 1000, 0, 20);
    
    h_nTriggersBeam = new TGraph();
    h_nTriggersLED = new TGraph();
    h_meanCharge_Tub1 = new TGraph();
    h_meanCharge_Tub2 = new TGraph();
    h_meanCharge_LED1 = new TGraph();
    h_meanCharge_LED2 = new TGraph();
    
    h_meanPeak_Tub1 = new TGraph();
    h_meanPeak_Tub2 = new TGraph();
    h_meanPeak_LED1 = new TGraph();
    h_meanPeak_LED2 = new TGraph();
    
    h_meanPeak_Counter1Pulse1 = new TGraph();
    h_meanPeak_Counter1Pulse2 = new TGraph();
    h_meanPeak_Counter2Pulse1 = new TGraph();
    h_meanPeak_Counter2Pulse2 = new TGraph();
    
    
    // InitSpillTree();
    InitMaps();
    
    fEcanvas = new TRootEmbeddedCanvas("Ecanvas", this, 1500, 800);
    AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
    fCanvas = fEcanvas->GetCanvas();
    fCanvas->Divide(5, 3);
    
    fCanvas2 = new TCanvas("fCanvas2", "Additional Info", 1500, 800);
    fCanvas2->Divide(5, 3);
    
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
    delete hChargeLED1;
    delete hChargeLED2;
    delete hPeakTub1;
    delete hPeakTub2;
    delete hPeakLED1;
    delete hPeakLED2;
    delete hPeakCounter1Pulse1;
    delete hPeakCounter1Pulse2;
    delete hPeakCounter2Pulse1;
    delete hPeakCounter2Pulse2;

    // delete hTDCTub1;
    // delete hTDCTub2;
    // delete hTDCCounter1Pulse1;
    // delete hTDCCounter1Pulse2;
    // delete hTDCCounter2Pulse1;
    // delete hTDCCounter2Pulse2;
    
    delete hDtTriggers;
    
    delete h_nTriggersBeam;
    delete h_nTriggersLED;
    delete h_meanCharge_Tub1;
    delete h_meanCharge_Tub2;
    delete h_meanCharge_LED2;
    delete h_meanPeak_Counter1Pulse1;
    delete h_meanPeak_Counter1Pulse2;
    delete h_meanPeak_Counter2Pulse1;
    delete h_meanPeak_Counter2Pulse2;
}

// void MainWindow::InitSpillTree()
// {
//     fSpillTree = new TTree("spillTree", "Stats of Each Spill");
//     fSpillTree->Branch("nTriggers", &s_nTiggersBeam);
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
    
    fCanvas2->Modified();
    fCanvas2->Update();
    
    // gSystem->ProcessEvents();
    fTimer->Reset();
    // cout << "updating ..." << endl;
    return kTRUE;
}

void MainWindow::SetFixedRanges()
{    
    // display ranges of the waveforms
    r_CHymin[0] = 6000; r_CHymax[0] = 8300;
    r_CHymin[1] = 6000; r_CHymax[1] = 8300;
    r_CHymin[2] = 6000; r_CHymax[2] = 8300;
    r_CHymin[3] = 6000; r_CHymax[3] = 8300;
    
    // scaling for pulsetime count (to show on waveform plot)
    r_pulsetime_count_scaling = 200;
    
    // max mean charge of the pmts to show on the history plot
    r_meanCharge_Tub = 80000.;
    r_meanPeak_Counter1 = 1000.;
    r_meanPeak_Counter2 = 5000.;
    
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
    r_meanCharge_LED = s_meanCharge_LED1 > s_meanCharge_LED2 ? s_meanCharge_LED1*scaling : s_meanCharge_LED2*scaling;

    r_meanPeak_Tub = s_meanPeak_Tub1 > s_meanPeak_Tub2 ? s_meanPeak_Tub1*scaling : s_meanPeak_Tub2*scaling;
    r_meanPeak_LED = s_meanPeak_LED1 > s_meanPeak_LED2 ? s_meanPeak_LED1*scaling : s_meanPeak_LED2*scaling;

    r_meanPeak_Counter1 = s_meanPeak_Counter1Pulse1 > s_meanPeak_Counter1Pulse2 ? s_meanPeak_Counter1Pulse1*scaling : s_meanPeak_Counter1Pulse2*scaling;
    r_meanPeak_Counter2 = s_meanPeak_Counter2Pulse1 > s_meanPeak_Counter2Pulse2 ? s_meanPeak_Counter2Pulse1*scaling : s_meanPeak_Counter2Pulse2*scaling;
    
    // fCHymin[0] = hCH0->GetMinimum(100)*0.8; fCHymax[0] = 8200;
    // fCHymin[1] = hCH1->GetMinimum(100)*0.8; fCHymax[1] = 8200;
    // fCHymin[2] = hCH2->GetMinimum(100)*0.8; fCHymax[2] = 8200;
    // fCHymin[3] = hCH3->GetMinimum(100)*0.8; fCHymax[3] = 8200;
    
}

void MainWindow::SetSignalWindow()
{
    for (int i=0; i<=1; i++) {
        fLine_SigTub[i][0] = new TLine(g_tStartTub[i], r_CHymax[i]-200, g_tStartTub[i], r_CHymax[i]);
        fLine_SigTub[i][1] = new TLine(g_tStopTub[i], r_CHymax[i]-200, g_tStopTub[i], r_CHymax[i]);
        fLine_SigTub[i][0]->SetLineColor(kGreen);
        fLine_SigTub[i][1]->SetLineColor(kGreen);
        
        fLine_SigLED[i][0] = new TLine(g_tStartLED[i], r_CHymax[i]-200, g_tStartLED[i], r_CHymax[i]);
        fLine_SigLED[i][1] = new TLine(g_tStopLED[i], r_CHymax[i]-200, g_tStopLED[i], r_CHymax[i]);
        // cout << g_tStartLED[i] << ", " <<  g_tStopLED[i] << endl;
        
        fLine_SigLED[i][0]->SetLineColor(kGreen);
        fLine_SigLED[i][1]->SetLineColor(kGreen);
        for (int j=0; j<=1; j++) {
            fLine_SigCounter[i][j][0] = new TLine(g_tStartCounterPulse[i][j], r_CHymax[i+2]-200, g_tStartCounterPulse[i][j], r_CHymax[i+2]);
            fLine_SigCounter[i][j][1] = new TLine(g_tStopCounterPulse[i][j], r_CHymax[i+2]-200, g_tStopCounterPulse[i][j], r_CHymax[i+2]);
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
    
    hChargeLED1->GetXaxis()->SetTitle("log_{10}(Charge)");
    hChargeLED2->GetXaxis()->SetTitle("log_{10}(Charge)");
    
    hPeakTub1->GetXaxis()->SetTitle("Peak Height");
    hPeakTub2->GetXaxis()->SetTitle("Peak Height");
    
    hPeakLED1->GetXaxis()->SetTitle("SPE Peak Height");
    hPeakLED2->GetXaxis()->SetTitle("SPE Peak Height");
    
    hPeakCounter1Pulse1->GetXaxis()->SetTitle("Peak Height");
    hPeakCounter1Pulse2->GetXaxis()->SetTitle("Peak Height");
    hPeakCounter2Pulse1->GetXaxis()->SetTitle("Peak Height");
    hPeakCounter2Pulse2->GetXaxis()->SetTitle("Peak Height");
    
    hChargeTub1->SetLineColor(kRed);
    hChargeTub2->SetLineColor(kBlue);
    hChargeLED1->SetLineColor(kRed);
    hChargeLED2->SetLineColor(kBlue);
    
    hPeakTub1->SetLineColor(kRed);
    hPeakTub2->SetLineColor(kBlue);
    hPeakLED1->SetLineColor(kRed);
    hPeakLED2->SetLineColor(kBlue);
    
    hPeakCounter1Pulse1->SetLineColor(kRed);
    hPeakCounter1Pulse2->SetLineColor(kBlue);
    hPeakCounter2Pulse1->SetLineColor(kRed);
    hPeakCounter2Pulse2->SetLineColor(kBlue);
    
    // hTDCTub1->SetLineColor(kRed);
    // hTDCTub2->SetLineColor(kBlue);
    // hTDCCounter1Pulse1->SetLineColor(kRed);
    // hTDCCounter1Pulse2->SetLineColor(kBlue); 
    // hTDCCounter2Pulse1->SetLineColor(kRed); 
    // hTDCCounter2Pulse2->SetLineColor(kBlue); 
    
    hDtTriggers->GetXaxis()->SetTitle("#DeltaT_{triggers} [#mus]");
    
    h_nTriggersBeam->SetTitle("Number of Triggers");
    h_nTriggersLED->SetTitle("Number of Triggers");
    h_meanCharge_Tub1->SetTitle("Mean Charge of Beam A/B");
    h_meanCharge_Tub2->SetTitle("Mean Charge of Beam A/B");
    h_meanCharge_LED1->SetTitle("Mean Charge of LED A/B");
    h_meanCharge_LED2->SetTitle("Mean Charge of LED A/B");
    
    h_meanPeak_Tub1->SetTitle("Mean Peak of Beam A/B");
    h_meanPeak_Tub2->SetTitle("Mean Peak of Beam A/B");
    h_meanPeak_LED1->SetTitle("Mean Peak of LED A/B");
    h_meanPeak_LED2->SetTitle("Mean Peak of LED A/B");
    
    h_meanPeak_Counter1Pulse1->SetTitle("Mean Peak of Counter 1");
    h_meanPeak_Counter1Pulse2->SetTitle("Mean Peak of Counter 1");
    h_meanPeak_Counter2Pulse1->SetTitle("Mean Peak of Counter 2");
    h_meanPeak_Counter2Pulse2->SetTitle("Mean Peak of Counter 2");
    
    h2Tub1Tub2->GetXaxis()->SetTitle("MA Peak Height");
    h2Tub1Tub2->GetYaxis()->SetTitle("MB Peak Height");
    h2H1H2->GetXaxis()->SetTitle("H1 Peak Height");
    h2H1H2->GetYaxis()->SetTitle("H2 Peak Height");
    h2MAH1->GetXaxis()->SetTitle("MA Peak Height");
    h2MAH1->GetYaxis()->SetTitle("H1 Peak Height");
    h2MBH2->GetXaxis()->SetTitle("MB Peak Height");
    h2MBH2->GetYaxis()->SetTitle("H2 Peak Height");
    
    h_meanCharge_Tub1->SetMarkerColor(kRed);
    h_meanCharge_Tub2->SetMarkerColor(kBlue);
    h_meanCharge_LED1->SetMarkerColor(kRed);
    h_meanCharge_LED2->SetMarkerColor(kBlue);
    h_meanPeak_Tub1->SetMarkerColor(kRed);
    h_meanPeak_Tub2->SetMarkerColor(kBlue);
    h_meanPeak_LED1->SetMarkerColor(kRed);
    h_meanPeak_LED2->SetMarkerColor(kBlue);
    h_meanPeak_Counter1Pulse1->SetMarkerColor(kRed);
    h_meanPeak_Counter1Pulse2->SetMarkerColor(kBlue);
    h_meanPeak_Counter2Pulse1->SetMarkerColor(kRed);
    h_meanPeak_Counter2Pulse2->SetMarkerColor(kBlue);
    h_nTriggersBeam->SetMarkerColor(kRed);
    h_nTriggersLED->SetMarkerColor(kBlue);
    
    for (int i=1; i!=5; i++) {
        fCanvas->cd(i);
        gPad->SetGridx();
        gPad->SetGridy();        
    }
    for (int i=11; i!=12; i++) {
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
        hCH0LED->SetBinContent(bin, 0);
        hCH1LED->SetBinContent(bin, 0);
        hCH2LED->SetBinContent(bin, 0);
        hCH3LED->SetBinContent(bin, 0);
        // hTDCTub1->SetBinContent(bin, 0);
        // hTDCTub2->SetBinContent(bin, 0);
        // hTDCCounter1Pulse1->SetBinContent(bin, 0);
        // hTDCCounter1Pulse2->SetBinContent(bin, 0);
        // hTDCCounter2Pulse1->SetBinContent(bin, 0);
        // hTDCCounter2Pulse2->SetBinContent(bin, 0);
    }
    
    s_meanCharge_Tub1 = 0;
    s_meanCharge_Tub2 = 0;
    s_meanCharge_LED1 = 0;
    s_meanCharge_LED2 = 0;
    s_meanPeak_Tub1 = 0;
    s_meanPeak_Tub2 = 0;
    s_meanPeak_LED1 = 0;
    s_meanPeak_LED2 = 0;
    s_meanPeak_Counter1Pulse1 = 0;
    s_meanPeak_Counter1Pulse2 = 0;
    s_meanPeak_Counter2Pulse1 = 0;
    s_meanPeak_Counter2Pulse2 = 0;
}

int MainWindow::LoadLastSpill()
{
    Reset();
    g_nSpills++;
    
    WblsDaq::Spinner spinner(fLastFile);
    int nentries = spinner.entries();
    
    s_nTriggersBeam = 0;
    s_nTriggersLED = 0;
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
    for (int i=0; i < nentries; ++i) {
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
            
            hDtTriggers->Fill(((evt->time_count - g_tLastTrigger)/f_header->perform_freq)*1e3 );
            g_tLastTrigger = evt->time_count;
            // cout << g_tLastTrigger << endl;
            
        }

        WFAnalyzer wf(evt);
        // get time window of each signal (same of every spill)
        if(g_nSpills==1) {
            for (int m=0; m<=1; m++) {
                g_tStartTub[m] = wf.g_tStartTub[m];
                g_tStopTub[m] = wf.g_tStopTub[m];
                g_tStartLED[m] = wf.g_tStartLED[m];
                g_tStopLED[m] = wf.g_tStopLED[m];
                for (int n=0; n<=1; n++) {
                    g_tStartCounterPulse[m][n] = wf.g_tStartCounterPulse[m][n];
                    g_tStopCounterPulse[m][n] = wf.g_tStopCounterPulse[m][n];
                }
            }
        }
        // wf analysis
        wf.Process();
        
        if (wf.isLED) { // LED trigger
            s_nTriggersLED++;
            for (unsigned bin=0; bin<WblsDaq::NFADCBins; ++bin) {
                hCH0LED->SetBinContent(bin, evt->ch0[bin] + hCH0LED->GetBinContent(bin));
                hCH1LED->SetBinContent(bin, evt->ch1[bin] + hCH1LED->GetBinContent(bin));
                hCH2LED->SetBinContent(bin, evt->ch2[bin] + hCH2LED->GetBinContent(bin));
                hCH3LED->SetBinContent(bin, evt->ch3[bin] + hCH3LED->GetBinContent(bin));
            }
            hChargeLED1->Fill(TMath::Log10(wf.fCharge_LED[0]));
            hChargeLED2->Fill(TMath::Log10(wf.fCharge_LED[1]));

            hPeakLED1->Fill(wf.fPeak_LED[0]);
            hPeakLED2->Fill(wf.fPeak_LED[1]);
            
            s_meanCharge_LED1 += wf.fCharge_LED[0];
            s_meanCharge_LED2 += wf.fCharge_LED[1];

            s_meanPeak_LED1 += wf.fPeak_LED[0];
            s_meanPeak_LED2 += wf.fPeak_LED[1];
            
            hPeakCounter1Pulse2->Fill(wf.fPeak_Counter[0][1]);
            // cout << wf.fPeak_Counter[0][1] << endl;
            s_meanPeak_Counter1Pulse2 += wf.fPeak_Counter[0][1];
            
        }
        else {  // Beam trigger
            s_nTriggersBeam++;
            for (unsigned bin=0; bin<WblsDaq::NFADCBins; ++bin) {
                hCH0->SetBinContent(bin, evt->ch0[bin] + hCH0->GetBinContent(bin));
                hCH1->SetBinContent(bin, evt->ch1[bin] + hCH1->GetBinContent(bin));
                hCH2->SetBinContent(bin, evt->ch2[bin] + hCH2->GetBinContent(bin));
                hCH3->SetBinContent(bin, evt->ch3[bin] + hCH3->GetBinContent(bin));
            }
            hChargeTub1->Fill(TMath::Log10(wf.fCharge_Tub[0]));
            hChargeTub2->Fill(TMath::Log10(wf.fCharge_Tub[1]));
            hPeakTub1->Fill(wf.fPeak_Tub[0]);
            hPeakTub2->Fill(wf.fPeak_Tub[1]);
            s_meanCharge_Tub1 += wf.fCharge_Tub[0];
            s_meanCharge_Tub2 += wf.fCharge_Tub[1];
            s_meanPeak_Tub1 += wf.fPeak_Tub[0];
            s_meanPeak_Tub2 += wf.fPeak_Tub[1];
            
            hPeakCounter1Pulse1->Fill(wf.fPeak_Counter[0][0]);
            hPeakCounter2Pulse1->Fill(wf.fPeak_Counter[1][0]);
            hPeakCounter2Pulse2->Fill(wf.fPeak_Counter[1][1]);
            
            s_meanPeak_Counter1Pulse1 += wf.fPeak_Counter[0][0];
            s_meanPeak_Counter2Pulse1 += wf.fPeak_Counter[1][0];
            s_meanPeak_Counter2Pulse2 += wf.fPeak_Counter[1][1];
            
            h2Tub1Tub2->Fill(wf.fPeak_Tub[0], wf.fPeak_Tub[1]);
            h2H1H2->Fill(wf.fPeak_Counter[0][0], wf.fPeak_Counter[1][0]);
            h2MAH1->Fill(wf.fPeak_Tub[0], wf.fPeak_Counter[0][0]);
            h2MBH2->Fill(wf.fPeak_Tub[1], wf.fPeak_Counter[1][0]);
        }
              
    }
    
    if (s_nTriggersBeam==0) s_nTriggersBeam = 1;
    if (s_nTriggersLED==0) s_nTriggersLED = 1;
    
    hCH0->Scale(1./s_nTriggersBeam);
    hCH1->Scale(1./s_nTriggersBeam);
    hCH2->Scale(1./s_nTriggersBeam);
    hCH3->Scale(1./s_nTriggersBeam);
    hCH0LED->Scale(1./s_nTriggersLED);
    hCH1LED->Scale(1./s_nTriggersLED);
    hCH2LED->Scale(1./s_nTriggersLED);
    hCH3LED->Scale(1./s_nTriggersLED);
    
    s_meanCharge_Tub1 /= s_nTriggersBeam;
    s_meanCharge_Tub2 /= s_nTriggersBeam;
    s_meanPeak_Tub1 /= s_nTriggersBeam;
    s_meanPeak_Tub2 /= s_nTriggersBeam;
    
    s_meanCharge_LED1 /= s_nTriggersLED;
    s_meanCharge_LED2 /= s_nTriggersLED;
    s_meanPeak_LED1 /= s_nTriggersLED;
    s_meanPeak_LED2 /= s_nTriggersLED;
    
    s_meanPeak_Counter1Pulse1 /= s_nTriggersBeam;
    s_meanPeak_Counter1Pulse2 /= s_nTriggersLED;
    s_meanPeak_Counter2Pulse1 /= s_nTriggersBeam;
    s_meanPeak_Counter2Pulse2 /= s_nTriggersBeam;
    // cout << s_meanCharge_Counter2Pulse1 << ", " << s_meanCharge_Counter2Pulse2 << endl;
    // cout << s_meanCharge_LED1 << endl;
    
    h_nTriggersBeam->SetPoint(h_nTriggersBeam->GetN(), f_run_number, s_nTriggersBeam);
    h_nTriggersLED->SetPoint(h_nTriggersLED->GetN(), f_run_number, s_nTriggersLED);
    h_meanCharge_Tub1->SetPoint(h_meanCharge_Tub1->GetN(), f_run_number, s_meanCharge_Tub1);
    h_meanCharge_Tub2->SetPoint(h_meanCharge_Tub2->GetN(), f_run_number, s_meanCharge_Tub2);
    h_meanCharge_LED1->SetPoint(h_meanCharge_LED1->GetN(), f_run_number, s_meanCharge_LED1);
    h_meanCharge_LED2->SetPoint(h_meanCharge_LED2->GetN(), f_run_number, s_meanCharge_LED2);
    
    h_meanPeak_Tub1->SetPoint(h_meanPeak_Tub1->GetN(), f_run_number, s_meanPeak_Tub1);
    h_meanPeak_Tub2->SetPoint(h_meanPeak_Tub2->GetN(), f_run_number, s_meanPeak_Tub2);
    h_meanPeak_LED1->SetPoint(h_meanPeak_LED1->GetN(), f_run_number, s_meanPeak_LED1);
    h_meanPeak_LED2->SetPoint(h_meanPeak_LED2->GetN(), f_run_number, s_meanPeak_LED2);
    
    h_meanPeak_Counter1Pulse1->SetPoint(h_meanPeak_Counter1Pulse1->GetN(), f_run_number, s_meanPeak_Counter1Pulse1);
    h_meanPeak_Counter1Pulse2->SetPoint(h_meanPeak_Counter1Pulse2->GetN(), f_run_number, s_meanPeak_Counter1Pulse2);
    h_meanPeak_Counter2Pulse1->SetPoint(h_meanPeak_Counter2Pulse1->GetN(), f_run_number, s_meanPeak_Counter2Pulse1);
    h_meanPeak_Counter2Pulse2->SetPoint(h_meanPeak_Counter2Pulse2->GetN(), f_run_number, s_meanPeak_Counter2Pulse2);
    // cout << s_meanCharge_Counter1Pulse1 << ", " << s_meanCharge_Counter1Pulse2 << endl;
    // fSpillTree->Fill();
    g_processDone = true;
    
    cout << fLastFile << ": Beam Triggers: " << s_nTriggersBeam << " | LED Triggers: " << s_nTriggersLED << endl;
    
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
    // for (unsigned bin=0; bin<=WblsDaq::NFADCBins; ++bin) {
    //     hTDCTub1->SetBinContent(bin, r_CHymin[0] + hTDCTub1->GetBinContent(bin)*r_pulsetime_count_scaling);
    //     hTDCTub2->SetBinContent(bin, r_CHymin[1] + hTDCTub2->GetBinContent(bin)*r_pulsetime_count_scaling);
    //     hTDCCounter1Pulse1->SetBinContent(bin, r_CHymin[2] + hTDCCounter1Pulse1->GetBinContent(bin)*r_pulsetime_count_scaling);
    //     hTDCCounter1Pulse2->SetBinContent(bin, r_CHymin[2] + hTDCCounter1Pulse2->GetBinContent(bin)*r_pulsetime_count_scaling);
    //     hTDCCounter2Pulse1->SetBinContent(bin, r_CHymin[3] + hTDCCounter2Pulse1->GetBinContent(bin)*r_pulsetime_count_scaling);
    //     hTDCCounter2Pulse2->SetBinContent(bin, r_CHymin[3] + hTDCCounter2Pulse2->GetBinContent(bin)*r_pulsetime_count_scaling);
    // }
    
    
    // == Main Canvas ==
    fCanvas->cd(1);
    // hCH0->GetXaxis()->SetRangeUser(500, 2000);
    hCH0->GetYaxis()->SetRangeUser(r_CHymin[0], r_CHymax[0]);
    // hCH0->GetXaxis()->SetRangeUser(200, 1500);
    hCH0->Draw();
    // hTDCTub1->Draw("same");
    fLine_SigTub[0][0]->Draw();
    fLine_SigTub[0][1]->Draw();
    fLine_SigLED[0][0]->Draw();
    fLine_SigLED[0][1]->Draw();
    
    fCanvas->cd(2);
    // hCH1->GetXaxis()->SetRangeUser(500, 2000);
    hCH1->GetYaxis()->SetRangeUser(r_CHymin[1], r_CHymax[1]);
    // hCH1->GetXaxis()->SetRangeUser(200, 1500);
    hCH1->Draw();
    // hTDCTub2->Draw("same");
    fLine_SigTub[1][0]->Draw();
    fLine_SigTub[1][1]->Draw();
    fLine_SigLED[1][0]->Draw();
    fLine_SigLED[1][1]->Draw();
    
    fCanvas->cd(3);
    // hCH2->GetXaxis()->SetRangeUser(500, 2000);
    hCH2->GetYaxis()->SetRangeUser(r_CHymin[2], r_CHymax[2]);
    // hCH2->GetXaxis()->SetRangeUser(200, 1500);
    hCH2->Draw();
    // hTDCCounter1Pulse1->Draw("same");
    // hTDCCounter1Pulse2->Draw("same");
    fLine_SigCounter[0][0][0]->Draw();
    fLine_SigCounter[0][0][1]->Draw();
    fLine_SigCounter[0][1][0]->Draw();
    fLine_SigCounter[0][1][1]->Draw();
    
    fCanvas->cd(4);
    // hCH3->GetXaxis()->SetRangeUser(500, 2000);
    hCH3->GetYaxis()->SetRangeUser(r_CHymin[3], r_CHymax[3]);
    // hCH3->GetXaxis()->SetRangeUser(200, 1500);
    hCH3->Draw();
    // hTDCCounter2Pulse1->Draw("same");
    // hTDCCounter2Pulse2->Draw("same");
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
    hPeakTub1->Draw();
    hPeakTub2->Draw("same");
        
    fCanvas->cd(7);
    hPeakLED1->Draw();
    hPeakLED2->Draw("same");
    
    fCanvas->cd(8);
    h2Tub1Tub2->Draw("colz");
    
    fCanvas->cd(9);
    h2H1H2->Draw("colz");
    
    fCanvas->cd(10);
    hDtTriggers->Draw();
        
    fCanvas->cd(11);
    h_meanPeak_Tub1->GetXaxis()->SetTitle("Run Number");
    h_meanPeak_Tub1->Draw("AP");
    h_meanPeak_Tub1->GetYaxis()->SetRangeUser(0, r_meanPeak_Tub);
    h_meanPeak_Tub2->Draw("P,same");
    
    fCanvas->cd(12);
    h_meanPeak_LED1->GetXaxis()->SetTitle("Run Number");
    h_meanPeak_LED1->Draw("AP");
    h_meanPeak_LED1->GetYaxis()->SetRangeUser(0, r_meanPeak_LED);
    h_meanPeak_LED2->Draw("P,same");
    
    fCanvas->cd(13);
    h2MAH1->Draw("colz");
    
    fCanvas->cd(14);
    h2MBH2->Draw("colz");
    
    fCanvas->cd(15);
    h_nTriggersBeam->GetXaxis()->SetTitle("Run Number");
    h_nTriggersBeam->Draw("AP");
    h_nTriggersBeam->GetYaxis()->SetRangeUser(0, 250);
    h_nTriggersLED->Draw("P,same");
    
    // == Second Canvas ==
    fCanvas2->cd(1);
    // hCH0LED->GetYaxis()->SetRangeUser(8120, 8160);
    hCH0LED->Draw();
    fLine_SigTub[0][0]->Draw();
    fLine_SigTub[0][1]->Draw();
    fLine_SigLED[0][0]->Draw();
    fLine_SigLED[0][1]->Draw();
    
    fCanvas2->cd(2);
    // hCH1LED->GetYaxis()->SetRangeUser(8160, 8200);
    hCH1LED->Draw();
    fLine_SigTub[1][0]->Draw();
    fLine_SigTub[1][1]->Draw();
    fLine_SigLED[1][0]->Draw();
    fLine_SigLED[1][1]->Draw();
    
    fCanvas2->cd(3);
    hCH2LED->GetYaxis()->SetRangeUser(r_CHymin[2], r_CHymax[2]);
    hCH2LED->Draw();
    fLine_SigCounter[0][0][0]->Draw();
    fLine_SigCounter[0][0][1]->Draw();
    fLine_SigCounter[0][1][0]->Draw();
    fLine_SigCounter[0][1][1]->Draw();
    
    fCanvas2->cd(4);
    hCH3LED->GetYaxis()->SetRangeUser(r_CHymin[3], r_CHymax[3]);
    hCH3LED->Draw();
    fLine_SigCounter[1][0][0]->Draw();
    fLine_SigCounter[1][0][1]->Draw();
    fLine_SigCounter[1][1][0]->Draw();
    fLine_SigCounter[1][1][1]->Draw();

    
    fCanvas2->cd(6);
    hChargeTub1->Draw();
    hChargeTub2->Draw("same");
        
    fCanvas2->cd(7);
    hChargeLED1->Draw();
    hChargeLED2->Draw("same");
    
    fCanvas2->cd(8);
    hPeakCounter1Pulse2->Draw();
    hPeakCounter1Pulse1->Draw("same");
    
    fCanvas2->cd(9);
    hPeakCounter2Pulse1->Draw();
    hPeakCounter2Pulse2->Draw("same");
    
    fCanvas2->cd(11);
    h_meanCharge_Tub1->GetXaxis()->SetTitle("Run Number");
    h_meanCharge_Tub1->Draw("AP");
    h_meanCharge_Tub1->GetYaxis()->SetRangeUser(0, r_meanCharge_Tub);
    h_meanCharge_Tub2->Draw("P,same");
    
    fCanvas2->cd(12);
    h_meanCharge_LED1->GetXaxis()->SetTitle("Run Number");
    h_meanCharge_LED1->Draw("AP");
    h_meanCharge_LED1->GetYaxis()->SetRangeUser(0, r_meanCharge_LED);
    h_meanCharge_LED2->Draw("P,same");
    
    fCanvas2->cd(13);
    h_meanPeak_Counter1Pulse1->GetXaxis()->SetTitle("Run Number");
    h_meanPeak_Counter1Pulse1->Draw("AP");
    h_meanPeak_Counter1Pulse1->GetYaxis()->SetRangeUser(0, r_meanPeak_Counter1);
    h_meanPeak_Counter1Pulse2->Draw("P,same");
    
    fCanvas2->cd(14);
    h_meanPeak_Counter2Pulse1->GetXaxis()->SetTitle("Run Number");
    h_meanPeak_Counter2Pulse1->Draw("AP");
    h_meanPeak_Counter2Pulse1->GetYaxis()->SetRangeUser(0, r_meanPeak_Counter2);
    h_meanPeak_Counter2Pulse2->Draw("P,same");
    
    return 1;
}

