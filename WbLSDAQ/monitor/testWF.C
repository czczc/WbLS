{
    gROOT->ProcessLine(".x Load.C");
    
    WblsDaq::Spinner spinner("/Users/chaozhang/Projects/LBNE/WbLS/software/WbLSDAQ/data/test/example.root");
    for (int i=0; i<1; ++i) {
        // int nbytes = spinner.load(i);
        WblsDaq::Event* evt = spinner.event(i);    
        WFAnalyzer wf(evt);
        wf->Process();
        cout << "baseline: tub1: " << wf->fBaseLine_Tub[0] << ", tub2: " << wf->fBaseLine_Tub[1] << endl;
        cout << "  charge: tub1: " << wf->fCharge_Tub[0] << ", tub2: " << wf->fCharge_Tub[1] << endl;
        cout << "     tdc: tub1: " << wf->fTDC_Tub[0] << ", tub2: " << wf->fTDC_Tub[1] << endl;
        cout << "  charge: pulse1: counter1: " << wf->fCharge_Counter[0][0] << ", counter2: " << wf->fCharge_Counter[1][0] << endl;
        cout << "  charge: pulse2: counter1: " << wf->fCharge_Counter[0][1] << ", counter2: " << wf->fCharge_Counter[1][1] << endl;
        cout << "     tdc: pulse1: counter1: " << wf->fTDC_Counter[0][0] << ", counter2: " << wf->fTDC_Counter[1][0] << endl;
        cout << "     tdc: pulse2: counter1: " << wf->fTDC_Counter[0][1] << ", counter2: " << wf->fTDC_Counter[1][1] << endl;
        
        // wf->Test();
    }
    
}