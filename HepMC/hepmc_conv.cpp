#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <ROOT/TTree.h>
#include <ROOT/TObject.h>
#include <ROOT/TFile.h>
#include <ROOT/TLorentzVector.h>
#include <ROOT/TParticle.h>
#include <ROOT/TParticlePDG.h>
#include <ROOT/TDatabasePDG.h>
#include <ROOT/TSystem.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <HepMC/GenVertex.h>
#include <HepMC/IO_GenEvent.h>
#include <type_traits>
#include <typeinfo>

using namespace HepMC;
using namespace std;

class myParticle
{
  public:
    TLorentzVector pvec4;
    myParticle(int _id, double _px, double _py, double _pz, double _e)
        : pdg_id(_id), px(_px), py(_py), pz(_pz), e(_e)
    {
        pvec4.SetPxPyPzE(px, py, pz, e);
    }

    myParticle(int _id, TLorentzVector vec4)
        : pdg_id(_id), px(vec4.Px()), py(vec4.Py()), pz(vec4.Pz()), e(vec4.E())
    {
        pvec4.SetPxPyPzE(px, py, pz, e);
    }

    int getId(void) const
    {
        return pdg_id;
    }

    double Px(void) const
    {
        return pvec4.Px();
    }

    double Py(void) const
    {
        return pvec4.Py();
    }

    double Pz(void) const
    {
        return pvec4.Pz();
    }

    double E(void) const
    {
        return pvec4.E();
    }

    double getPt(void) const
    {
        return pvec4.Pt();
    }

    double getEta(void) const
    {
        return pvec4.Eta();
    }

    double DeltaR(const TLorentzVector &v) const
    {
        return pvec4.DeltaR(v);
    }

    double DeltaR(const myParticle &p) const
    {
        return pvec4.DeltaR(p.pvec4);
    }

  private:
    int pdg_id;
    double px, py, pz, e;
};

class IsStateFinal
{
  public:
    /// returns true if the GenParticle does not decay
    bool operator()(const HepMC::GenParticle *p)
    {
        if (!p->end_vertex() && p->status() == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
};

int main(int argc, char **argv)
{

    // gSystem->Load("libEG");
    const auto begin = chrono::steady_clock::now();

    TFile *fo = new TFile("wz_fromHepMC.root", "RECREATE");
    int n_particles;
    vector<int> *par_ids = new vector<int>;
    vector<double> *par_px = new vector<double>, *par_py = new vector<double>, *par_pz = new vector<double>, *par_e = new vector<double>;

    TTree *particlesTree = new TTree("Particles", "Particles from HepMC");
    particlesTree->Branch("n_particles", &n_particles);
    particlesTree->Branch("par_ids", &par_ids);
    particlesTree->Branch("par_px", &par_px);
    particlesTree->Branch("par_py", &par_py);
    particlesTree->Branch("par_pz", &par_pz);
    particlesTree->Branch("par_e", &par_e);

    for (int i = 1; i < argc; i++)
    {
        HepMC::GenEvent *evt = new HepMC::GenEvent;
        HepMC::IO_GenEvent ascii_in(argv[i], std::ios::in);
        IsStateFinal isfinal;
        while (evt = ascii_in.read_next_event())
        {

            int particle_counter = 0;
            vector<myParticle> selected_particles;

            vector<HepMC::GenParticle *> finalstateparticles;
            for (HepMC::GenEvent::particle_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p)
            {
                if (isfinal(*p))
                {
                    finalstateparticles.push_back(*p);
                }
            }
            for (auto &fparticle : finalstateparticles)
            {
                selected_particles.emplace_back(fparticle->pdg_id(), fparticle->momentum().px(), fparticle->momentum().py(), fparticle->momentum().pz(), fparticle->momentum().e());
            }

            for (auto &par : selected_particles)
            {
                particle_counter++;
                par_ids->push_back(par.getId());
                par_px->push_back(par.Px());
                par_py->push_back(par.Py());
                par_pz->push_back(par.Pz());
                par_e->push_back(par.E());
            }

            n_particles = particle_counter;
            particlesTree->Fill();
            par_ids->clear();
            par_px->clear();
            par_py->clear();
            par_pz->clear();
            par_e->clear();

            par_ids->shrink_to_fit();
            par_px->shrink_to_fit();
            par_py->shrink_to_fit();
            par_pz->shrink_to_fit();
            par_e->shrink_to_fit();
            delete evt;

        } //reading file loop
    }

    fo->Write();
    fo->Close();
    delete par_ids;
    delete par_px;
    delete par_py;
    delete par_pz;
    delete par_e;
    delete fo;

    const auto end = chrono::steady_clock::now();

    cout << "Running time: " << std::chrono::duration_cast<chrono::minutes>(end - begin).count() << endl;
    return 0;
}
