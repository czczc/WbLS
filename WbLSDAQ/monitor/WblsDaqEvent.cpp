#include "WblsDaqEvent.h"

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>

void WblsDaq::set_branch_address(TTree* tree, WblsDaq::Event* obj)
{
    tree->SetBranchAddress("EventNumber",&obj->count);
    tree->SetBranchAddress("TriggerTimeFromRunStart",&obj->time);
    tree->SetBranchAddress("Channel0",&obj->ch0);
    tree->SetBranchAddress("Channel1",&obj->ch1);
    tree->SetBranchAddress("Channel2",&obj->ch2);
    tree->SetBranchAddress("Channel3",&obj->ch3);
}
void WblsDaq::set_branch_address(TTree* tree, WblsDaq::Header* obj)
{
}
void WblsDaq::set_branch_address(TTree* tree, WblsDaq::Footer* obj)
{
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
    return m_head;
}

WblsDaq::Footer* WblsDaq::Spinner::footer()
{
    return m_foot;
}
