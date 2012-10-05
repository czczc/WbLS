#ifndef WFANALYZER_H
#define WFANALYZER_H

#include "WblsDaqEvent.h"

class WFAnalyzer {
public:
    WFAnalyzer(WblsDaq::Event* evt);
    virtual ~WFAnalyzer();
    
    void Process();
    void ProcessTub(int i); 
    void ProcessCounter(int i); 
    
    void Test();
    
    WblsDaq::Event* fEvent;
    double fBaseLine_Tub[2];
    // double fMaxADC_Tub[2];
    double fCharge_Tub[2];
    double fTDC_Tub[2];
    
    double fBaseLine_Counter[2];
    double fCharge_Counter[2][2]; // counter i, pulse j
    double fTDC_Counter[2][2];
};

#endif