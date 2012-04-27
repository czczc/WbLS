#ifndef CONTROL_WINDOW_H
#define CONTROL_WINDOW_H

#include "TGFrame.h"

class TGTextButton;
class ViewWindow;
class TGNumberEntryField;

class ControlWindow: public TGVerticalFrame
{
public:
    ControlWindow(const TGWindow *p, int w, int h, ViewWindow *v);
    virtual ~ControlWindow();
    int currentEvent;

    void Prev();
    void Next();
    void Jump();
    void VisualCut();

    void UpdateHitTimePanel();
    void UpdateAll();

private:
    ViewWindow        *view;
    TGHorizontalFrame *fNavigationFrame;
    TGTextButton      *prev, *next;
    TGNumberEntryField       *eventEntry;

    ClassDef(ControlWindow, 0)
};

#endif