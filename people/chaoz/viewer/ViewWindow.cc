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

#include <iostream>
using namespace std;


ViewWindow::ViewWindow(const TGWindow *p, int w,int h)
	:TRootEmbeddedCanvas("ECanvas", p, w, h)
{
	currentEvent = 0;
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
	hHitTime->GetYaxis()->SetTitle("NPE");
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
	for (int i=0; i<fEvent->nHit; i++) {
		if (fEvent->hit_tc[i]>cuts.tc_min && fEvent->hit_tc[i]<cuts.tc_max) {
			UpdatePMT(fEvent->hit_pmtID[i]);
		}
	}
	fTopPanel->Update();
	UpdateCanvas();
	// fBotPanel->cd();
	// fTopPanel->Update();

}

void ViewWindow::UpdatePMT(int id)
{
	pmtPE[id]++;
	pmtMarker[id]->SetMarkerSize(0.3 * TMath::Sqrt(pmtPE[id]));

	if (pmtPE[id] == 1) { pmtMarker[id]->SetMarkerColor(45); }
	else { pmtMarker[id]->SetMarkerColor(kRed); }
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
	fEvent->GetEntry(currentEvent);
	hContainer->SetTitle(fEvent->decayMode[fEvent->mode].c_str());
	fBotPanel->Update();
}

void ViewWindow::InitStyle()
{
	gROOT->SetStyle("Plain");
	gStyle->SetOptStat(0);
	gStyle->SetPalette(1);
	gStyle->SetTitleStyle(0);
	gStyle->SetTitleBorderSize(0);
	gStyle->SetTitleOffset(1.1, "x");
	gStyle->SetTitleOffset(1.5, "y");
	gStyle->SetHistLineWidth(2);
	gStyle->SetLegendBorderSize(0);
	gStyle->SetPadLeftMargin(0.14);
	gStyle->SetPadRightMargin(0.04);
	gStyle->SetMarkerSize(0.3);
	gROOT->ForceStyle();
	// Sim->UseCurrentStyle();

}

void ViewWindow::UpdateCanvas()
{
	// Canvas()->cd();
	fBotPanel->cd();

	// fTopPanel->cd();
	// fTopPanel->Update();
	// fBotPanel->cd();
	// fBotPanel->Update();
	// Canvas()->Modified();
	// fBotPanel->Modified();

	// Canvas()->Update();
	// gPad->cd(0);
}

ViewWindow::~ViewWindow(){}
