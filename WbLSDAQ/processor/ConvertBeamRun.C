{
    gROOT->ProcessLine(".x load.C");
    // BeamTree x(1980, 2177);
    // x.PrintInfo();
    
    BeamTree *x;
    x = new BeamTree(1575, 1656, "water-2000MeV"); x->Process(); delete x;
    x = new BeamTree(1658, 1830, "water-475MeV"); x->Process(); delete x;
    x = new BeamTree(1831, 1979, "water-210MeV"); x->Process(); delete x;
    
    x = new BeamTree(1980, 2177, "wbls1-210MeV"); x->Process(); delete x;
    x = new BeamTree(2178, 2383, "wbls1-475MeV"); x->Process(); delete x;
    x = new BeamTree(2384, 2571, "wbls1-2000MeV"); x->Process(); delete x;
    
    x = new BeamTree(2572, 2740, "wbls2-2000MeV"); x->Process(); delete x;
    x = new BeamTree(2743, 2923, "wbls2-475MeV"); x->Process(); delete x;
    x = new BeamTree(2924, 3101, "wbls2-210MeV"); x->Process(); delete x;
    
    x = new BeamTree(3121, 3314,  "ls-210MeV", true); x->Process(); delete x;
    x = new BeamTree(3315, 3506,  "ls-475MeV", true); x->Process(); delete x;
    x = new BeamTree(3507, 3662, "ls-2000MeV", true); x->Process(); delete x;
    
}