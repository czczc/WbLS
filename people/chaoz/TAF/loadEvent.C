{
    gROOT->Reset();
    TString prefix = ".";
    TString include = ".include ";
    TString load = ".L ";
    TString WCSIMROOT = "/Users/chaozhang/Projects/LBNE/WbLS/software/WCSim_WbLS";
    
    gSystem->Load(WCSIMROOT + "/libWCSimRoot");    
    
    gROOT->ProcessLine( include + WCSIMROOT + "/include" ); 
    gROOT->ProcessLine( include + prefix ); 
    
    
    gROOT->ProcessLine( load + prefix + "/CEvent.cc+" );
    
    CEvent ev("../gen/wcsim.root", "events.root");
    ev.Loop();
    
}