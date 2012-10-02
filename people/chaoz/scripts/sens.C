{
    double total = 22.5e9 * 6e23 / (0.66+16*0.3+12*0.03);
    double free_proton = total * 0.66;
    double bound_proton = total * (0.3*8 + 0.03*6);
    double total_proton = free_proton+bound_proton;
    cout << " free proton: " << free_proton << endl;
    cout << "bound proton: " << bound_proton << endl;
    cout << "total proton: " << total_proton << endl;
    
    double current_limit = 2.3e33; // years
    double br = 0.635+0.211;
    double ch1_eff = 0.968 * 0.992 * 0.956;
    double ch1_eff_golden = ch1_eff * 0.54;
    double ch1_eff_silver = ch1_eff * 0.424;
    cout << "ch1: golden: eff: " << ch1_eff_golden << endl;
    cout << "ch1: silver: eff: " << ch1_eff_silver << endl;
    cout << "effective eff: " << ch1_eff_golden + ch1_eff_silver*(free_proton+bound_proton*0.5)/total_proton << endl;
    
    double run_time = 10; // years
    double nevent_golden = total_proton * br * ch1_eff_golden * run_time / current_limit;
    double nevent_silver = (free_proton+bound_proton*0.5) * br * ch1_eff_silver * run_time / current_limit;
    cout << "total events: golden: " << nevent_golden << endl; 
    cout << "total events: silver: " << nevent_silver << endl; 
    
    
    // sensitivity
    cout << "=================================" << endl;
    double proton_lifetime = 0;
    double cl90_events = 2.34;  // FC, 90% C.L (sig, bg) = (0, 0.1)
    proton_lifetime = (total_proton*ch1_eff_golden + (free_proton+bound_proton*0.5)*ch1_eff_silver)
        * br * run_time / cl90_events;
    cout << "sensitivity:  no event: " << proton_lifetime << endl;
    double cl90_events1 = 4.26;  // FC, 90% C.L (sig, bg) = (1, 0.1)
    proton_lifetime = (total_proton*ch1_eff_golden + (free_proton+bound_proton*0.5)*ch1_eff_silver)
        * br * run_time / cl90_events1;
    cout << "sensitivity: one event: " << proton_lifetime << endl;
    
    double cl90_events = 2.0;  // FC, 90% C.L (sig, bg) = (0, 0.6)
    cout << "sensitivity LENA: no event " << 1.45e34*0.65 * 10 / cl90_events << endl; // LENA
    
    cout << "=================================" << endl;
    int start_yr = 2020;
    int end_year = 2040;
    for (int i=start_yr; i<=end_year; i++) {
        double run_yr = i-start_yr;
        if (i==start_yr) run_yr = 1./12;  // 1-month sensitivy
        double cl90_ev = 2.44 - (2.44-1.94)/0.5*0.01*run_yr;
        double x = i;
        double y = (total_proton*ch1_eff_golden + (free_proton+bound_proton*0.5)*ch1_eff_silver)
            * br * run_yr / cl90_ev;
        cout << x << " " << y << endl;
    }
}

