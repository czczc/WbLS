#ifndef BEAM_TREE_H
#define BEAM_TREE_H

#include <vector>
#include <string>

class TTree;
class TFile;

class BeamTree {
public:
    BeamTree(){};
    BeamTree(int startRun, int endRun, std::string desc="", bool isLS=false);
    virtual ~BeamTree();
    
    void LoadData();
    void InitTree();
    void Process();
    void PrintInfo();
    
    TTree* fBeamTree;
    int fStartRun;
    int fEndRun;
    std::string fDescription;
    std::vector<std::string> fFilenames;
    TFile* fOutput;
    std::string fOutputName;
    float fGain_T1;
    float fGain_T2;
    
    // tree variables;
    Float_t b_nPE_T1;
    Float_t b_nPE_T2;
    Float_t b_nPE_H1;
    Float_t b_nPE_H2;
    Float_t b_nPE_H3;
    Float_t b_nPE_VC;
    
    Float_t b_t_T1;
    Float_t b_t_T2;
    Float_t b_t_H1;
    Float_t b_t_H2;
    Float_t b_t_H3;
    Float_t b_t_VC;
    
};

#endif