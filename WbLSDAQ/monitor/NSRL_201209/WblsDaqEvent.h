/**
 * \class WblsDaqEvent
 *
 * \brief One Trigger of the DAQ
 *
 * This provides structs that shadow the structure of the DAQ trees
 * and a class that assists in reading these trees for analysis.
 *
 * bv@bnl.gov Tue Sep 25 10:49:07 2012
 *
 */


#ifndef WBLSDAQEVENT_H
#define WBLSDAQEVENT_H

#include <string>
#include <vector>

class TTree;

namespace WblsDaq {

    namespace RunType {
        enum RunType_t {
            kUnspecified = 0,       // see DAQ log output
            kPedestal,              // pedestal run, no detectors
            kCosmicTube,            // original cosmic tube detector
            kTubDet,                // stand-alone Tub
            kNSRL,                  // NSRL setup
            // ...
            nRunTypes
        };
    }

    namespace SampleType {
        enum SampleType_t {
            kUnspecified = 0,       // see DAQ log output
            kAir,                   // 
            kWater,                 // 
            kWBLS1,                 // 
            kWBLS2,                 // 
            kLS1,                   // 
            kLS2,
            // ...
            nSampleTypes
        };
    }

    namespace FrequencyType {
        enum FrequencyType_t {
            kUnspecified = 0,
            k2Gsps,k1Gsps,k500Msps,k400Msps,k200Msps,k100Msps,k50Msps,
            // ...
            nFreqTypes
        };
    }

    typedef unsigned short FADCBin;

    const unsigned int NFADCBins = 2560;

    struct Event {

        int count;              // the trigger number
        double time;            // time since start of run
	    FADCBin ch0[NFADCBins]; // ch0's digitized trace
	    FADCBin ch1[NFADCBins]; // ch1's digitized trace
	    FADCBin ch2[NFADCBins]; // ch2's digitized trace
	    FADCBin ch3[NFADCBins]; // ch3's digitized trace
        Long64_t time_count;    // internal cpu count of the time since start of run
    };
    
    struct Header {
        int run_number;         // the run number
        // time_t runstart;        // absolute time of run start
        Long64_t runstart;        // absolute time of run start
        bool isdata;            // true if data, false if MC
        int nbits;              // bits per digitization (12 or 14)
        int freqtype;           // FADC digitization frequency setting
        int runtype;            // type of run
        int sampletype;         // type of sample
        float ch0_gain;         // set gain of ch0
        float ch1_gain;         // set gain of ch1
        float ch2_gain;         // set gain of ch2
        float ch3_gain;         // set gain of ch3
        int ch0_device;         // device on ch0
        int ch1_device;         // device on ch1
        int ch2_device;         // device on ch2
        int ch3_device;         // device on ch3
        int pedestal;           // 
        Long64_t perform_freq;  // performance freqency of CPU
    };

    struct Footer {
        int nevents;
        Long64_t runstop;
    };

    // Tell the tree to use the object's address space
    void set_branch_address(TTree* tree, WblsDaq::Event* obj);
    void set_branch_address(TTree* tree, WblsDaq::Header* obj);
    void set_branch_address(TTree* tree, WblsDaq::Footer* obj);

    class Spinner {
        TTree *m_event_tree, *m_head_tree, *m_foot_tree;
        WblsDaq::Event* m_event;
        WblsDaq::Header* m_head;
        WblsDaq::Footer* m_foot;

    public:

        // Create a spinner on the given file
        Spinner(std::string filename);

        // Create a spinner on a list of files
        Spinner(const std::vector<std::string>& filenames);
        
        ~Spinner();
        
        // Return number of entries
        int entries();

        // Load an entry, returns number of bytes loaded 
        int load(int entry);

        // Access the event.  If entry is < 0 return the currently
        // loaded event, otherwise load it
        Event* event(int entry = -1);
        
        // Access the footer and header.  May return 0.
        Header* header();
        Footer* footer();
        
        TTree* event_tree() { return m_event_tree; }
        TTree* header_tree() { return m_head_tree; }
        TTree* footer_tree() { return m_foot_tree; }
        
        void PrintRunInfo();
    };

} // namespace WblsDaq


#endif  // WBLSDAQEVENT_H
