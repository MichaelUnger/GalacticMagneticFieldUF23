/** @file calcUF23Field.cxx

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
 to implement a loop over positions.

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
using ModelType = UF23Field::ModelType;
int readCommandLine(const int, const char**, Vector3&, ModelType&);

int
main(const int argc, const char** argv)
{
  // read and check command line parameters
  Vector3 position;
  ModelType model;
  if (const int iError = readCommandLine(argc, argv, position, model))
    return iError;

  // evaluate the corresponding UF23Field model at requested position
  const UF23Field uf23Field(model);
  const auto field = uf23Field(position);
  cout << scientific << setprecision(4)
       << "(x,y,z)    = (" << position << ") kpc\n"
       << "(bx,by,bz) = (" << field << ") microgauss" << endl;
  return 0;
}

void
usage(const string& progName)
{
  cerr << " usage: " << progName << " <model name> <x> <y> <z> \n"
       << "        " << " galactocentric coordinates x/y/z in kpc\n"
       << "        " << " (Earth at negative x, North at positive z)\n"
       << "        " << " available models: ";
  const auto& modelNames = UF23Field::GetModelNames();
  for (const auto& m : modelNames)
    cerr << m.second << " ";
  cout << endl;
}

int
readCommandLine(const int argc, const char** argv,
                Vector3& position, ModelType& model)
{
  const auto& modelNames = UF23Field::GetModelNames();
  map<string, UF23Field::ModelType> uf23Models;
  for (const auto& mn : modelNames)
    uf23Models[mn.second] = mn.first;

  if (argc < 5 || !uf23Models.count(argv[1])) {
    usage(argv[0]);
    return 1;
  }

  model = uf23Models.at(argv[1]);

  try {
    position.Set(stod(argv[2]),
                 stod(argv[3]),
                 stod(argv[4]));
  }
  catch (...) {
    usage(argv[0]);
    return 2;
  }
  return 0;
}
