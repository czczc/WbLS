#ifndef MCEVENT_H
#define MCEVENT_H

#include "TObject.h"

#include <map>
#include <string>
#include "TTree.h"

class TFile;

class MCEvent {
public:
    TFile *rootFile;
    TTree *simTree;
    TTree *geoTree;

    map<int, string> decayMode;

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


    MCEvent();
    MCEvent(const char* filename);
    virtual ~MCEvent();

    //  methods
    TTree* Tree() { return simTree; }
    void GetEntry(int i) { simTree->GetEntry(i); }
    void PrintInfo();
    std::vector< std::vector<float> > FindRing(int track_id, int resolution = 200);
    std::vector<float> ProjectToTankSurface_C(float *vtx, float *dir);

private:
    void InitBranchAddress();
    void InitDecayMode();
    void InitGeometry();

};

#endif
