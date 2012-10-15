#include "BeamTree.h"
#include "WblsDaqEvent.h"
#include "WFAnalyzer.h"

#include "TTree.h"
#include "TFile.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>

using namespace std;

BeamTree::BeamTree(int startRun, int endRun, string desc, bool isLS)
{
    fStartRun = startRun;
    fEndRun = endRun;
    if (desc.empty()) {
        char str[100];
        sprintf(str, "%d-%d", startRun, endRun);
        fDescription = str;
    }
    else {
        fDescription = desc;
    }
    fGain_T1 = isLS ? 0.1 : 1.0;
    fGain_T2 = isLS ? 0.2 : 1.0;
    LoadData();
    InitTree();
    // Process();
}

BeamTree::~BeamTree()
{
    if (fOutput) { delete fOutput; }
}

void BeamTree::LoadData()
{
    for (int i=fStartRun; i!=fEndRun; i++){
        string filename;
        char str_run[100];
        sprintf(str_run, "%d", i);
        filename = filename + "../data/beamrun/rootoutputfile" + str_run + ".root";
        fFilenames.push_back(filename);
    }
}

void BeamTree::InitTree()
{
    fOutputName = fOutputName + "../data/reduced/" + fDescription + ".root";
    fBeamTree = new TTree("beamTree", "Stats of Beam Events");
    fBeamTree->Branch("nPE_T1", &b_nPE_T1);
    fBeamTree->Branch("nPE_T2", &b_nPE_T2);
    fBeamTree->Branch("nPE_H1", &b_nPE_H1);
    fBeamTree->Branch("nPE_H2", &b_nPE_H2);
    fBeamTree->Branch("nPE_H3", &b_nPE_H3);
    fBeamTree->Branch("nPE_VC", &b_nPE_VC);
    
    fBeamTree->Branch("t_T1", &b_t_T1);
    fBeamTree->Branch("t_T2", &b_t_T2);
    fBeamTree->Branch("t_H1", &b_t_H1);
    fBeamTree->Branch("t_H2", &b_t_H2);
    fBeamTree->Branch("t_H3", &b_t_H3);
    fBeamTree->Branch("t_VC", &b_t_VC);
}

void BeamTree::Process()
{
    WblsDaq::Spinner spinner(fFilenames);
    Long64_t nevents = spinner.entries();
    cout << "total entries: " << nevents << endl;
    // spinner.PrintRunInfo();
    // WblsDaq::Header* f_header = spinner.header();
    // WblsDaq::Footer* f_footer = spinner.footer();
    // else { f_run_number = f_header->run_number; }
    // f_runtype = f_header->runtype;
    // f_sampletype = f_header->sampletype;
    // f_runstart = f_header->runstart;
    // f_runstop = f_footer->runstop;
    
    for (int i=0; i < nevents; ++i) {
        WblsDaq::Event* evt = spinner.event(i);    
        if (!evt) {
            cerr << "Failed to get event at entry #" << i << endl;
            return;
        }
        
        // process waveform
        WFAnalyzer wf(evt);
        wf.Process();
        
        b_nPE_T1 = wf.fCharge_Tub[0] / fGain_T1;
        b_nPE_T2 = wf.fCharge_Tub[1] / fGain_T2;
        b_nPE_H1 = wf.fCharge_Counter[0][0];
        b_nPE_H3 = wf.fCharge_Counter[0][1];
        b_nPE_H2 = wf.fCharge_Counter[1][0];
        b_nPE_VC = wf.fCharge_Counter[1][1];
        
        b_t_T1 = wf.fTDC_Tub[0];
        b_t_T2 = wf.fTDC_Tub[1];
        b_t_H1 = wf.fTDC_Counter[0][0];
        b_t_H3 = wf.fTDC_Counter[0][1];
        b_t_H2 = wf.fTDC_Counter[1][0];
        b_t_VC = wf.fTDC_Counter[1][1];
        
        // cout << b_nPE_T1 << endl;
        fBeamTree->Fill();
        // cout << "proceessed " << i << endl;
    }
    
    fOutput = new TFile(fOutputName.c_str(), "recreate");
    fBeamTree->Write();
    fOutput->Close();
    cout << "reduced tree saved to " << fOutputName << endl; 
}

void BeamTree::PrintInfo()
{
    for (size_t i=0; i<fFilenames.size(); i++) {
        cout << fFilenames.at(i) << endl;
    }
}