/* 
A simple script for converting hepevt format files to root files.
This one keep only leptons,but the same logic can apply for any other implementation.


*/
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <chrono>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <sstream> // std::istringstream
#include <string>  // std::string
#include <iomanip>
#include <ROOT/TTree.h>
#include <ROOT/TFile.h>
#include <ROOT/TLorentzVector.h>

using namespace std;

class myVertex
{
  public:
    double xx;        // vertex x position in mm
    double yy;        // vertex y position in mm
    double zz;        // vertex z position in mm
    double prod_time; // production time in mm/c
};

class myParticle
{
  public:
    double px;   // momentum in x axis in GeV/c
    double py;   // momentum in y axis in GeV/c
    double pz;   // momentum in z axis in GeV/c
    double e;    // Energy in GeV
    double mass; // mass in GeV/c^2
};

class myHEPEVT
{
  public:
    vector<int> nparhep;           // number of particle in the event
    vector<int> isthep;            // status code of each particle in the event
    vector<int> idhep;             // PDG particle identity
    vector<pair<int, int>> jmohep; // positions of each particle mothers in the event
    vector<pair<int, int>> jdahep; // positions of each particle daughters in the event
    vector<myParticle> phep;       // momentum info of each particles
    vector<myVertex> vhep;         // vertex info of each particle

    myHEPEVT(const int &n_of_particles)
    {
        nparhep.resize(n_of_particles);
        isthep.resize(n_of_particles);
        idhep.resize(n_of_particles);
        jmohep.resize(n_of_particles);
        jdahep.resize(n_of_particles);
        phep.resize(n_of_particles);
        vhep.resize(n_of_particles);
    };
};

typedef struct
{
    int nparhep[20000];
    int isthep[20000];
    int idhep[20000];
    int jmohep[20000][2];
    int jdahep[20000][2];
    double phep[20000][5];
    double vhep[20000][4];

} HEPEVT;

int main(int argc, char *argv[])
{
    auto begin = std::chrono::steady_clock::now();

    // reading the .hepevt file
    ifstream inputFile;
    inputFile.open(argv[1], ios::binary);

    // creating the root file
    TFile *outputfile = TFile::Open("example.root", "recreate");
    vector<int> lep_ids;
    vector<double> lep_px;
    vector<double> lep_py;
    vector<double> lep_pz;
    vector<double> lep_e;
    int n_OfLeptons;
    TTree *leptonsTree = new TTree("leptons", "Z+3jets  leptons");
    leptonsTree->Branch("n_OfLeptons", &n_OfLeptons);
    leptonsTree->Branch("lep_ids", &lep_ids);
    leptonsTree->Branch("lep_px", &lep_px);
    leptonsTree->Branch("lep_py", &lep_py);
    leptonsTree->Branch("lep_pz", &lep_pz);
    leptonsTree->Branch("lep_e", &lep_e);

    int NEVHEP; // Event number
    int NHEP;   // Number of particles in the event
    while (inputFile.good())
    {
        int lepton_counter = 0;
        inputFile >> NEVHEP >> NHEP;
        if (inputFile.eof())
        {
            break;
        }

        myHEPEVT event(NHEP);
        for (int n = 0; n < NHEP; n++)
        {

            // inputFile >> event.nparhep[n] >> event.isthep[n] >> event.idhep[n] >> event.jmohep[n][0] >> event.jmohep[n][1] >> event.jdahep[n][0] >> event.jdahep[n][1] >> event.phep[n][0] >> event.phep[n][1] >> event.phep[n][2] >> event.phep[n][3] >> event.phep[n][4] >> event.vhep[n][0] >> event.vhep[n][1] >> event.vhep[n][2] >> event.vhep[n][3];

            inputFile >> event.nparhep[n] >> event.isthep[n] >> event.idhep[n] >> event.jmohep[n].first >> event.jmohep[n].second >>
                event.jdahep[n].first >> event.jdahep[n].second >>
                event.phep[n].px >> event.phep[n].py >> event.phep[n].pz >> event.phep[n].e >> event.phep[n].mass >>
                event.vhep[n].xx >> event.vhep[n].yy >> event.vhep[n].zz >> event.vhep[n].prod_time;

            // status code of 1 translates to stable final state
            if ((event.isthep[n] == 1) && (abs(event.idhep[n]) == 11 || abs(event.idhep[n]) == 13))
            {
                lepton_counter++;
                lep_ids.push_back(event.idhep[n]);
                lep_px.push_back(event.phep[n].px);
                lep_py.push_back(event.phep[n].py);
                lep_pz.push_back(event.phep[n].pz);
                lep_e.push_back(event.phep[n].e);
            }
        }

        n_OfLeptons = lepton_counter;
        leptonsTree->Fill();

        lep_ids.clear();
        lep_px.clear();
        lep_py.clear();
        lep_pz.clear();
        lep_e.clear();
    }

    inputFile.close();

    outputfile->Write();
    outputfile->Close();

    auto end = std::chrono::steady_clock::now();

    cout << "Running time: " << chrono::duration_cast<chrono::minutes>(end - begin).count() << endl;
    return 0;
}
