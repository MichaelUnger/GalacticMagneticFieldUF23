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
using ModelType = UF23Field::ModelType;
int readCommandLine(const int, const char**, int&, Vector3&, ModelType&);


int
main(const int argc, const char** argv)
{
  // read command line parameters
  int nSamples;
  Vector3 position;
  ModelType model;
  if (const int iError = readCommandLine(argc, argv, nSamples, position, model))
    return iError;

  // UF23 model and parameter covariance
  UF23Field uf23Field(model);
  const ParameterCovariance pcov(model);
  const unsigned int dim = pcov.GetDimension();

  // print nominal value
  const auto field = uf23Field(position);
  cout << scientific << setprecision(4)
       << " (x,y,z) = (" << position << ") kpc" << endl;
  cout << " central value: (bx,by,bz) = (" << field << ") microgauss" << endl;

  // remember the original parameters
  const auto centralValues = uf23Field.GetParameters();

  // generator for standard-normal distributed random numbers
  const int seed = 123;
  std::mt19937_64 engine(seed);
  std::normal_distribution<double> ndist;
  auto gen = [&ndist, &engine](){ return ndist(engine); };

  // mapping of elements in parameter vector and covariance matrix
  const auto indices = pcov.GetParameterIndices();

  // draw n samples of the parameters
  for (int i = 0; i < nSamples; ++i) {
    // vector of standard-normal random numbers
    vector<double> normal(dim);
    generate(begin(normal), end(normal), gen);
    // add random delta from covariance to the nominal parameter values
    const auto delta = pcov.GetRandomDelta(normal);
    auto sampledParameters = centralValues;
    for (unsigned int j = 0; j < delta.size(); ++j) {
      const auto index = indices[j];
      sampledParameters[index] += delta[j];
    }
    // evaluate UF23 field with new parameters
    uf23Field.SetParameters(sampledParameters);
    const auto field = uf23Field(position);
    cout << " (" << field
         << ")" << endl;
  }
  return 0;
}


void
usage(const string& progName)
{
  cerr << " usage: " << progName << " <model name> <x> <y> <z> <n>\n"
       << "        " << " n samples at galactocentric coordinates x/y/z (kpc)\n"
       << "        " << " (Earth at negative x, North at positive z)\n"
       << "        " << " available models: ";
  const auto& modelNames = UF23Field::GetModelNames();
  for (const auto& m : modelNames)
    cerr << m.second << " ";
  cout << endl;
}


int
readCommandLine(const int argc, const char** argv,
                int& nSamples, Vector3& position, ModelType& model)
{
  const auto& modelNames = UF23Field::GetModelNames();
  map<string, ModelType> uf23Models;
  for (const auto& mn : modelNames)
    uf23Models[mn.second] = mn.first;

  if (argc < 5 || !uf23Models.count(argv[1])) {
    usage(argv[0]);
    return 1;
  }

  model = uf23Models.at(argv[1]);

  try {
    nSamples = stoi(argv[5]);
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
