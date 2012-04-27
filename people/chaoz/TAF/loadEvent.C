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

    // CEvent ev("../gen/wcsim100_py90.root", "../viewer/data/events100_py90.root");
    // CEvent ev("../gen/wcsim100_py180.root", "../viewer/data/events100_py180.root");
    // CEvent ev("../gen/wcsim100_py270.root", "../viewer/data/events100_py270.root");
    CEvent ev("../gen/muon_150MeV_py90.root", "../viewer/data/events_muon_150MeV_py90.root");
    ev.Loop();

}