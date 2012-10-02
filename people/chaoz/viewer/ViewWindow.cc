#include "ViewWindow.h"

#include "TRootEmbeddedCanvas.h"
#include "MCEvent.h"

#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TBox.h"
#include "TEllipse.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TString.h"
#include "TMath.h"
#include "TMarker.h"
#include "TColor.h"
#include "TGraph.h"

#include <iostream>
using namespace std;


ViewWindow::ViewWindow(const TGWindow *p, int w,int h)
    :TRootEmbeddedCanvas("ECanvas", p, w, h)
{
    currentEvent = 0;
    vRing = 0;
    InitCuts();
    InitStyle();
    InitHist();
    InitCanvas();
}

void ViewWindow::Open(const char* fileName)
{
    if (fEvent) delete fEvent;
    fEvent = new MCEvent(fileName);
    // currentEvent = 0;
    if (hHitTime) delete hHitTime;
    InitDrawHist();
    UpdateHitTime();
    UpdatePMTMap();
}

void ViewWindow::InitCuts()
{
    cuts.tc_min = 0;
    cuts.tc_max = 20000;
}

void ViewWindow::InitHist()
{
    fEvent = new MCEvent("data/events100_py90.root");
    SetCurrentEvent(0);

    float radius = fEvent->cylRadius;
    float halfX = fEvent->cylRadius * TMath::Pi();
    float halfY = fEvent->cylLength / 2;

    hContainer = new TH2F("hContainer", "", 100, -halfX, halfX, 100, -(halfY+2*radius), halfY+2*radius);
    hContainer->GetXaxis()->SetLabelSize(0);
    hContainer->GetYaxis()->SetLabelSize(0);


    WCMain = new TBox(-halfX, -halfY, halfX, halfY);
    WCMain->SetFillStyle(0);
    WCMain->SetLineColor(1);
    WCMain->SetLineWidth(2);

    WCTop = new TEllipse(0, halfY+radius, radius);
    WCTop->SetFillStyle(0);
    WCTop->SetLineColor(1);
    WCTop->SetLineWidth(2);

    WCBottom = new TEllipse(0, -(halfY+radius), radius);
    WCBottom->SetFillStyle(0);
    WCBottom->SetLineColor(1);
    WCBottom->SetLineWidth(2);

    InitDrawHist();
}

void ViewWindow::InitDrawHist()
{
    hHitTime = new TH1F("hHitTime", "", 20000, 1, 20000);
    hHitTime->SetFillColor(kCyan);
    hHitTime->GetXaxis()->SetTitle("Hit Time [ns]");
    hHitTime->GetYaxis()->SetTitle("Number of PE");
    hHitTime->GetYaxis()->SetRangeUser(0, 150);
    hHitTime->GetYaxis()->SetNdivisions(505);
}

void ViewWindow::UpdatePMTMap()
{
    fTopPanel->cd();
    for (int i=0; i<fEvent->totalPMTs; i++) {
        pmtPE[i] = 0;
        pmtMarker[i]->SetMarkerSize(0);
    }
    fEvent->GetEntry(currentEvent);
    int npe = 0;
    for (int i=0; i<fEvent->nHit; i++) {
        if (fEvent->hit_tc[i]>cuts.tc_min && fEvent->hit_tc[i]<cuts.tc_max) {
            UpdatePMT(fEvent->hit_pmtID[i], fEvent->hit_tc[i]);
            npe++;
        }
    }
    cout << "PE:" << npe << " | total PE: " << hHitTime->GetEntries() << endl;
    fTopPanel->Update();
    UpdateCanvas();
}

void ViewWindow::UpdatePMT(int id, float tc)
{
    pmtPE[id]++;
    pmtMarker[id]->SetMarkerSize(0.3 * TMath::Sqrt(pmtPE[id]));

    if (pmtPE[id] == 1) { pmtMarker[id]->SetMarkerColor(45); }
    else { pmtMarker[id]->SetMarkerColor(kRed); }
    
    if (tc>100) { ; }
    pmtMarker[id]->Draw();
}

void ViewWindow::InitPMT(int id)
{
    float radius = fEvent->cylRadius;
    float halfY = fEvent->cylLength / 2;

    float x = fEvent->pmt_x[id];
    float y = fEvent->pmt_y[id];
    float x_ = -TMath::ATan2(y, x) * radius;
    float y_ = fEvent->pmt_z[id];;
    if ( y_ > halfY - 0.01 ) {
        x_ = -y;
        y_ = -x + halfY + radius;
    }
    else if (y_ < -(halfY - 0.01)) {
        x_ = -y;
        y_ = x - halfY - radius;
    }

    pmtPE[id] = 0;
    pmtMarker[id] = new TMarker(x_, y_, 25);
    pmtMarker[id]->SetMarkerSize(0);
    pmtMarker[id]->Draw();
}

void ViewWindow::InitCanvas()
{
    Canvas()->cd();
    fTopPanel = new TPad("TopPanel", "TopPanel", 0., .3, 1, 1.);
    fTopPanel->SetBottomMargin(0.02);
    fTopPanel->SetFillColor(0);
    fTopPanel->SetFillStyle(0);
    fTopPanel->Draw();
    // cout << gPad << endl;

    Canvas()->cd();
    fBotPanel = new TPad("BotPanel", "BotPanel", 0., 0., 1, .3);
    fBotPanel->SetTopMargin(0.02);
    fBotPanel->SetFillColor(0);
    fBotPanel->SetFillStyle(0);
    fBotPanel->SetLogx();
    fBotPanel->Draw();
    UpdateHitTime();

    fTopPanel->cd();
    hContainer->Draw();
    WCMain->Draw();
    WCTop->Draw();
    WCBottom->Draw();
    for (int i=0; i<fEvent->totalPMTs; i++) {
        InitPMT(i);
    }
    UpdatePMTMap();
}

void ViewWindow::UpdateHitTime()
{
    TString selection;
    selection.Form("Entry$==%d", currentEvent);
    fBotPanel->cd();
    fEvent->Tree()->Draw("hit_tc>>hHitTime", selection);
    hHitTime->GetXaxis()->SetRangeUser(cuts.tc_min, cuts.tc_max);
    // fEvent->GetEntry(currentEvent);
    hContainer->SetTitle(fEvent->decayMode[fEvent->mode].c_str());
    fBotPanel->Update();
}

void ViewWindow::SetCurrentEvent(int eventNo)
{
    currentEvent = eventNo;
    fEvent->GetEntry(currentEvent);
    // fEvent->PrintInfo();
    int nTrack = fEvent->nTrack;
    nVisTrack = 0;
    for (int i = 2; i < nTrack; i++) {
        int pdg = fEvent->track_pdg[i];
        int abs_pdg = TMath::Abs(pdg);
        // TString name;
        if ( abs_pdg == 14 || abs_pdg == 12) continue; // nu_mu or nu_e
        visTrackID[nVisTrack] = i;
        nVisTrack++;
    }
}

void ViewWindow::InitStyle()
{
    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(0);
    gStyle->SetPalette(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleOffset(1.3, "x");
    gStyle->SetTitleOffset(0.8, "y");
    gStyle->SetLabelSize(0.05, "x");
    gStyle->SetLabelSize(0.05, "y");
    gStyle->SetHistLineWidth(2);
    gStyle->SetLegendBorderSize(0);
    gStyle->SetPadLeftMargin(0.08);
    gStyle->SetPadRightMargin(0.04);
    gStyle->SetMarkerSize(0.3);
    gROOT->ForceStyle();
    // Sim->UseCurrentStyle();
}

void ViewWindow::UpdateCanvas()
{
    fBotPanel->cd();
}

void ViewWindow::DrawTrack(int listID)
{
    ClearVirtualRing();
    int track_id = visTrackID[listID-1];
    int resolution = 200;
    // cout << "track #" << track_id << endl;
    vector< vector<float> > vertice = fEvent->FindRing(track_id, resolution);
    if (vertice.size() == 0) { return; }
    // fEvent->PrintInfo();
    vRing = new TGraph(resolution);
    float radius = fEvent->cylRadius;
    float halfY = fEvent->cylLength / 2;
    for (int i=0; i<resolution; i++) {
        float x = vertice[i][0];
        float y = vertice[i][1];
        float x_ = -TMath::ATan2(y, x) * radius;
        float y_ = vertice[i][2];;
        if ( y_ > halfY - 0.01 ) {
            x_ = -y;
            y_ = -x + halfY + radius;
        }
        else if (y_ < -(halfY - 0.01)) {
            x_ = -y;
            y_ = x - halfY - radius;
        }
        vRing->SetPoint(i, x_, y_);
    }
    fTopPanel->cd();
    vRing->Draw("P");
    vRing->SetMarkerColor(kBlue);
    vRing->SetMarkerStyle(20);
    vRing->SetMarkerSize(0.6); 
    fTopPanel->Update();
}

void ViewWindow::ClearVirtualRing() 
{ 
    if (vRing) {
        delete vRing; 
        vRing = 0; 
        fTopPanel->Update();
    } 
}

ViewWindow::~ViewWindow(){}
