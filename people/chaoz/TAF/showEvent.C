// gloable variables
TCanvas c1;


TFile f("events100.root");
TTree *Sim = (TTree*)f.Get("/Event/Sim");
int entry = -1;
TString drawStr;
TString title;
const int NMODES = 70;
const char* modeText[NMODES];
string last = "next";
bool inited = false;

// for (int i = 0; i < NMODES; ++i) {
// 	modeText[i] = "Unknown";
// }


void showEvent(const char* filename="events100.root"){}


void draw()
{
	int mode; 
	Sim->GetBranch("mode")->SetAddress(&mode);

	Sim->GetEntry(entry);
	cout << mode << endl;

	drawStr.Form("Entry$==%i", entry);
	gPad->SetLogx();
	Sim->Draw("hit_tc>>h(20000,1,20000)", drawStr);
	TH1F *h = (TH1F*)gDirectory->FindObject("h");
	title.Form("Event %i: %s", entry, modeText[mode]);
	h->SetTitle(title);
	h->GetXaxis()->SetTitle("Hit Time [ns]");
	h->GetYaxis()->SetTitle("Number of PE");
	h->GetYaxis()->SetRangeUser(0, 150);
}

void next()
{
	if (!inited) { init(); }

	entry++;
	// t.Show(entry);
	draw();
	last = "next";
}

void prev()
{
	entry--;
	draw();
	last = "prev";
}

void toggle()
{
	if (last == "next") { prev(); last = "prev"; }
	else { next(); last = "next"; }
}

void show()
{
	Sim->Show(entry);
}

void init()
{
	modeText[0]  = "Unknown??";
	modeText[11] = "K^{+} -> #mu^{+} + #nu_{#mu} (63.47%)";
	modeText[21] = "K^{+} -> #pi^{+} + #pi^{0} (21.13%)";
	modeText[31] = "K^{+} -> #pi^{+} + #pi^{+} + #pi^{-} (5.58%)";
	modeText[41] = "K^{+} -> #pi^{0} + e^{+} + #nu_{e} (4.87%)";
	modeText[51] = "K^{+} -> #pi^{0} + #mu^{+} + #nu_{#mu} (3.27%)";
	modeText[61] = "K^{+} -> #pi^{+} + #pi^{0} + #pi^{0} (1.73%)";
	style();

	inited = true;
}


void style()
{
	gROOT->SetStyle("Plain");
	gStyle->SetOptStat("e");
	gStyle->SetPalette(1);
	gStyle->SetTitleStyle(0);
	gStyle->SetTitleBorderSize(0);
	gStyle->SetTitleOffset(1.1, "x");
	gStyle->SetTitleOffset(1.25, "y");
	gStyle->SetHistLineWidth(2);
	gStyle->SetLegendBorderSize(0);
	gROOT->ForceStyle(); 
	Sim->UseCurrentStyle();

	gPad->SetLogx();

}