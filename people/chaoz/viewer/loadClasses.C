{
    TString prefix = ".";
    TString include = ".include ";
    TString load = ".L ";

    gROOT->ProcessLine( include + prefix );

    gROOT->ProcessLine( load + prefix + "/MCEvent.cc+" );
    gROOT->ProcessLine( load + prefix + "/ViewWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/ControlWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/MainWindow.cc+" );

}