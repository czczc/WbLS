#include "MCEvent.h"

#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"

using namespace std;

MCEvent::MCEvent(){}

MCEvent::MCEvent(const char* filename)
{
    rootFile = new TFile(filename);
    simTree = (TTree*)rootFile->Get("/Event/Sim");
    geoTree = (TTree*)rootFile->Get("/Detector/Geometry");

    InitDecayMode();
    InitBranchAddress();
    InitGeometry();
}

MCEvent::~MCEvent()
{
}

void MCEvent::InitDecayMode()
{
    decayMode[0]  = "Unknown??";
    decayMode[-11] = "Background: Muon";
    decayMode[11] = "K^{+} -> #mu^{+} + #nu_{#mu} (63.47%)";           // 63.47%
    decayMode[21] = "K^{+} -> #pi^{+} + #pi^{0} (21.13%)";             // 21.13%
    decayMode[31] = "K^{+} -> #pi^{+} + #pi^{+} + #pi^{-} (5.58%)";    // 5.58%
    decayMode[41] = "K^{+} -> #pi^{0} + e^{+} + #nu_{e} (4.87%)";      // 4.87%
    decayMode[51] = "K^{+} -> #pi^{0} + #mu^{+} + #nu_{#mu} (3.27%)";  // 3.27%
    decayMode[61] = "K^{+} -> #pi^{+} + #pi^{0} + #pi^{0} (1.73%)";    // 1.73%
}

void MCEvent::InitBranchAddress()
{
    simTree->SetBranchAddress("eventNumber" , &eventNumber);
    simTree->SetBranchAddress("mode"        , &mode);
    simTree->SetBranchAddress("nTrigger"    , &nTrigger);

    simTree->SetBranchAddress("nTrack"      , &nTrack);
    simTree->SetBranchAddress("track_pdg"   , &track_pdg);
    simTree->SetBranchAddress("track_parent", &track_parent);
    simTree->SetBranchAddress("track_t"     , &track_t);
    simTree->SetBranchAddress("track_e"     , &track_e);
    simTree->SetBranchAddress("track_ee"    , &track_ee);
    simTree->SetBranchAddress("track_x"     , &track_x);
    simTree->SetBranchAddress("track_y"     , &track_y);
    simTree->SetBranchAddress("track_z"     , &track_z);
    simTree->SetBranchAddress("track_dx"    , &track_dx);
    simTree->SetBranchAddress("track_dy"    , &track_dy);
    simTree->SetBranchAddress("track_dz"    , &track_dz);

    simTree->SetBranchAddress("nPMT"        , &nPMT);
    simTree->SetBranchAddress("nHit"        , &nHit);
    simTree->SetBranchAddress("hit_pmtID"   , &hit_pmtID);
    simTree->SetBranchAddress("hit_t"       , &hit_t);
    simTree->SetBranchAddress("hit_tc"      , &hit_tc);
    simTree->SetBranchAddress("hit_wl"      , &hit_wl);
}

void MCEvent::InitGeometry()
{
    geoTree->SetBranchAddress("cylRadius", &cylRadius);
    geoTree->SetBranchAddress("cylLength", &cylLength);
    geoTree->SetBranchAddress("totalPMTs", &totalPMTs);
    geoTree->SetBranchAddress("pmt_x", &pmt_x);
    geoTree->SetBranchAddress("pmt_y", &pmt_y);
    geoTree->SetBranchAddress("pmt_z", &pmt_z);

    geoTree->GetEntry(0);
}

void MCEvent::PrintInfo()
{
    cout << "=================================\n";
    cout << "#" << eventNumber << ": nTrigger: " << nTrigger
         << " | nTrack: "<< nTrack
         << " | nPMT: "<< nPMT
         << " | nHit: "<< nHit
         << "\n\tDecay Mode " << mode << ": " << decayMode[mode]
         << "\n";
    for (int i=2; i<nTrack; i++) {
         cout << "\tPDG: " << track_pdg[i]
             << " | Parent: " << track_parent[i]
             << " | Time: " << track_t[i]
             << " | KE: " << track_e[i]
             << " | E: " << track_ee[i]
             << " | Vtx: (" << track_x[i] << ", " << track_y[i] << ", " << track_z[i] << ")"
             // << " | Dir: (" << track_dx[i] << ", " << track_dy[i] << ", " << track_dz[i] << ")"
             << "\n";
    }
    cout << endl;
}

vector< vector<float> > MCEvent::FindRing(int track_id, int resolution)
{
    // this code is borrowed from superscan
    vector<vector<float> > ring;
    float theta = 42; // degree
    float pos[3], e1[3], e2[3], e3[3];
    float magnitude = 0.0, phi = 0.0, dphi, st, ct;
    float a, b = 0.0;
    float vcone[3];
    float pi = TMath::Pi();
    int id = track_id;
    float r_index = 1.333*1.0112;
    // float theta = acos(1/rIndex/sqrt(1-(track_ee[id] - track_e[id])/track_e[id]/track_e[id]))*180/pi;
    float E = track_ee[id];
    float KE = track_e[id];
    float M = E - KE;
    float v_over_c = 1;
    // if (fabs(M)<0.01) { c_over_v = 1; } // speed of light;
    // else {
    v_over_c = sqrt((E*E-M*M)/E/E);
    // cout << v_over_c << endl;
    if (v_over_c < 1/r_index) {
        return ring; // below Cherenkov thresold
    }
    else {
        theta = acos(1/v_over_c/r_index)*180/pi;
    }
    // }

    // cout << theta << endl;

    pos[0] = track_x[id];
    pos[1] = track_y[id];
    pos[2] = track_z[id];

    /* get coordinate system with e3 along dir.
     This used to be done by vbasis (random direction and all), but
     for now, make this ultra-portable, by doing by hand */
    e3[0] = track_dx[id]; 
    e3[1] = track_dy[id]; 
    e3[2] = track_dz[id];

    /* use either x-axis or y-axis to define a plane, and find
     transformed y-axis as perpendicular to this plane */
    if (fabs(track_dy[id]) < fabs(track_dx[id])) {
        /* if dir[1] is more nearly perpendicular to the y-axis,
           then take e2 as (y x e3) */
        e2[0] = e3[2];
        e2[1] = 0;
        e2[2] = -e3[0];
    }
    else {
        /* dir[0] is more nearly perpendicular to x-axis,
           so take e2 as (x x e3) */
        e2[0] = 0;
        e2[1] = -e3[2];
        e2[2] = e3[1];
    }

    /* now find last axis (transformed x-axis) by e2 x e3 */
    e1[0] = e2[1]*e3[2] - e2[2]*e3[1];
    e1[1] = e2[2]*e3[0] - e2[0]*e3[2];
    e1[2] = e2[0]*e3[1] - e2[1]*e3[0];

    /* normalize directions */
    magnitude = sqrt(e1[0]*e1[0] + e1[1]*e1[1] + e1[2]*e1[2]);
    e1[0] = e1[0]/magnitude;
    e1[1] = e1[1]/magnitude;
    e1[2] = e1[2]/magnitude;

    magnitude = sqrt(e2[0]*e2[0] + e2[1]*e2[1] + e2[2]*e2[2]);
    e2[0] = e2[0]/magnitude;
    e2[1] = e2[1]/magnitude;
    e2[2] = e2[2]/magnitude;

    magnitude = sqrt(e3[0]*e3[0] + e3[1]*e3[1] + e3[2]*e3[2]);
    e3[0] = e3[0]/magnitude;
    e3[1] = e3[1]/magnitude;
    e3[2] = e3[2]/magnitude;

    /* Get ready to loop over cone */
    theta *= (pi / 180.0);
    dphi = (2.0 * pi) / (float)resolution;
    st = sin(theta);
    ct = cos(theta);

    for (int i = 0; i < resolution; i++) {
        /* Get a vector in the cone */
        a = cos(phi) * st;
        b = sin(phi) * st;

        vcone[0] = a * e1[0] + b * e2[0] + ct * e3[0];
        vcone[1] = a * e1[1] + b * e2[1] + ct * e3[1];
        vcone[2] = a * e1[2] + b * e2[2] + ct * e3[2];
        
        ring.push_back(ProjectToTankSurface_C(pos, vcone));

        phi += dphi;
    }
    return ring;
}    

vector<float> MCEvent::ProjectToTankSurface_C(float *vtx, float *dir)
{
    // projects a vector to the tank surface (taken from SuperScan)
    float a,b,c,d=0;
    float det_r = cylRadius;
    float det_z = cylLength/2;
    float pos[3];

    /* Find intercept of this vector with walls of detector */
    a = (dir[0]*dir[0]) + (dir[1]*dir[1]);
    b = 2.0 * ( dir[0] * vtx[0] + dir[1] * vtx[1] );
    c = (vtx[0]*vtx[0]) + (vtx[1]*vtx[1]) - (det_r*det_r);

    if (a != 0.0) {
        /* Find intersection with walls */
        c = (b*b - 4.0*a*c);
        if (c>0.0) {
            c = sqrt(c);
            d = (-b+c)/(2.0*a);
            if (d<0.0) d = (-b-c)/(2.0*a);
            pos[2] = vtx[2] + d*dir[2];

            /* Check that the z coordinate is inside the detector */

            if (pos[2]>det_z) { /* Intersects top */
                d = (det_z-vtx[2])/dir[2];
                pos[2] = det_z;
            }
            else
                if (pos[2]<-det_z) { /* Intersects bottom */
                    d = (-det_z-vtx[2])/dir[2];
                    pos[2] = -det_z;
                }
        }
        pos[0] = vtx[0] + d*dir[0];
        pos[1] = vtx[1] + d*dir[1];
    }
    else {
        /* Goes out through the top */
        pos[0] = vtx[0];
        pos[1] = vtx[1];
        if (dir[2]>0.0) { pos[2] = det_z; }
        else { pos[2] = -det_z; }
    }

    std::vector<float> tmp;
    for (int i=0; i<3; i++) { tmp.push_back(pos[i]); }
    return tmp;
}
