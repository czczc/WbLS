#ifndef CONTROL_WINDOW_H
#define CONTROL_WINDOW_H

#include "TGFrame.h"

class TGTextButton;
class ViewWindow;
class TGNumberEntryField;
class TGListBox;

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

    void UpdateAll();

private:
    ViewWindow        *view;
    TGHorizontalFrame *fNavigationFrame;
    TGTextButton      *prev, *next, *clear;
    TGNumberEntryField       *eventEntry;

    TGGroupFrame      *fTracksFrame;
    TGListBox         *fTracksListBox;
    void InitTrackListBox();
    void UpdateTrackListBox();

    ClassDef(ControlWindow, 0)
};

#endif