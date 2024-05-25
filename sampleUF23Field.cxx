/** @file sampleUF23Field.cxx

 @brief  example program to obtain a sample of magnetic field values
         distributed according to fitted parameter uncertainties
 @return an integer 0 upon success

 command line parameters are

    <model name> <x> <y> <z> <n>

 where x/y/z are the galactocentric coordinates in kpc (Earth at negative x,
 North at positive z) and the model is one of either base, cre10, expX, neCL,
 nebCor, spur, synCG or twistX (see arXiv:2311.12120 for details)
 and n is the number of samples

 prints the three components of the coherent field in microgauss

 note: this is just a test program demonstrating the sampling. For
 standard applications, like particle tracking or line of sight integrals,
 the desired calculation needs to be done for each parameter realization
 separately and then repeated.

 Please send bugs and suggestions to michael.unger@kit.edu and gf25@nyu.edu

 If you use this code, please cite arXiv:2311.12120

*/

#include "UF23Field.h"
#include "ParameterCovariance.h"

#include <string>
#include <algorithm>
#include <random>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

using namespace std;

void
usage(const string& progName, const vector<string>& models)
{
  cerr << " usage: " << progName << " <model name> <x> <y> <z> <n>\n"
       << "        " << " n samples at galactocentric coordinates x/y/z (kpc)\n"
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

  int n;
  Vector3 position;
  try {
    n = stoi(argv[5]);
    position.Set(stod(argv[2]),
                 stod(argv[3]),
                 stod(argv[4]));
  }
  catch (...) {
    usage(argv[0], modelNames);
    return 2;
  }

  const auto model = uf23Models.at(argv[1]);
  UF23Field uf23Field(model);
  const ParameterCovariance pcov(model);

  const auto field = uf23Field(position);
  cout << scientific << setprecision(4)
       << " (x,y,z) = (" << position << ") kpc" << endl;
  cout << " central value: (bx,by,bz) = (" << field << ") microgauss" << endl;

  const auto centralValues = uf23Field.GetParameters();

  const int seed = 123;
  std::mt19937_64 engine(seed);
  std::normal_distribution<double> ndist;
  auto gen = [&ndist, &engine](){ return ndist(engine); };

  const unsigned int dim = pcov.GetDimension();
  vector<double> normal(dim);
  const auto indices = pcov.GetParameterIndices();

  for (int i = 0; i < n; ++i) {
    generate(begin(normal), end(normal), gen);
    const auto delta = pcov.GetRandomDelta(normal);
    auto sampledParameters = centralValues;
    for (unsigned int j = 0; j < delta.size(); ++j) {
      const auto index = indices[j];
      sampledParameters[index] = centralValues[index] + delta[j];
    }
    uf23Field.SetParameters(sampledParameters);
    const auto field = uf23Field(position);
    cout << " (" << field
         << ")" << endl;
  }
  return 0;
}
