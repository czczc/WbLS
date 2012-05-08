void print_stats(int i)
{
    gROOT->Reset();
    TString prefix = ".";
    TString include = ".include ";
    TString load = ".L ";

    gROOT->ProcessLine( include + prefix );
    gROOT->ProcessLine( load + prefix + "/CEventStats.cc+" );


    CEventStats ev("../tree/kaon_105MeV_py90/tree001.root", 
        "test.root");
    ev.PrintInfo(i);

}