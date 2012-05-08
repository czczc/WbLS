#ifndef CEVENTSTATS_H
#define CEVENTSTATS_H

#include "TObject.h"

#include <map>
#include <string>
#include "TTree.h"

class TFile;

class CEventStats {
public:
    TFile *rootFile;
    TTree *simTree;
    TTree *geoTree;

    TFile *outputFile;
    TTree *outputTree;
    
    std::map<int, std::string> decayMode;

    enum State {
        MAXTRACK = 100,
        MAXPMT = 20000,
        MAXHIT = 50000,
    };

    float cylRadius;
    float cylLength;
    int totalPMTs;
    float pmt_x[MAXPMT];
    float pmt_y[MAXPMT];
    float pmt_z[MAXPMT];

    int eventNumber;
    int nTrigger;
    int mode;

    int nTrack;
    int track_pdg[MAXTRACK];
    int track_parent[MAXTRACK];
    float track_t[MAXTRACK]; // time [ns]
    float track_e[MAXTRACK]; // KE [ns]
    float track_ee[MAXTRACK]; // Total Energy [ns]
    float track_x[MAXTRACK];
    float track_y[MAXTRACK];
    float track_z[MAXTRACK];
    float track_dx[MAXTRACK];
    float track_dy[MAXTRACK];
    float track_dz[MAXTRACK];

    int nPMT;  // # hit PMTs
    int nHit;  // # total hits == total PEs
    int hit_pmtID[MAXHIT];
    float hit_t[MAXHIT];
    float hit_tc[MAXHIT];
    float hit_wl[MAXHIT];

    // stats variables
    float kaon_decay_t; // ns
    
    // ch1: kaon -> mu + nu
    float ch1_muon_e; 
    float ch1_michele_t; 
    float ch1_michele_e;
    float ch1_kaon_npe; // with clean time separation
    float ch1_muon_npe; // with clean time separation
    float ch1_muon_npe_recon; // with clean time separation
    // float ch1_prompt_npe; // within 100 ns
    // float ch1_rising_dt; // rising time of the hittime shape
    // float ch1_early_charge_ratio; // early charge / all charge (in the first 100ns)
    
    float prompt_npe; // within 100 ns
    float rising_dt; // rising time of the hittime shape
    float rising_dt_vis; // rising time of the hittime shape, adding smear
    float early_charge_ratio; // early charge / all charge (in the first 100ns)
    
    float bg1_muon_e;
    float bg1_muon_npe_recon;
    float bg1_michele_t;
    
    CEventStats();
    CEventStats(const char* filename, const char* outname);
    virtual ~CEventStats();

    //  methods
    void Loop(int maxEntry=-1);
    void SaveOutput();
    void SetDefault();
    void InspectShape();
    void InspectVisShape();
    
    TTree* Tree() { return simTree; }
    void GetEntry(int i) { simTree->GetEntry(i); }
    void PrintInfo();
    void PrintInfo(int i);
    // std::vector< std::vector<float> > FindRing(int track_id, int resolution = 200);
    // std::vector<float> ProjectToTankSurface_C(float *vtx, float *dir);
    float GetRandomMuonPE(float e);
    
private:
    void InitBranchAddress();
    void InitDecayMode();
    void InitOutput(const char* filename);
    
    // void InitGeometry();

};

#endif
