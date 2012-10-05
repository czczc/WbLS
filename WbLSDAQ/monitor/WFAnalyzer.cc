#include "WFAnalyzer.h"

#include "TMath.h"
#include "TGraph.h"
#include "TCanvas.h"
#include <iostream>
#include <vector>
using namespace std;

WFAnalyzer::WFAnalyzer(WblsDaq::Event* evt)
{
    fEvent = evt;
}

WFAnalyzer::~WFAnalyzer()
{
    
}

void WFAnalyzer::Process()
{
    ProcessTub(1);
    ProcessTub(2);
    ProcessCounter(1);
    ProcessCounter(2);
}

void WFAnalyzer::ProcessTub(int tubNo)
{
    WblsDaq::FADCBin* trace = 0;
    
    if (tubNo==1) {
        trace = fEvent->ch0;
    }
    else if (tubNo==2) {
        trace = fEvent->ch1;
    }
    else {
        cout << "warning: no such tub!" << endl;
    }
    
    // calculate baseline
    const int NBINS_BASELINE = 200;
    double bl = TMath::Mean(NBINS_BASELINE, trace+100);
    double bl_tolerance = 4*TMath::RMS(NBINS_BASELINE, trace);
    int nBins_used = NBINS_BASELINE;
    for (int i=0; i<NBINS_BASELINE; i++) {
        // remove points out of 4 sigma
        if (TMath::Abs(trace[i]-bl) > bl_tolerance) {
            bl = (bl*nBins_used - trace[i])/(nBins_used-1);
            nBins_used--;
            // cout << "removed one bin" << endl;
        }
    }
    fBaseLine_Tub[tubNo-1] = bl;

    // remove baseline, invert to positive trace
    double cleanTrace[WblsDaq::NFADCBins];
    double xPoints[WblsDaq::NFADCBins];
    
    for (unsigned i=0; i<WblsDaq::NFADCBins; i++) {
        cleanTrace[i] = bl - trace[i];
        xPoints[i] = i;
        // cout << cleanTrace[i] << endl;
    }
    
    // find max pulse
    vector<double> pulses;
    vector<double> tdcs;
    double charge = 0;
    double tdc = 0;
    bool foundPulse = false;
    const int THRESHOLD = 10; // threshold for tdc start
    for (unsigned i=0; i<WblsDaq::NFADCBins-1; i++) {
        if (cleanTrace[i]>0 && cleanTrace[i+1]>0) {
            foundPulse = true;
            charge += cleanTrace[i];
            if(cleanTrace[i]<THRESHOLD && cleanTrace[i+1]>THRESHOLD && tdc<1) tdc = i;
        }
        else {
            if(foundPulse) {
                pulses.push_back(charge);
                tdcs.push_back(tdc);
            }
            charge = 0;
            tdc = 0;
            foundPulse = false;
        }
    }
    double maxCharge = 0;
    double pulseTdc = 0;
    for (unsigned i=0; i<pulses.size(); i++) {
        if (pulses[i] > maxCharge) {
            maxCharge = pulses[i];
            pulseTdc = tdcs[i];
        }
    }
    fCharge_Tub[tubNo-1] = maxCharge; 
       fTDC_Tub[tubNo-1] = pulseTdc;
     
    // draw trace
    // TCanvas *c1 = new TCanvas();
    // TGraph *g = new TGraph(WblsDaq::NFADCBins, xPoints, cleanTrace);
    // g->Draw("AL");
}

void WFAnalyzer::ProcessCounter(int counterNo)
{
    WblsDaq::FADCBin* trace = 0;
    
    if (counterNo==1) {
        trace = fEvent->ch2;
    }
    else if (counterNo==2) {
        trace = fEvent->ch3;
    }
    else {
        cout << "warning: no such counter!" << endl;
    }
    
    // calculate baseline
    const int NBINS_BASELINE = 200;
    double bl = TMath::Mean(NBINS_BASELINE, trace+100);
    double bl_tolerance = 4*TMath::RMS(NBINS_BASELINE, trace);
    int nBins_used = NBINS_BASELINE;
    for (int i=0; i<NBINS_BASELINE; i++) {
        // remove points out of 4 sigma
        if (TMath::Abs(trace[i]-bl) > bl_tolerance) {
            bl = (bl*nBins_used - trace[i])/(nBins_used-1);
            nBins_used--;
            // cout << "removed one bin" << endl;
        }
    }
    fBaseLine_Counter[counterNo-1] = bl;

    // remove baseline, invert to positive trace
    double cleanTrace[WblsDaq::NFADCBins];
    double xPoints[WblsDaq::NFADCBins];
    
    for (unsigned i=0; i<WblsDaq::NFADCBins; i++) {
        cleanTrace[i] = bl - trace[i];
        xPoints[i] = i;
        // cout << cleanTrace[i] << endl;
    }
    
    // calculate pulse in fixed window
    const unsigned START_PULSE1 = 1200;
    const unsigned END_PULSE1 = 1600;
    const unsigned START_PULSE2 = 1800;
    const unsigned END_PULSE2 = 2200;
    double charge1 = 0;
    double tdc1 = 0;
    double charge2 = 0;
    double tdc2 = 0;
    const int THRESHOLD = 100; // threshold for tdc start
    
    for (unsigned i=0; i<WblsDaq::NFADCBins-1; i++) {
        if(i>=START_PULSE1 && i<=END_PULSE1) {
            charge1 += cleanTrace[i];
            if(cleanTrace[i]<THRESHOLD && cleanTrace[i+1]>THRESHOLD && tdc1<1) tdc1 = i;
            
        }
        else if(i>=START_PULSE2 && i<=END_PULSE2) {
            charge2 += cleanTrace[i];
            if(cleanTrace[i]<THRESHOLD && cleanTrace[i+1]>THRESHOLD && tdc2<1) tdc2 = i;
            
        }    
    }
    fCharge_Counter[counterNo-1][0] = charge1; 
    fCharge_Counter[counterNo-1][1] = charge2; 
    fTDC_Counter[counterNo-1][0] = tdc1; 
    fTDC_Counter[counterNo-1][1] = tdc2; 
      
    // draw trace
    // TCanvas *c1 = new TCanvas();
    // TGraph *g = new TGraph(WblsDaq::NFADCBins, xPoints, cleanTrace);
    // g->Draw("AL");
}


void WFAnalyzer::Test()
{
    cout << fEvent->time << endl;
}