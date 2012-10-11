#include "WblsDaqEvent.h"

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>

#include <iostream>
using namespace std;

void WblsDaq::set_branch_address(TTree* tree, WblsDaq::Event* obj)
{
    tree->SetBranchAddress("EventNumber",&obj->count);
    tree->SetBranchAddress("TriggerTimeFromRunStart",&obj->time);
    tree->SetBranchAddress("Channel0",&obj->ch0);
    tree->SetBranchAddress("Channel1",&obj->ch1);
    tree->SetBranchAddress("Channel2",&obj->ch2);
    tree->SetBranchAddress("Channel3",&obj->ch3);
    tree->SetBranchAddress("TriggerCountFromRunStart",  &obj->time_count);
}
void WblsDaq::set_branch_address(TTree* tree, WblsDaq::Header* obj)
{
    tree->SetBranchAddress("RunNumber", &obj->run_number);
    tree->SetBranchAddress("RunStartTime",  &obj->runstart);
    tree->SetBranchAddress("IsThisDataFile",  &obj->isdata);
    tree->SetBranchAddress("is12or14bit",  &obj->nbits);
    tree->SetBranchAddress("frequency",  &obj->freqtype);
    tree->SetBranchAddress("runtype",  &obj->runtype);
    tree->SetBranchAddress("sampletype",  &obj->sampletype);
    tree->SetBranchAddress("Channel0Gain",  &obj->ch0_gain);
    tree->SetBranchAddress("Channel1Gain",  &obj->ch1_gain);
    tree->SetBranchAddress("Channel2Gain",  &obj->ch2_gain);
    tree->SetBranchAddress("Channel3Gain",  &obj->ch3_gain);
    tree->SetBranchAddress("Channel0device",  &obj->ch0_device);
    tree->SetBranchAddress("Channel1device",  &obj->ch1_device);
    tree->SetBranchAddress("Channel2device",  &obj->ch2_device);
    tree->SetBranchAddress("Channel3device",  &obj->ch3_device);
    tree->SetBranchAddress("PedestalSubstructedAtRun",  &obj->pedestal);
    tree->SetBranchAddress("PerformanceFrequency",  &obj->perform_freq);
}
void WblsDaq::set_branch_address(TTree* tree, WblsDaq::Footer* obj)
{
    tree->SetBranchAddress("TotalEventsNumber",  &obj->nevents);
    tree->SetBranchAddress("RunStopTime",  &obj->runstop);
    
}

WblsDaq::Spinner::~Spinner()
{
    if (m_event) delete m_event;
    if (m_head) delete m_head;
    if (m_foot) delete m_foot;
}

WblsDaq::Spinner::Spinner(std::string filename)
{
    TFile* fp = TFile::Open(filename.c_str()); // intentional leak

    m_event_tree = dynamic_cast<TTree*>(fp->Get("FADCData"));
    m_event = new WblsDaq::Event();
    set_branch_address(m_event_tree, m_event);

    m_head_tree = dynamic_cast<TTree*>(fp->Get("Header"));
    m_head = new WblsDaq::Header();
    set_branch_address(m_head_tree, m_head);

    m_foot_tree = dynamic_cast<TTree*>(fp->Get("Footer"));
    m_foot = new WblsDaq::Footer();
    set_branch_address(m_foot_tree, m_foot);
}

WblsDaq::Spinner::Spinner(const std::vector<std::string>& filenames)
{
    TChain* chain = new TChain("FADCData","WbLS DAQ Triggers");
    m_event_tree = chain;
    m_event = new WblsDaq::Event();
    set_branch_address(m_event_tree, m_event);

    m_head = 0;
    m_foot = 0;

    for (size_t ind=0; ind< filenames.size(); ++ind) {
        chain->Add(filenames[ind].c_str());

        bool leak = false;
        TFile* fp = TFile::Open(filenames[ind].c_str());

        if (!m_head) {
            TTree* t = dynamic_cast<TTree*>(fp->Get("Header"));
            if (t) {
                m_head = new WblsDaq::Header();
                set_branch_address(t, m_head);
                leak = true;
            }
        }
        if (!m_foot) {
            TTree* t = dynamic_cast<TTree*>(fp->Get("Footer"));
            if (t) {
                m_foot = new WblsDaq::Footer();
                set_branch_address(t, m_foot);
                leak = true;
            }
        }

        // Fixme: best to not leak, but no real harm is done
        if (leak) { continue; }
        delete fp; fp = 0;
    }
}
        
int WblsDaq::Spinner::entries()
{
    if (!m_event_tree) return 0;
    return m_event_tree->GetEntries();
}

int WblsDaq::Spinner::load(int entry)
{
    if (!m_event_tree) return 0;
    return m_event_tree->GetEntry(entry);
}

WblsDaq::Event* WblsDaq::Spinner::event(int entry)
{
    if (!m_event_tree) return 0;
    if (entry >= 0) {
        this->load(entry);
    }
    return m_event;
}
        
WblsDaq::Header* WblsDaq::Spinner::header()
{
    m_head_tree->GetEntry(0);
    return m_head;
}

WblsDaq::Footer* WblsDaq::Spinner::footer()
{
    m_foot_tree->GetEntry(0);
    
    return m_foot;
}

void WblsDaq::Spinner::PrintRunInfo()
{
    WblsDaq::Header* h = header();
    WblsDaq::Footer* f = footer();
    cout << "  run_number:" << h->run_number << endl;
    cout << "    runstart:" << h->runstart << endl;
    cout << "      isdata:" << h->isdata << endl;
    cout << "       nbits:" << h->nbits << endl;
    cout << "    freqtype:" << h->freqtype << endl;
    cout << "     runtype:" << h->runtype << endl;
    cout << "  sampletype:" << h->sampletype << endl;
    cout << "    ch0_gain:" << h->ch0_gain << endl;
    cout << "    ch1_gain:" << h->ch1_gain << endl;
    cout << "    ch2_gain:" << h->ch2_gain << endl;
    cout << "    ch3_gain:" << h->ch3_gain << endl;
    cout << "  ch0_device:" << h->ch0_device << endl;
    cout << "  ch1_device:" << h->ch1_device << endl;
    cout << "  ch2_device:" << h->ch2_device << endl;
    cout << "  ch3_device:" << h->ch3_device << endl;
    cout << "    pedestal:" << h->pedestal << endl;
    cout << "    pedestal:" << h->pedestal << endl;
    cout << "perform_freq:" << h->perform_freq << endl;
    
    cout << "   runstop:" << f->runstop << endl;
    cout << "   nevents:" << f->nevents << endl;
}

