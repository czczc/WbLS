#include "MainWindow.h"
#include "ViewWindow.h"
#include "ControlWindow.h"

#include "TApplication.h"
#include "TRootEmbeddedCanvas.h"
#include "TGMenu.h"
#include "TGFileDialog.h"
#include "TString.h"

#include <iostream>
using namespace std;


MainWindow::MainWindow(const TGWindow *p, int w,int h)
    : TGMainFrame(p, w, h)
{
    InitMenu();

    fViewAndControlFrame = new TGHorizontalFrame(this, w, h, kFixedWidth);
    AddFrame(fViewAndControlFrame, new TGLayoutHints(kLHintsExpandX  | kLHintsExpandY, 2, 2, 2, 2));

    fViewWindow = new ViewWindow(fViewAndControlFrame, w-200, h);
    fViewAndControlFrame->AddFrame(fViewWindow, new TGLayoutHints(
        // kLHintsTop | kFixedWidth, 2, 2, 2, 2));
        kLHintsTop | kLHintsExpandX  | kLHintsExpandY, 2, 2, 2, 2));

    fControlWindow = new ControlWindow(fViewAndControlFrame, 200, h, fViewWindow);
    fViewAndControlFrame->AddFrame(fControlWindow, new TGLayoutHints(
        kLHintsTop  | kFixedWidth, 2, 2, 2, 2));

    // set main window stats
    SetWindowName("WbLS Event Viewer");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

void MainWindow::InitMenu()
{
    fMenuBar = new TGMenuBar(this, 10, 100);
    AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
    fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

    fMenuFile = new TGPopupMenu(gClient->GetRoot());
    fMenuFile->AddEntry("&Open...", M_FILE_OPEN);
    fMenuFile->AddSeparator();
    fMenuFile->AddEntry(new TGHotString("E&xit"), M_FILE_EXIT);
    fMenuFile->Connect("Activated(int)", "MainWindow", this, "HandleMenu(int)");

    fMenuBar->AddPopup(new TGHotString("&File"), fMenuFile, fMenuBarItemLayout);

}

void MainWindow::HandleMenu(int id)
{
    const char *filetypes[] = {"ROOT files", "*.root", 0, 0};
    switch (id) {
        case M_FILE_OPEN:
            {
                static TString dir("./data");
                TGFileInfo fi;
                fi.fFileTypes = filetypes;
                fi.fIniDir    = StrDup(dir);
                // printf("fIniDir = %s\n", fi.fIniDir);
                new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi);
                // printf("Open file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
                dir = fi.fIniDir;
                if (fi.fFilename) {
                    cout << "file: " << fi.fFilename << endl;
                    fViewWindow->Open(fi.fFilename);
                }
            }
            break;

        case M_FILE_EXIT:
            gApplication->Terminate(0);
            break;
    }
}

MainWindow::~MainWindow()
{
    CleanUp();
}

void MainWindow::CleanUp()
{
    delete fViewWindow;
    delete fMenuBar;
}
