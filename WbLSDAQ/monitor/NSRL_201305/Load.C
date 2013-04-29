{
    gROOT->ProcessLine(".include ./");
    
    gROOT->ProcessLine(".L WblsDaqEvent.cpp+");
    gROOT->ProcessLine(".L WFAnalyzer.cc+");
    gROOT->ProcessLine(".L MainWindow.cc+");
    
    // gROOT->ProcessLine(".L fakeDAQ.C+");
    
    // gROOT->ProcessLine(".L WblsDaqMonitor.cpp+");
}