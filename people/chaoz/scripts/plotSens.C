{
    gROOT->Reset();
    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(0);
    gStyle->SetPalette(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleOffset(1.1, "x");
    gStyle->SetTitleOffset(1.25, "y");
    gStyle->SetHistLineWidth(2);
    gStyle->SetLegendBorderSize(0);
    gROOT->ForceStyle();
    
    ifstream in("sk_sen.txt");
    string line;
    double x, y;
    vector<double> v_x, v_y; 
    while(true){
        getline(in, line);
        if (!in.good() || line.length()<1) break;
        istringstream lineStr(line);
        if (lineStr.peek() == '#') continue;
        lineStr >> x >> y;
        v_x.push_back(x);
        v_y.push_back(y);       
    }
    int size = v_x.size();
    double *a_x = new double[size];
    double *a_y = new double[size];
    
    for (int i=0; i<size; i++) {
        a_x[i] = v_x.at(i);
        a_y[i] = v_y.at(i);
        // cout << a_x[i] << " " << a_y[i] << endl;   
    }
    
    ifstream in2("wbls_sen.txt");
    vector<double> v_x2, v_y2; 
    while(true){
        getline(in2, line);        
        if (!in2.good() || line.length()<1) break;
        istringstream lineStr(line);
        if (lineStr.peek() == '#') continue;
        lineStr >> x >> y;
        v_x2.push_back(x);
        v_y2.push_back(y);       
    }
    int size2 = v_x2.size();
    double *a_x2 = new double[size];
    double *a_y2 = new double[size];
    
    for (int i=0; i<size2; i++) {
        a_x2[i] = v_x2.at(i);
        a_y2[i] = v_y2.at(i);
        // cout << a_x2[i] << " " << a_y2[i] << endl;   
    }
    
    ifstream in3("lena_sen.txt");
    vector<double> v_x3, v_y3; 
    while(true){
        getline(in3, line);        
        if (!in3.good() || line.length()<1) break;
        istringstream lineStr(line);
        if (lineStr.peek() == '#') continue;
        lineStr >> x >> y;
        v_x3.push_back(x);
        v_y3.push_back(y);       
    }
    int size3 = v_x3.size();
    double *a_x3 = new double[size3];
    double *a_y3 = new double[size3];
    
    for (int i=0; i<size3; i++) {
        a_x3[i] = v_x3.at(i);
        a_y3[i] = v_y3.at(i);
        // cout << a_x2[i] << " " << a_y2[i] << endl;   
    }
    
    TCanvas c1;
    
    TH2F hDummy("hDummy", "", 100, 1995, 2040, 100, 1e32, 1e35);
    hDummy.SetXTitle("Year");
    hDummy.SetYTitle("Lifetime Sensitivity (90% C.L.)");
    hDummy.Draw();
    gPad->SetLogy();
    gPad->SetGridx();
    gPad->SetGridy();
    
    TGraph g_sk(size, a_x, a_y);
    g_sk.SetLineWidth(3);
    g_sk.SetLineColor(kAzure+9);
    g_sk.Draw("L,same");
    
    TGraph g_wbls(size2, a_x2, a_y2);
    g_wbls.SetLineWidth(3);
    g_wbls.SetLineColor(kRed-4);
    g_wbls.Draw("C,same");
    
    TGraph g_lena(size3, a_x3, a_y3);
    g_lena.SetLineWidth(3);
    g_lena.SetLineColor(kGreen+3);
    
    TLatex l(1997, 4e34, "p #rightarrow #nu + K^{+}");
    l.SetTextSize(0.07);
    l.Draw();
    
    TLatex l_sk(2004, 3.5e33, "Super-K");
    l_sk.SetTextColor(kAzure+9);
    l_sk.Draw();
    
    TLatex l_lena(2019, 2e34, "LENA");
    l_lena.SetTextColor(kGreen+3);
    
    TLatex l_wbls(2018, 2.1e34, "WbLS 22.5 kt");
    l_wbls.SetTextColor(kRed-4);
    l_wbls.Draw();
    
    c1.SaveAs("pdf/sens.pdf");
    
    bool drawLENA = true;
    if (drawLENA) {
        l_wbls.SetX(2028);
        l_wbls.SetY(1.01e34);
        g_lena.Draw("C,same");
        l_lena.Draw();
        c1.SaveAs("pdf/sens_incLENA.pdf");
    }
    
    in.close();
    in2.close();
    delete[] a_x;
    delete[] a_y;
    delete[] a_x2;
    delete[] a_y2;
}