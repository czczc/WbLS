#ifndef _CEVENT_H
#define _CEVENT_H

#include "TObject.h"

class TFile;
class TTree;
class WCSimRootEvent;
class WCSimRootTrigger;

class CEvent {
public:    
    TFile *rootFile;
    TTree *wcsimT;
    WCSimRootEvent *rawEvent;
    TFile *outputFile;
    TTree *outputTree;
    
    enum State {
        MAXTRACK = 100,
        MAXPMT = 20000,
        MAXHIT = 50000,
    };
    
    float pmt_pos[MAXPMT][3];
    
    int eventNumber;
    int nTrigger;
    
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
    
	
    CEvent();
    CEvent(const char* filename, const char* outname);
    virtual ~CEvent();
    
    //  methods
    void InitOutput(const char* filename);
    void SaveOutput();
    void InitPMTGeom();
    
    void Loop(int maxEntry=-1);
    void ProcessTracks(WCSimRootTrigger* trigger, int currentTracks);
    void ProcessHits(WCSimRootTrigger* trigger, int currentHits);
    void Reset();
    virtual void PrintInfo();
    
private:    
    float VertexCorrectedTime(int idx);
    
};

#endif
