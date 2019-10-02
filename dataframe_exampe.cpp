#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string> // std::string
#include <ROOT/TTree.h>
#include <ROOT/TFile.h>
#include <ROOT/TLorentzVector.h>
#include <ROOT/RDataFrame.hxx>

using namespace std;
using namespace ROOT;

int main(int argc, char *argv[])
{
  auto filename = argv[1];
  auto treeName = "particles";

  ROOT::RDataFrame d(treeName, filename);

  auto colNames = d.GetColumnNames();
  for (auto &&colName : colNames)
  {
    cout << colName << endl;
  }

  cout << colNames.size() << endl;

  auto print = [](int &n) { cout << n << endl; };
  auto c = d.Map(print, {"n_particles"});

  return 0;
}
