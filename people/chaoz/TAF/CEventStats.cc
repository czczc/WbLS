#include "CEventStats.h"

#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TH1F.h"
#include "TRandom.h"

using namespace std;

CEventStats::CEventStats(){}

CEventStats::CEventStats(const char* filename, const char* outname)
{
    rootFile = new TFile(filename);
    simTree = (TTree*)rootFile->Get("/Event/Sim");
    geoTree = (TTree*)rootFile->Get("/Detector/Geometry");

    InitDecayMode();
    InitBranchAddress();
    // InitGeometry();
    InitOutput(outname);
}

CEventStats::~CEventStats()
{
}

void CEventStats::InitDecayMode()
{
    decayMode[0]  = "Unknown??";
    decayMode[11] = "K^{+} -> #mu^{+} + #nu_{#mu} (63.47%)";           // 63.47%
    decayMode[21] = "K^{+} -> #pi^{+} + #pi^{0} (21.13%)";             // 21.13%
    decayMode[31] = "K^{+} -> #pi^{+} + #pi^{+} + #pi^{-} (5.58%)";    // 5.58%
    decayMode[41] = "K^{+} -> #pi^{0} + e^{+} + #nu_{e} (4.87%)";      // 4.87%
    decayMode[51] = "K^{+} -> #pi^{0} + #mu^{+} + #nu_{#mu} (3.27%)";  // 3.27%
    decayMode[61] = "K^{+} -> #pi^{+} + #pi^{0} + #pi^{0} (1.73%)";    // 1.73%
}

void CEventStats::InitBranchAddress()
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

void CEventStats::InitOutput(const char* filename)
{
    TDirectory* tmpDir = gDirectory;
    outputFile = new TFile(filename, "recreate");
    TDirectory* subDir = outputFile->mkdir("Event");
    subDir->cd();
    outputTree = new TTree("Stats","Event Tree of Stats Info");

    outputTree->Branch("eventNumber"   , &eventNumber, "eventNumber/I");
    outputTree->Branch("mode"          , &mode, "mode/I");
    outputTree->Branch("nTrack"        , &nTrack, "nTrack/I");
    outputTree->Branch("nPMT"          , &nPMT, "nPMT/I");
    outputTree->Branch("nHit"          , &nHit, "nHit/I");
    outputTree->Branch("kaon_decay_t"  , &kaon_decay_t, "kaon_decay_t/F");
    outputTree->Branch("ch1_muon_e"    , &ch1_muon_e, "ch1_muon_e/F");
    outputTree->Branch("ch1_michele_t" , &ch1_michele_t, "ch1_michele_t/F");
    outputTree->Branch("ch1_michele_e" , &ch1_michele_e, "ch1_michele_e/F");
    outputTree->Branch("ch1_kaon_npe"  , &ch1_kaon_npe, "ch1_kaon_npe/F");
    outputTree->Branch("ch1_muon_npe"  , &ch1_muon_npe, "ch1_muon_npe/F");
    outputTree->Branch("ch1_muon_npe_recon"  , &ch1_muon_npe_recon, "ch1_muon_npe_recon/F");
    // outputTree->Branch("ch1_prompt_npe", &ch1_prompt_npe, "ch1_prompt_npe/F");
    // outputTree->Branch("ch1_rising_dt" , &ch1_rising_dt, "ch1_rising_dt/F");
    // outputTree->Branch("ch1_early_charge_ratio" , &ch1_early_charge_ratio, "ch1_early_charge_ratio/F");
    outputTree->Branch("rising_dt" , &rising_dt, "rising_dt/F");
    outputTree->Branch("rising_dt_vis" , &rising_dt_vis, "rising_dt_vis/F");
    outputTree->Branch("early_charge_ratio" , &early_charge_ratio, "early_charge_ratio/F");
    outputTree->Branch("prompt_npe", &prompt_npe, "prompt_npe/F");

    outputTree->Branch("bg1_muon_e"    , &bg1_muon_e, "bg1_muon_e/F");
    outputTree->Branch("bg1_muon_npe_recon", &bg1_muon_npe_recon, "bg1_muon_npe_recon/F");
    outputTree->Branch("bg1_michele_t"    , &bg1_michele_t, "bg1_michele_t/F");

    gDirectory = tmpDir;
}

void CEventStats::Loop(int maxEntry)
{
    int nEvent = maxEntry;
    if (maxEntry == -1) nEvent = simTree->GetEntries();
    for (int entry=0; entry<nEvent; entry++) {
        simTree->GetEntry(entry);
        SetDefault();
        kaon_decay_t = nTrack>3 ? track_t[3] : 0;
        prompt_npe = 0;
        for (int i=0; i<nHit; i++) {
            if (hit_tc[i] < 100) { prompt_npe++; }
        }
        InspectShape();
        InspectVisShape();
        
        // channel one
        if (mode == 11) {
            for (int i=3; i<nTrack; i++) {
                if (track_pdg[i] == -13) { 
                    ch1_muon_e = track_e[i]; 
                    ch1_muon_npe_recon = GetRandomMuonPE(ch1_muon_e);
                }
                else if (track_pdg[i] == -11) {
                    ch1_michele_t = track_t[i];
                    ch1_michele_e = track_e[i];
                    break;
                }
            }
            ch1_kaon_npe = ch1_muon_npe = 0;
            for (int i=0; i<nHit; i++) {
                if (kaon_decay_t > 20 && hit_tc[i] < 20) { ch1_kaon_npe++; }
                if (kaon_decay_t > 10 && ch1_michele_t >100
                    && hit_tc[i]>kaon_decay_t-1 && hit_tc[i]<100) {
                    ch1_muon_npe++;
                }
                // if (hit_tc[i] < 100) { ch1_prompt_npe++; }
            }
            // InspectShape();
        }
        
        // background one
        if (mode == -11) {
            bg1_muon_e = track_e[2];
            bg1_muon_npe_recon = GetRandomMuonPE(bg1_muon_e);
            bg1_michele_t = track_t[3];
        }
        PrintInfo();
        outputTree->Fill();
    }
    SaveOutput();
}

// returns the light yield for a muon energy, with gaus distribution
float CEventStats::GetRandomMuonPE(float e)
{
    // 4.95 PE / MeV for muon
    // 10% at some energy
    float ref_e = 100;  // MeV
    return 4.95*gRandom->Gaus(e, ref_e*sqrt(ref_e/1000.)*0.1);
}


void CEventStats::InspectShape()
{
    int nBins = 100;
    TH1F h("h", "h", nBins, 0, nBins);
    for (int i=0; i<nHit; i++) { h.Fill(hit_tc[i]); }
    float maxPE = h.GetMaximum();
    // float maxPE_bin = h.GetMaximumBin();
    float pe_15 = maxPE * 0.15;
    float pe_85 = maxPE * 0.85;
    float t_15 = 0;
    float t_85 = 0;
    int i = 0;
    float pe = 0;
    for (; i<nBins; i++) {
        pe = h.GetBinContent(i);
        if ( pe > pe_15) { break; }
    }
    t_15 = i;
    for (; i<nBins; i++) {
        pe = h.GetBinContent(i);
        if ( pe > pe_85) { break; }
    }
    t_85 = i;
    rising_dt = t_85 - t_15;
    early_charge_ratio = h.Integral(t_15, t_85) / h.Integral();
}

void CEventStats::InspectVisShape()
{
    int nBins = 110;
    TH1F h("h", "h", nBins, -10, nBins-10);
    float t_res = 2.2;  // ns
    for (int i=0; i<nHit; i++) { h.Fill( gRandom->Gaus(hit_tc[i], t_res) ); }
    float maxPE = h.GetMaximum();
    // float maxPE_bin = h.GetMaximumBin();
    float pe_15 = maxPE * 0.15;
    float pe_85 = maxPE * 0.85;
    float t_15 = 0;
    float t_85 = 0;
    int i = 0;
    float pe = 0;
    for (; i<nBins; i++) {
        pe = h.GetBinContent(i);
        if ( pe > pe_15) { break; }
    }
    t_15 = i;
    for (; i<nBins; i++) {
        pe = h.GetBinContent(i);
        if ( pe > pe_85) { break; }
    }
    t_85 = i;
    rising_dt_vis = t_85 - t_15;
}

void CEventStats::SetDefault()
{
    ch1_muon_e = bg1_muon_e = -1; 
    ch1_michele_t = bg1_michele_t = -1; 
    ch1_michele_e = -1;
    rising_dt = rising_dt_vis = -1;
    early_charge_ratio = -1;
    ch1_kaon_npe = ch1_muon_npe = ch1_muon_npe_recon = bg1_muon_npe_recon = -1;
}

void CEventStats::SaveOutput()
{
    TDirectory* tmpDir = gDirectory;
    outputFile->cd("/Event");
    outputTree->Write();
    outputTree = 0;

    gDirectory = tmpDir;
}

void CEventStats::PrintInfo()
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
             // << " | Dir: (" << track_dx[i] << ", " << track_dy[i] << ", " << track_dz[i] << ")"
             << "\n";
    }
    cout << endl;
}

void CEventStats::PrintInfo(int i)
{
    GetEntry(i);
    PrintInfo();
}
