#include "WblsDaqEvent.h"

#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"

#include <iostream>

using namespace std;

void fakeDAQ(const char* outFile = "/Users/chaozhang/Projects/LBNE/WbLS/software/WbLSDAQ/data/test/fakedata.root")
{
    WblsDaq::Spinner spinner("/Users/chaozhang/Projects/LBNE/WbLS/software/WbLSDAQ/data/test/example.root");

    TTree* header = spinner.header_tree();
    TTree* footer = spinner.footer_tree();
    TTree* events = spinner.event_tree();
    
    const int N = 120; // fake 400 events
    int nentries = events->GetEntries();
    gRandom->SetSeed(0);
    int startN = int(gRandom->Uniform(nentries));
    TString sel = Form("Entry$>=%d && Entry$<%d", startN, startN + N);
    TFile f(outFile, "recreate");    
    TTree *th = header->CopyTree("Entry$<8");
    TTree *tf = footer->CopyTree("Entry$<8");
    TTree *te = events->CopyTree(sel.Data());
    // TTree *te = events->CopyTree("Entry$>1850 && Entry$<1970");
    // cout << sel << endl;
    th->Write();
    tf->Write();
    te->Write();
    f.Close();
    // cout << outFile << endl;
}