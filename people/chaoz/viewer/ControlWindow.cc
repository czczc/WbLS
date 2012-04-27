#include "ControlWindow.h"
#include "ViewWindow.h"

#include <iostream>
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TPad.h"
#include "TH1F.h"
#include "TAxis.h"

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

void ControlWindow::UpdateHitTimePanel()
{
    eventEntry->SetNumber(currentEvent);
    view->SetCurrentEvent(currentEvent);
    view->UpdateHitTime();
}

void ControlWindow::UpdateAll()
{
    eventEntry->SetNumber(currentEvent);
    view->SetCurrentEvent(currentEvent);
    view->UpdateHitTime();
    view->UpdatePMTMap();
}

ControlWindow::~ControlWindow()
{
    delete prev;
    delete eventEntry;
    delete next;
    delete fNavigationFrame;
}
