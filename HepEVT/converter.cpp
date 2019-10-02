/* 
A simple script for converting hepevt format files to root files.
Z + 1 jet events with Z decaying to leptons is used. Only particles with stable final states will be saved.
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string> // std::string
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
    auto begin = chrono::steady_clock::now();

    // reading the .hepevt file
    ifstream inputFile;
    inputFile.open(argv[1], ios::binary);

    // creating the root file
    TFile *outputfile = TFile::Open("example.root", "recreate");
    // declaring the vectors used for storing the particle id and kinematic variables
    vector<int> par_ids;
    vector<double> par_px, par_py, par_pz, par_e;
    int n_particles;
    TTree *leptonsTree = new TTree("particles", "Z+1jets  particles");
    leptonsTree->Branch("n_particles", &n_particles);
    leptonsTree->Branch("par_ids", &par_ids);
    leptonsTree->Branch("par_px", &par_px);
    leptonsTree->Branch("par_py", &par_py);
    leptonsTree->Branch("par_pz", &par_pz);
    leptonsTree->Branch("par_e", &par_e);

    int NEVHEP; // Event number
    int NHEP;   // Number of particles in the event
    while (inputFile.good())
    {
        int particle_counter = 0;
        inputFile >> NEVHEP >> NHEP;
        if (inputFile.eof())
        {
            break;
        }

        myHEPEVT event(NHEP);
        for (int n = 0; n < NHEP; n++)
        {
            // Using the struct to store the event
            // inputFile >> event.nparhep[n] >> event.isthep[n] >> event.idhep[n] >> event.jmohep[n][0] >> event.jmohep[n][1] >>
            //     event.jdahep[n][0] >> event.jdahep[n][1] >> event.phep[n][0] >> event.phep[n][1] >> event.phep[n][2] >> event.phep[n][3] >> event.phep[n][4] >>
            //     event.vhep[n][0] >> event.vhep[n][1] >> event.vhep[n][2] >> event.vhep[n][3];

            // Using the class to store the event
            inputFile >> event.nparhep[n] >> event.isthep[n] >> event.idhep[n] >> event.jmohep[n].first >> event.jmohep[n].second >>
                event.jdahep[n].first >> event.jdahep[n].second >>
                event.phep[n].px >> event.phep[n].py >> event.phep[n].pz >> event.phep[n].e >> event.phep[n].mass >>
                event.vhep[n].xx >> event.vhep[n].yy >> event.vhep[n].zz >> event.vhep[n].prod_time;

            // status code of 1 translates to stable final state
            if (event.isthep[n] == 1)
            {
                particle_counter++;
                par_ids.push_back(event.idhep[n]);
                par_px.push_back(event.phep[n].px);
                par_py.push_back(event.phep[n].py);
                par_pz.push_back(event.phep[n].pz);
                par_e.push_back(event.phep[n].e);
            }
        }

        n_particles = particle_counter;
        leptonsTree->Fill();
        n_particles = 0;
        par_ids.clear();
        par_px.clear();
        par_py.clear();
        par_pz.clear();
        par_e.clear();
    }

    inputFile.close();

    outputfile->Write();
    outputfile->Close();

    auto end = chrono::steady_clock::now();

    cout << "Running time: " << chrono::duration_cast<chrono::minutes>(end - begin).count() << endl;
    return 0;
}
