#include "ControlWindow.h"
#include "ViewWindow.h"
#include "MCEvent.h"

#include <iostream>
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGListBox.h"
#include "TPad.h"
#include "TH1F.h"
#include "TAxis.h"
#include "TString.h"
#include "TMath.h"

using namespace std;

ControlWindow::ControlWindow(const TGWindow *p, int w, int h, ViewWindow *v)
    :TGVerticalFrame(p, w, h)
{
    view = v;
    currentEvent = 0;

    //Navigation Frame
    fNavigationFrame = new TGHorizontalFrame(this, w, 100, kFixedWidth);
    AddFrame(fNavigationFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));

    prev = new TGTextButton(fNavigationFrame, "< Prev");
    fNavigationFrame->AddFrame(prev, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

    eventEntry = new TGNumberEntryField(fNavigationFrame, -1, 0, TGNumberFormat::kNESInteger);
    eventEntry->SetDefaultSize(50, 20);
    fNavigationFrame->AddFrame(eventEntry, new TGLayoutHints(kLHintsTop | kLHintsCenterY, 3, 2, 2, 2));
    eventEntry->Connect("ReturnPressed()", "ControlWindow", this, "Jump()");

    next = new TGTextButton(fNavigationFrame, "Next >");
    fNavigationFrame->AddFrame(next, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

    prev->Connect("Clicked()", "ControlWindow", this, "Prev()");
    next->Connect("Clicked()", "ControlWindow", this, "Next()");

    view->BotPanel()->Connect("RangeChanged()", "ControlWindow", this, "VisualCut()");

    // Tracks List Box
    InitTrackListBox();
    AddFrame(fTracksFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

}

void ControlWindow::Next()
{
    currentEvent++;
    UpdateAll();
}

void ControlWindow::Prev()
{
    currentEvent--;
    UpdateAll();
}

void ControlWindow::Jump()
{
    currentEvent = eventEntry->GetIntNumber();
    UpdateAll();
}

void ControlWindow::VisualCut()
{
    int xmin = view->hHitTime->GetXaxis()->GetFirst();
    int xmax = view->hHitTime->GetXaxis()->GetLast();
    (view->cuts).tc_min = xmin;
    (view->cuts).tc_max = xmax;
    view->UpdatePMTMap();
    // view->UpdateHitTime();

    cout << "VisualCut(): x axis: (" << xmin << ", " << xmax << ")" << endl;
}


void ControlWindow::UpdateAll()
{
    eventEntry->SetNumber(currentEvent);
    view->SetCurrentEvent(currentEvent);
    view->ClearVirtualRing();
    view->UpdateHitTime();
    view->UpdatePMTMap();
    UpdateTrackListBox();
}

void ControlWindow::InitTrackListBox()
{
    fTracksFrame = new TGGroupFrame(this, "Visible Tracks", kVerticalFrame);
    fTracksFrame->SetTitlePos(TGGroupFrame::kLeft);
    fTracksListBox = new TGListBox(fTracksFrame, 120);
    UpdateTrackListBox();
    fTracksFrame->AddFrame(fTracksListBox, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    fTracksListBox->Resize(100, 160);
    fTracksListBox->Connect("Selected(int)", "ViewWindow", view, "DrawTrack(int)");
    clear = new TGTextButton(fTracksFrame, "Clear");
    fTracksFrame->AddFrame(clear, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
    clear->Connect("Clicked()", "ViewWindow", view, "ClearVirtualRing()");
}

void ControlWindow::UpdateTrackListBox()
{
    fTracksListBox->RemoveAll();

    TString title;
    int nTrack = view->nVisTrack;
    MCEvent *ev = view->fEvent;
    for (int i = 0; i < nTrack; i++) {
        int id = view->visTrackID[i];
        int pdg = ev->track_pdg[id];
        TString name;
        switch (pdg) {
            case 321:
                name = "K+";
                break;
            case 211:
                name = "Pi+";
                break;
            case -211:
                name = "Pi-";
                break;
            case 111:
                name = "Pi0";
                break;
            case 22:
                name = "Gamma";
                break;
            case -11:
                name = "e+";
                break;
            case 11:
                name = "e-";
                break;
            case -13:
                name = "mu+";
                break;
            case 13:
                name = "mu-";
                break;
            default:
                name = "Unknown";
        }
        title.Form("%s: %.1f MeV", name.Data(), ev->track_e[id]);
        fTracksListBox->NewEntry(title.Data());
    }
}

ControlWindow::~ControlWindow()
{
    delete prev;
    delete eventEntry;
    delete next;
    delete fNavigationFrame;
    delete fTracksFrame;
}
