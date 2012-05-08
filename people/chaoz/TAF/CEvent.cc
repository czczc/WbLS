#include "CEvent.h"

#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TVector3.h"

#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"

using namespace std;

CEvent::CEvent()
    :rootFile(0), wcsimT(0), rawEvent(0), outputFile(0), outputTree(0), geoTree(0)
{}

CEvent::CEvent(const char* filename, const char* outname)
{
    rootFile = new TFile(filename);
    rawEvent = new WCSimRootEvent();

    wcsimT = (TTree*)rootFile->Get("wcsimT");
    wcsimT->SetBranchAddress("wcsimrootevent", &rawEvent);

    InitDecayMode();
    InitOutput(outname);
    InitPMTGeom();
}

CEvent::~CEvent()
{
}

void CEvent::InitDecayMode()
{
    // 63.47%
    decayMode[11] = "K+ -> mu+ + nu_mu, mu+ -> e+ + nu_mu| + nu_e"; // 321 -> 14 + -13, -13 -> -11 + -14 + 12
    // 21.13%
    decayMode[21] = "K+ -> pi+ + pi0, pi+ -> mu+ + nu_mu, mu+ -> e+ + nu_mu| + nu_e, pi0 -> 2*gamma";
    // 5.58%
    decayMode[31] = "K+ -> 2*pi+ + pi-,  pi+ -> mu+ + nu_mu, mu+ -> e+ + nu_mu| + nu_e, pi- disppear";
    // 4.87%
    decayMode[41] = "K+ -> pi0 + e+ + nu_e, pi0 -> 2*gamma";
    // 3.27%
    decayMode[51] = "K+ -> pi0 + mu+ + nu_mu, mu+ -> e+ + nu_mu| + nu_e, pi0 -> 2*gamma,";
    // 1.73%
    decayMode[61] = "K+ -> pi+ + 2*pi0, pi+ -> mu+ + nu_mu, mu+ -> e+ + nu_mu| + nu_e, pi0 -> 2*gamma";
}

void CEvent::InitOutput(const char* filename)
{
    TDirectory* tmpDir = gDirectory;
    outputFile = new TFile(filename, "recreate");
    TDirectory* subDir = outputFile->mkdir("Event");
    subDir->cd();
    outputTree = new TTree("Sim","Event Tree from Simulation");

    outputTree->Branch("eventNumber" , &eventNumber, "eventNumber/I");
    outputTree->Branch("mode"        , &mode, "mode/I");
    outputTree->Branch("nTrigger"    , &nTrigger, "nTrigger/I");

    outputTree->Branch("nTrack"      , &nTrack, "nTrack/I");
    outputTree->Branch("track_pdg"   , &track_pdg, "track_pdg[nTrack]/I");
    outputTree->Branch("track_parent", &track_parent, "track_parent[nTrack]/I");
    outputTree->Branch("track_t"     , &track_t, "track_t[nTrack]/F");
    outputTree->Branch("track_e"     , &track_e, "track_e[nTrack]/F");
    outputTree->Branch("track_ee"    , &track_ee, "track_ee[nTrack]/F");
    outputTree->Branch("track_x"     , &track_x, "track_x[nTrack]/F");
    outputTree->Branch("track_y"     , &track_y, "track_y[nTrack]/F");
    outputTree->Branch("track_z"     , &track_z, "track_z[nTrack]/F");
    outputTree->Branch("track_dx"    , &track_dx, "track_dx[nTrack]/F");
    outputTree->Branch("track_dy"    , &track_dy, "track_dy[nTrack]/F");
    outputTree->Branch("track_dz"    , &track_dz, "track_dz[nTrack]/F");

    outputTree->Branch("nPMT"        , &nPMT, "nPMT/I");
    outputTree->Branch("nHit"        , &nHit, "nHit/I");
    outputTree->Branch("hit_pmtID"   , &hit_pmtID, "hit_pmtID[nHit]/I");
    outputTree->Branch("hit_t"       , &hit_t, "hit_t[nHit]/F");
    outputTree->Branch("hit_tc"      , &hit_tc, "hit_tc[nHit]/F");
    outputTree->Branch("hit_wl"      , &hit_wl, "hit_wl[nHit]/F");

    TDirectory* subDir2 = outputFile->mkdir("Detector");
    subDir2->cd();
    geoTree = new TTree("Geometry", "Detector Geometry");
    geoTree->Branch("cylRadius", &cylRadius, "cylRadius/F");
    geoTree->Branch("cylLength", &cylLength, "cylLength/F");
    geoTree->Branch("orientation", &orientation, "orientation/I");
    geoTree->Branch("geoType", &geoType, "geoType/I");
    geoTree->Branch("totalPMTs", &totalPMTs, "totalPMTs/I");
    geoTree->Branch("pmt_x", &pmt_x, "pmt_x[totalPMTs]/F");
    geoTree->Branch("pmt_y", &pmt_y, "pmt_y[totalPMTs]/F");
    geoTree->Branch("pmt_z", &pmt_z, "pmt_z[totalPMTs]/F");

    gDirectory = tmpDir;
}

void CEvent::SaveOutput()
{
    TDirectory* tmpDir = gDirectory;
    outputFile->cd("/Event");
    outputTree->Write();
    outputTree = 0;

    gDirectory = tmpDir;
}

void CEvent::InitPMTGeom()
{
    // PMT Geometry Tree
    TTree *gtree = (TTree*)rootFile->Get("wcsimGeoT");
    WCSimRootGeom *wcsimrootgeom = new WCSimRootGeom();
    gtree->GetBranch("wcsimrootgeom")->SetAddress(&wcsimrootgeom);
    gtree->GetEntry(0);

    cylRadius = wcsimrootgeom->GetWCCylRadius();
    cylLength = wcsimrootgeom->GetWCCylLength();
    geoType   = wcsimrootgeom->GetGeo_Type();
    orientation = wcsimrootgeom->GetOrientation();
    totalPMTs = wcsimrootgeom->GetWCNumPMT();
    // cout << "total PMTs: " << totalPMTs << endl;
    for (int i=0; i!=totalPMTs; i++){
        pmt_x[i] = wcsimrootgeom->GetPMT(i).GetPosition(0);
        pmt_y[i] = wcsimrootgeom->GetPMT(i).GetPosition(1);
        pmt_z[i] = wcsimrootgeom->GetPMT(i).GetPosition(2);
    }
    geoTree->Fill();

    // Write geoTree to Disk
    TDirectory* tmpDir = gDirectory;
    outputFile->cd("/Detector");
    geoTree->Write();
    geoTree = 0;
    gDirectory = tmpDir;
}

void CEvent::Loop(int maxEntry)
{
    int nEvent = maxEntry;
    if (maxEntry == -1) nEvent = wcsimT->GetEntries();
    for (int entry=0; entry<nEvent; entry++) {
        wcsimT->GetEntry(entry);

        eventNumber = entry+1;
        nTrigger    = rawEvent->GetNumberOfEvents();
        nTrack      = 0;
        nPMT        = 0;
        nHit        = 0;

        for (int trigIdx=0; trigIdx<nTrigger; trigIdx++) {
            WCSimRootTrigger *rawTrigger = rawEvent->GetTrigger(trigIdx);

            ProcessTracks(rawTrigger, nTrack);
            nTrack += rawTrigger->GetNtrack();

            // raw hits info are all in the first trigger. (what??)
            if (trigIdx!=0) continue;
            ProcessHits(rawTrigger, nHit);
            nPMT += rawTrigger->GetNcherenkovhits();
        }

        DetermineDecayMode();
        // if (mode<10)
            PrintInfo();
        outputTree->Fill();
    }
    SaveOutput();
}

void CEvent::ProcessTracks(WCSimRootTrigger* trigger, int currentTracks)
{
    int ntrack = trigger->GetNtrack();
    // cout << ntrack << endl;

    for (int i=0; i<ntrack; i++) {
        WCSimRootTrack* track = (WCSimRootTrack*)trigger->GetTracks()->At(i);
        int idx = i+currentTracks;
        track_pdg[idx]    = track->GetIpnu();
        track_parent[idx] = track->GetParenttype();
        track_t[idx]      = track->GetTime();
        track_ee[idx]     = track->GetE();
        track_e[idx]      = track->GetE() - track->GetM();
        track_x[idx]      = track->GetStart(0);
        track_y[idx]      = track->GetStart(1);
        track_z[idx]      = track->GetStart(2);
        track_dx[idx]     = track->GetDir(0);
        track_dy[idx]     = track->GetDir(1);
        track_dz[idx]     = track->GetDir(2);
    }
}

void CEvent::ProcessHits(WCSimRootTrigger* trigger, int currentHits)
{
    int npmt = trigger->GetNcherenkovhits();
    // cout << nhit << endl;
    for (int i=0; i<npmt; i++) {
        WCSimRootCherenkovHit* hit = (WCSimRootCherenkovHit*)trigger->GetCherenkovHits()->At(i);
        int hitIdx = hit->GetTotalPe(0);
        int pmtPEs = hit->GetTotalPe(1);
        // cout << hitIdx << ", " << pmtPEs << endl;
        for (int j=0; j<pmtPEs; j++) {
            WCSimRootCherenkovHitTime* hittime = (WCSimRootCherenkovHitTime*)trigger->GetCherenkovHitTimes()->At(hitIdx+j);
            int idx = hitIdx + j + currentHits;
            hit_t[idx] = hittime->GetTruetime();
            hit_wl[idx] = hittime->GetWavelength();
            hit_pmtID[idx] = hit->GetTubeID()-1;
            hit_tc[idx] = VertexCorrectedTime(idx);

            nHit++;
        }
    }

}

float CEvent::VertexCorrectedTime(int idx)
{
    // assuming we can reconstruct the vertex ...
    int tubeIdx  = hit_pmtID[idx];
    float tube_x = pmt_x[tubeIdx];
    float tube_y = pmt_y[tubeIdx];
    float tube_z = pmt_z[tubeIdx];

    TVector3 vtx3(track_x[2], track_y[2], track_z[2]);
    TVector3 hit3(tube_x, tube_y, tube_z);
    float rindex = 1.3492;
    return hit_t[idx] - (vtx3-hit3).Mag()/299792458.*rindex*1.e7; // position in cm
}

void CEvent::Reset()
{
}

void CEvent::DetermineDecayMode()
{
    if (nTrack <= 2) { mode = -2; } // unphyiscal
    else if (track_pdg[2] != 321) {  // not K+ primary
        mode = -1; 
        if (nTrack == 6) {
            if (track_pdg[2] == -13) { mode = -11; } // mu+ bg simulation
        }
    } 
    else {
        if (nTrack == 8) {
            if (track_pdg[3] == 14 && track_pdg[4] == -13) { mode = 11; }
            else if (track_pdg[3] == 111 && track_pdg[4] == 12 && track_pdg[7] == -11) { mode = 41; }
            else { mode = 1; } // unknown 8 tracks
        }
        else if (nTrack == 11) {
            if (track_pdg[3] == 111 && track_pdg[4] == 14) { mode = 51; }
            else { mode = 2; } // unknown 11 tracks
        }
        else if (nTrack == 12) {
            if (track_pdg[3] == 111 && track_pdg[6] == 211) { mode = 21; }
            else { mode = 3; } // unknown 12 tracks
        }
        else if (nTrack == 13) {
            mode = 22; // pi0 decays to e+e-gamma
        }
        else if (nTrack == 10) {
            mode = 23; // pi+ changes to pi0??
        }
        else if (nTrack == 15) {
            mode = 61;
            // if (track_pdg[3] == 111 && track_pdg[4] == 111 && track_pdg[9] == 211) { mode = 61; }
            // if (track_pdg[3] == 211 && track_pdg[4] == 111 && track_pdg[5] == 111) { mode = 61; }
            // else { mode = 4; } // unknown 15 tracks
        }
        else if (nTrack == 16) {
            int pi_plus = 0;
            int pi_minus = 0;
            for (int i=0; i<nTrack; i++) {
                if (track_pdg[i] == 211) { pi_plus++; }
                else if (track_pdg[i] == -211) { pi_minus++; }
            }
            if (pi_minus==1 && pi_plus==2) { mode = 31; }
            else { mode = 5; } // unknown 16 tracks
        }
        else {
            mode = 0; // unknown K+ decay
        }
    }
}

void CEvent::PrintInfo()
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
