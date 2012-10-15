{
    gROOT->ProcessLine(".include ../event");
    
    gROOT->ProcessLine(".L ../event/WblsDaqEvent.cpp+");
    gROOT->ProcessLine(".L ../event/WFAnalyzer.cc+");
        
    gROOT->ProcessLine(".L BeamTree.cc+");
    
}