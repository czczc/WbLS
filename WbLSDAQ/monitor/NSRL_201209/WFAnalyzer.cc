#include "WFAnalyzer.h"

#include "TMath.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include <iostream>
#include <vector>
using namespace std;

WFAnalyzer::WFAnalyzer(WblsDaq::Event* evt)
{
    fEvent = evt;
    SetPulseWindow();
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

void WFAnalyzer::SetPulseWindow()
{
    double ref = 1250 + 160 - 65; // fadc bin of tub1 peak (trigger in the center and tub1 is 65ns before trigger);
    double prePulse_tub = 100; // # of bins to include before the peak for tub (can be longer window)
    double postPulse_tub = 200; // # of bins to include after the peak for tub (can be longer window)
    double prePulse_counter = 50; // # of bins to include before the peak
    double postPulse_counter = 100; // # of bins to include after the peak
    
    // ____________________________
    //       |Trigger
    // ____________________________
    // |T1
    // ____________________________
    // |T2
    // ____________________________
    //    |H1      |H3
    // ____________________________
    //    |H2      |VC
    
    double T2_to_T1 = 0;
    double H1_to_T1 = 46;
    double H3_to_H1 = 149;
    double H2_to_T1 = 46;
    double VC_to_H2 = 149;
    
    // tub 1
    g_tStartTub[0] = ref - prePulse_tub; 
     g_tStopTub[0] = ref + postPulse_tub;
    
    // tub 2
    g_tStartTub[1] = ref + T2_to_T1 - prePulse_tub; 
     g_tStopTub[1] = ref + T2_to_T1 + postPulse_tub;
    
    // counter1 pulse1
    g_tStartCounterPulse[0][0] = ref + H1_to_T1 - prePulse_counter; 
     g_tStopCounterPulse[0][0] = ref + H1_to_T1 + postPulse_counter; 
     
    // counter1 pulse2
    g_tStartCounterPulse[0][1] = ref + H1_to_T1 + H3_to_H1 - prePulse_counter; 
     g_tStopCounterPulse[0][1] = ref + H1_to_T1 + H3_to_H1 + postPulse_counter;
    
    // counter2 pulse1
    g_tStartCounterPulse[1][0] = ref + H2_to_T1 - prePulse_counter; 
     g_tStopCounterPulse[1][0] = ref + H2_to_T1 + postPulse_counter; 
     
    // counter2 pulse2
    g_tStartCounterPulse[1][1] = ref + H2_to_T1 + VC_to_H2 - prePulse_counter; 
     g_tStopCounterPulse[1][1] = ref + H2_to_T1 + VC_to_H2 + postPulse_counter; 
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
    
    // calculate baseline, the smaller value of average of 200 bins from (100, 300), (300, 500)
    const int NBINS_BASELINE = 200;
    double bl = TMath::Mean(NBINS_BASELINE, trace+100);
    double bl_tolerance = 4*TMath::RMS(NBINS_BASELINE, trace+100);
    int nBins_used = NBINS_BASELINE;
    for (int i=100; i<NBINS_BASELINE+100; i++) {
        // remove points out of 4 sigma
        if (TMath::Abs(trace[i]-bl) > bl_tolerance) {
            bl = (bl*nBins_used - trace[i])/(nBins_used-1);
            nBins_used--;
            // cout << "removed one bin" << endl;
        }
    }
    double bl2 = TMath::Mean(NBINS_BASELINE, trace+100+NBINS_BASELINE);
    double bl2_tolerance = 4*TMath::RMS(NBINS_BASELINE, trace+100+NBINS_BASELINE);
    nBins_used = NBINS_BASELINE;
    for (int i=100+NBINS_BASELINE; i<NBINS_BASELINE+100+NBINS_BASELINE; i++) {
        // remove points out of 4 sigma
        if (TMath::Abs(trace[i]-bl2) > bl2_tolerance) {
            bl2 = (bl2*nBins_used - trace[i])/(nBins_used-1);
            nBins_used--;
            // cout << "removed one bin" << endl;
        }
    }
    fBaseLine_Tub[tubNo-1] = bl < bl2 ? bl: bl2;
    // cout << bl << ", " << bl2 << endl;

    // remove baseline, invert to positive trace
    double cleanTrace[WblsDaq::NFADCBins];
    double xPoints[WblsDaq::NFADCBins];
    
    for (unsigned i=0; i<WblsDaq::NFADCBins; i++) {
        cleanTrace[i] = bl - trace[i];
        xPoints[i] = i;
        // cout << cleanTrace[i] << endl;
    }
    
    // // calculate pulse with pulse finding of continuous area
    // vector<double> pulses;
    // vector<double> tdcs;
    // double charge = 0;
    // double tdc = 0;
    // bool foundPulse = false;
    // const int THRESHOLD = 10; // threshold for tdc start
    // for (unsigned i=0; i<WblsDaq::NFADCBins-1; i++) {
    //     if (cleanTrace[i]>0 && cleanTrace[i+1]>0) {
    //         foundPulse = true;
    //         charge += cleanTrace[i];
    //         if(cleanTrace[i]<THRESHOLD && cleanTrace[i+1]>THRESHOLD && tdc<1) tdc = i;
    //     }
    //     else {
    //         if(foundPulse) {
    //             pulses.push_back(charge);
    //             tdcs.push_back(tdc);
    //         }
    //         charge = 0;
    //         tdc = 0;
    //         foundPulse = false;
    //     }
    // }
    // double maxCharge = 0;
    // double pulseTdc = 0;
    // for (unsigned i=0; i<pulses.size(); i++) {
    //     if (pulses[i] > maxCharge) {
    //         maxCharge = pulses[i];
    //         pulseTdc = tdcs[i];
    //     }
    // }
    // fCharge_Tub[tubNo-1] = maxCharge; 
    //    fTDC_Tub[tubNo-1] = pulseTdc;
    
    // calculate pulse in fixed window
    double charge = 0;
    double tdc = 0;
    const int THRESHOLD = 100; // threshold for tdc start
    
    for (unsigned i=0; i<WblsDaq::NFADCBins-1; i++) {
        if(i>=g_tStartTub[tubNo-1] && i<=g_tStopTub[tubNo-1]) {
            charge += cleanTrace[i];
            if(cleanTrace[i]<THRESHOLD && cleanTrace[i+1]>THRESHOLD && tdc<1) tdc = i;
            
        }
    }
    fCharge_Tub[tubNo-1] = charge; 
       fTDC_Tub[tubNo-1] = tdc;
     
    // draw trace
    if (tubNo==1) {
        TGraph *g = new TGraph(WblsDaq::NFADCBins, xPoints, cleanTrace);
        g->Draw("AL");
        g->GetXaxis()->SetRangeUser(500, 600);
    }
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
    
    // calculate baseline, the smaller value of average of 200 bins from (100, 300), (300, 500)
    const int NBINS_BASELINE = 200;
    double bl = TMath::Mean(NBINS_BASELINE, trace+100);
    double bl_tolerance = 4*TMath::RMS(NBINS_BASELINE, trace+100);
    int nBins_used = NBINS_BASELINE;
    for (int i=100; i<NBINS_BASELINE+100; i++) {
        // remove points out of 4 sigma
        if (TMath::Abs(trace[i]-bl) > bl_tolerance) {
            bl = (bl*nBins_used - trace[i])/(nBins_used-1);
            nBins_used--;
            // cout << "removed one bin" << endl;
        }
    }
    double bl2 = TMath::Mean(NBINS_BASELINE, trace+100+NBINS_BASELINE);
    double bl2_tolerance = 4*TMath::RMS(NBINS_BASELINE, trace+100+NBINS_BASELINE);
    nBins_used = NBINS_BASELINE;
    for (int i=100+NBINS_BASELINE; i<NBINS_BASELINE+100+NBINS_BASELINE; i++) {
        // remove points out of 4 sigma
        if (TMath::Abs(trace[i]-bl2) > bl2_tolerance) {
            bl2 = (bl2*nBins_used - trace[i])/(nBins_used-1);
            nBins_used--;
            // cout << "removed one bin" << endl;
        }
    }
    fBaseLine_Tub[counterNo-1] = bl < bl2 ? bl: bl2;

    // remove baseline, invert to positive trace
    double cleanTrace[WblsDaq::NFADCBins];
    double xPoints[WblsDaq::NFADCBins];
    
    for (unsigned i=0; i<WblsDaq::NFADCBins; i++) {
        cleanTrace[i] = bl - trace[i];
        xPoints[i] = i;
        // cout << cleanTrace[i] << endl;
    }
    
    // calculate pulse in fixed window
    double charge1 = 0;
    double tdc1 = 0;
    double charge2 = 0;
    double tdc2 = 0;
    const int THRESHOLD = 100; // threshold for tdc start
    
    for (unsigned i=0; i<WblsDaq::NFADCBins-1; i++) {
        if(i>=g_tStartCounterPulse[counterNo-1][0] && i<=g_tStopCounterPulse[counterNo-1][0]) {
            charge1 += cleanTrace[i];
            if(cleanTrace[i]<THRESHOLD && cleanTrace[i+1]>THRESHOLD && tdc1<1) tdc1 = i;
            
        }
        else if(i>=g_tStartCounterPulse[counterNo-1][1] && i<=g_tStopCounterPulse[counterNo-1][1]) {
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
    // if (counterNo==1) {
    //     TGraph *g = new TGraph(WblsDaq::NFADCBins, xPoints, cleanTrace);
    //     g->Draw("AL");
    // }
}


void WFAnalyzer::Test()
{
    cout << fEvent->time << endl;
}