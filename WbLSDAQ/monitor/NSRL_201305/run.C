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
    gStyle->SetMarkerStyle(25);
    gStyle->SetMarkerSize(0.6);
    gROOT->ForceStyle();
    
    gROOT->ProcessLine(".x Load.C");
    
    MainWindow *gMainWindow = new MainWindow(gClient->GetRoot(), 500, 300);
    
}