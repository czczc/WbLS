#include "MCEvent.h"

#include <iostream>

#include "TFile.h"
#include "TTree.h"


using namespace std;

MCEvent::MCEvent(){}

MCEvent::MCEvent(const char* filename)
{
    rootFile = new TFile(filename);
    simTree = (TTree*)rootFile->Get("/Event/Sim");
    geoTree = (TTree*)rootFile->Get("/Detector/Geometry");

    InitDecayMode();
    InitBranchAddress();
    InitGeometry();
}

MCEvent::~MCEvent()
{
}

void MCEvent::InitDecayMode()
{
    decayMode[0]  = "Unknown??";
    decayMode[11] = "K^{+} -> #mu^{+} + #nu_{#mu} (63.47%)";           // 63.47%
    decayMode[21] = "K^{+} -> #pi^{+} + #pi^{0} (21.13%)";             // 21.13%
    decayMode[31] = "K^{+} -> #pi^{+} + #pi^{+} + #pi^{-} (5.58%)";    // 5.58%
    decayMode[41] = "K^{+} -> #pi^{0} + e^{+} + #nu_{e} (4.87%)";      // 4.87%
    decayMode[51] = "K^{+} -> #pi^{0} + #mu^{+} + #nu_{#mu} (3.27%)";  // 3.27%
    decayMode[61] = "K^{+} -> #pi^{+} + #pi^{0} + #pi^{0} (1.73%)";    // 1.73%
}

void MCEvent::InitBranchAddress()
{
    simTree->SetBranchAddress("eventNumber" , &eventNumber);
    simTree->SetBranchAddress("mode"        , &mode);
    simTree->SetBranchAddress("nTrigger"    , &nTrigger);

    simTree->SetBranchAddress("nTrack"      , &nTrack);
    simTree->SetBranchAddress("track_pdg"   , &track_pdg);
    simTree->SetBranchAddress("track_parent", &track_parent);
    simTree->SetBranchAddress("track_t"     , &track_t);
    simTree->SetBranchAddress("track_e"     , &track_e);
    simTree->SetBranchAddress("track_ee"    , &track_ee);
    simTree->SetBranchAddress("track_x"     , &track_x);
    simTree->SetBranchAddress("track_y"     , &track_y);
    simTree->SetBranchAddress("track_z"     , &track_z);
    simTree->SetBranchAddress("track_dx"    , &track_dx);
    simTree->SetBranchAddress("track_dy"    , &track_dy);
    simTree->SetBranchAddress("track_dz"    , &track_dz);

    simTree->SetBranchAddress("nPMT"        , &nPMT);
    simTree->SetBranchAddress("nHit"        , &nHit);
    simTree->SetBranchAddress("hit_pmtID"   , &hit_pmtID);
    simTree->SetBranchAddress("hit_t"       , &hit_t);
    simTree->SetBranchAddress("hit_tc"      , &hit_tc);
    simTree->SetBranchAddress("hit_wl"      , &hit_wl);
}

void MCEvent::InitGeometry()
{
    geoTree->SetBranchAddress("cylRadius", &cylRadius);
    geoTree->SetBranchAddress("cylLength", &cylLength);
    geoTree->SetBranchAddress("totalPMTs", &totalPMTs);
    geoTree->SetBranchAddress("pmt_x", &pmt_x);
    geoTree->SetBranchAddress("pmt_y", &pmt_y);
    geoTree->SetBranchAddress("pmt_z", &pmt_z);

    geoTree->GetEntry(0);
}

void MCEvent::PrintInfo()
{
    cout << "=================================\n";
    cout << "#" << eventNumber << ": nTrigger: " << nTrigger
         << " | nTrack: "<< nTrack
         << " | nPMT: "<< nPMT
         << " | nHit: "<< nHit
         << "\n\tDecay Mode " << mode << ": " << decayMode[mode]
         << "\n";
    for (int i=2; i<nTrack; i++) {
         cout << "\tPDG: " << track_pdg[i]
             << " | Parent: " << track_parent[i]
             << " | Time: " << track_t[i]
             << " | KE: " << track_e[i]
             << " | E: " << track_ee[i]
             << " | Vtx: (" << track_x[i] << ", " << track_y[i] << ", " << track_z[i] << ")"
             << " | Dir: (" << track_dx[i] << ", " << track_dy[i] << ", " << track_dz[i] << ")"
             << "\n";
    }
    cout << endl;
}
