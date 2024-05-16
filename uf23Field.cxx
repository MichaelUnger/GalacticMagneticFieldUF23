/** @file uf23Field.cxx

 @brief  example program to calculate the coherent Galactic magnetic field
 @return an integer 0 upon success

 command line parameters are

    <model name> <x> <y> <z>

 where x/y/z are the galactocentric coordinates in kpc (Earth at negative x,
 North at positive z) and the model is one of either base, cre10, expX, neCL,
 nebCor, spur, synCG or twistX (see arXiv:2311.12120 for details)

 prints the three components of the coherent field in microgauss

 note: this is just a test program. If many evaluations are needed
 it is very inefficient to call this program repeatedly due to
 the overhead of initialization. For that case it is better
 to implement a loop over positions below.

 Please send bugs and suggestions to michael.unger@kit.edu and gf25@nyu.edu

 If you use this code, please cite arXiv:2311.12120

*/

#include "UF23Field.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

void
usage(const string& progName, const vector<string>& models)
{
  cerr << " usage: " << progName << " <model name> <x> <y> <z> \n"
       << "        " << " galactocentric coordinates x/y/z in kpc\n"
       << "        " << " (Earth at negative x, North at positive z)\n"
       << "        " << " available models: ";
  for (const auto& m : models)
    cerr << m << (&m != &models.back() ? ", " : "\n");
}

int
main(const int argc, const char** argv)
{
  const map<string, UF23Field::ModelType> uf23Models =
    {
     {"base", UF23Field::base},
     {"neCL", UF23Field::neCL},
     {"expX", UF23Field::expX},
     {"spur", UF23Field::spur},
     {"cre10", UF23Field::cre10},
     {"synCG", UF23Field::synCG},
     {"twistX", UF23Field::twistX},
     {"nebCor", UF23Field::nebCor}
    };

  vector<string> modelNames;
  for (const auto& iter : uf23Models)
    modelNames.push_back(iter.first);

  if (argc < 5 || !uf23Models.count(argv[1])) {
    usage(argv[0], modelNames);
    return 1;
  }

  Vector3 position;
  try {
    position.Set(stod(argv[2]),
                 stod(argv[3]),
                 stod(argv[4]));
  }
  catch (...) {
    usage(argv[0], modelNames);
    return 2;
  }

  const UF23Field uf23Field(uf23Models.at(argv[1]));
  const auto field = uf23Field(position);
  cout << scientific << setprecision(4)
       << "(x,y,z)    = (" << position << ") kpc\n"
       << "(bx,by,bz) = (" << field << ") microgauss" << endl;
  return 0;
}
