{
    gROOT->Reset();
    TString prefix = ".";
    TString include = ".include ";
    TString load = ".L ";

    gROOT->ProcessLine( include + prefix );
    gROOT->ProcessLine( load + prefix + "/CEventStats.cc+" );

    // CEvent ev("../gen/wcsim100_py90.root", "../viewer/data/events100_py90.root");
    // CEvent ev("../gen/wcsim100_py180.root", "../viewer/data/events100_py180.root");
    // CEvent ev("../gen/wcsim100_py270.root", "../viewer/data/events100_py270.root");
    CEventStats ev("events100.root", "stats100.root");
    ev.Loop();

}