void plot_abs(){
  const int NUMENTRIES_water=60;
  double GeV = 1e9;
  double ENERGY_water[NUMENTRIES_water] =
    { 1.56962e-09*GeV, 1.58974e-09*GeV, 1.61039e-09*GeV, 1.63157e-09*GeV, 
      1.65333e-09*GeV, 1.67567e-09*GeV, 1.69863e-09*GeV, 1.72222e-09*GeV, 
      1.74647e-09*GeV, 1.77142e-09*GeV,1.7971e-09*GeV, 1.82352e-09*GeV, 
      1.85074e-09*GeV, 1.87878e-09*GeV, 1.90769e-09*GeV, 1.93749e-09*GeV, 
      1.96825e-09*GeV, 1.99999e-09*GeV, 2.03278e-09*GeV, 2.06666e-09*GeV,
      2.10169e-09*GeV, 2.13793e-09*GeV, 2.17543e-09*GeV, 2.21428e-09*GeV, 
      2.25454e-09*GeV, 2.29629e-09*GeV, 2.33962e-09*GeV, 2.38461e-09*GeV, 
      2.43137e-09*GeV, 2.47999e-09*GeV, 2.53061e-09*GeV, 2.58333e-09*GeV, 
      2.63829e-09*GeV, 2.69565e-09*GeV, 2.75555e-09*GeV, 2.81817e-09*GeV, 
      2.88371e-09*GeV, 2.95237e-09*GeV, 3.02438e-09*GeV, 3.09999e-09*GeV,
      3.17948e-09*GeV, 3.26315e-09*GeV, 3.35134e-09*GeV, 3.44444e-09*GeV, 
      3.54285e-09*GeV, 3.64705e-09*GeV, 3.75757e-09*GeV, 3.87499e-09*GeV, 
      3.99999e-09*GeV, 4.13332e-09*GeV, 4.27585e-09*GeV, 4.42856e-09*GeV, 
      4.59258e-09*GeV, 4.76922e-09*GeV, 4.95999e-09*GeV, 5.16665e-09*GeV, 
      5.39129e-09*GeV, 5.63635e-09*GeV, 5.90475e-09*GeV, 6.19998e-09*GeV };
  
   double ABWFF = 1.0;
  double cm = 0.01;

   double ABSORPTION_water[NUMENTRIES_water] =
    {
      16.1419*cm*ABWFF,  18.278*cm*ABWFF, 21.0657*cm*ABWFF, 24.8568*cm*ABWFF, 30.3117*cm*ABWFF, 
      38.8341*cm*ABWFF, 54.0231*cm*ABWFF, 81.2306*cm*ABWFF, 120.909*cm*ABWFF, 160.238*cm*ABWFF, 
      193.771*cm*ABWFF, 215.017*cm*ABWFF, 227.747*cm*ABWFF,  243.85*cm*ABWFF, 294.036*cm*ABWFF, 
      321.647*cm*ABWFF,  342.81*cm*ABWFF, 362.827*cm*ABWFF, 378.041*cm*ABWFF, 449.378*cm*ABWFF,
      739.434*cm*ABWFF, 1114.23*cm*ABWFF, 1435.56*cm*ABWFF, 1611.06*cm*ABWFF, 1764.18*cm*ABWFF, 
      2100.95*cm*ABWFF,  2292.9*cm*ABWFF, 2431.33*cm*ABWFF,  3053.6*cm*ABWFF, 4838.23*cm*ABWFF, 
      6539.65*cm*ABWFF, 7682.63*cm*ABWFF, 9137.28*cm*ABWFF, 12220.9*cm*ABWFF, 15270.7*cm*ABWFF, 
      19051.5*cm*ABWFF, 23671.3*cm*ABWFF, 29191.1*cm*ABWFF, 35567.9*cm*ABWFF,   42583*cm*ABWFF,
      49779.6*cm*ABWFF, 56465.3*cm*ABWFF,   61830*cm*ABWFF, 65174.6*cm*ABWFF, 66143.7*cm*ABWFF,   
      64820*cm*ABWFF,   61635*cm*ABWFF, 57176.2*cm*ABWFF, 52012.1*cm*ABWFF, 46595.7*cm*ABWFF, 
      41242.1*cm*ABWFF, 36146.3*cm*ABWFF, 31415.4*cm*ABWFF, 27097.8*cm*ABWFF, 23205.7*cm*ABWFF, 
      19730.3*cm*ABWFF, 16651.6*cm*ABWFF, 13943.6*cm*ABWFF, 11578.1*cm*ABWFF, 9526.13*cm*ABWFF
    };

   double ABSORPTION_water1[NUMENTRIES_water] = //old
     { 22.8154*cm*ABWFF, 28.6144*cm*ABWFF, 35.9923*cm*ABWFF, 45.4086*cm*ABWFF, 57.4650*cm*ABWFF,
       72.9526*cm*ABWFF, 92.9156*cm*ABWFF, 118.737*cm*ABWFF, 152.255*cm*ABWFF, 195.925*cm*ABWFF,
       202.429*cm*ABWFF, 224.719*cm*ABWFF, 236.407*cm*ABWFF, 245.700*cm*ABWFF, 289.017*cm*ABWFF,
       305.810*cm*ABWFF, 316.456*cm*ABWFF, 326.797*cm*ABWFF, 347.222*cm*ABWFF, 414.938*cm*ABWFF,
       636.943*cm*ABWFF, 934.579*cm*ABWFF, 1245.33*cm*ABWFF, 1402.52*cm*ABWFF, 1550.39*cm*ABWFF,
       1745.20*cm*ABWFF, 1883.24*cm*ABWFF, 2016.13*cm*ABWFF, 2442.18*cm*ABWFF, 3831.28*cm*ABWFF,
       4652.89*cm*ABWFF, 5577.04*cm*ABWFF, 6567.08*cm*ABWFF, 7559.88*cm*ABWFF, 8470.06*cm*ABWFF,
       9205.54*cm*ABWFF, 9690.95*cm*ABWFF, 9888.36*cm*ABWFF, 9804.50*cm*ABWFF, 9482.17*cm*ABWFF,
       8982.77*cm*ABWFF, 8369.39*cm*ABWFF, 7680.31*cm*ABWFF, 6902.11*cm*ABWFF, 6183.84*cm*ABWFF,
       5522.27*cm*ABWFF, 4914.33*cm*ABWFF, 4357.09*cm*ABWFF, 3847.72*cm*ABWFF, 3383.51*cm*ABWFF,
       2961.81*cm*ABWFF, 2580.08*cm*ABWFF, 2235.83*cm*ABWFF, 1926.66*cm*ABWFF, 1650.21*cm*ABWFF,
       1404.21*cm*ABWFF, 1186.44*cm*ABWFF, 994.742*cm*ABWFF, 827.027*cm*ABWFF, 681.278*cm*ABWFF};

  double wavelength[60],lambda[60],lambda1[60];
  
  for (Int_t i=0;i!=60;i++){
    wavelength[i] = 1240./ENERGY_water[i];
    lambda[i] = 1./(ABSORPTION_water[i]);
    lambda1[i] = 1./(ABSORPTION_water1[i]);
    
  }
  
  
  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  c1->SetFillColor(10);
  c1->SetLogy(1);
  TGraph *g1 = new TGraph(60,wavelength,lambda);
  g1->Draw("AL");
  g1->GetYaxis()->SetRangeUser(1e-4,1000);
  g1->GetXaxis()->SetTitle("Wavelength (nm)");
  g1->SetTitle();
  g1->GetYaxis()->SetTitle("1/#lambda (1/m)");
  TGraph *g2 = new TGraph(60,wavelength,lambda1);
  g2->Draw("Lsame");
  g2->SetLineColor(2);

  

  double wl1, abs;
  double x[900],y[900];
  ifstream infile("abs.txt");
  for (Int_t i=0;i!=900;i++){
    infile >> wl1 >> abs;
    x[i] = wl1;
    abs +=5e-4;
    //cout << x[i] << endl;
    y[i] = abs/0.1;
  }

  for (Int_t i=0;i!=60;i++){
    Double_t a1 = (ABSORPTION_water[i]);
    Double_t a2 = 1./connect(wavelength[i],900,x,y);
    if (a1>a2) 
      {
	cout << a2*100 << "*cm*ABWFF,";
      }else{
      cout <<a1*100 << "*cm*ABWFF,";
    }
  }
  cout << endl;
  
  TGraph *g3 = new TGraph(900,x,y);
  g3->Draw("Lsame");
  g3->SetLineColor(4);

  TLegend *le1 = new TLegend(0.45,0.6,0.7,0.89);
  le1->SetFillColor(10);
  le1->SetBorderSize(0);
  le1->AddEntry(g1,"SK Water","l");
  le1->AddEntry(g2,"Water","l");
  le1->AddEntry(g3,"Water-LS","l");
  le1->Draw();
}

Double_t connect(Double_t x, Int_t ncount, Double_t *wave_length, Double_t *quantity){
  // linear interpolate the quantity function versus wave_length
  if (x < *wave_length || x >=*(wave_length+ncount-1)){
    // if (x < *wave_length) return *(quantity);
//     if (x >=*(wave_length+ncount-1) return *(quantity+ncount-1);
    return 0;
  }else{
    for (Int_t i=0;i!=ncount;i++){
      if (x>=*(wave_length+i) && x < *(wave_length+i+1)){
	return (x-*(wave_length+i))/(*(wave_length+i+1)-*(wave_length+i))* (*(quantity+i+1)) + (*(wave_length+i+1)-x)/(*(wave_length+i+1)-*(wave_length+i)) * (*(quantity+i));
      }
    }
  }
}
