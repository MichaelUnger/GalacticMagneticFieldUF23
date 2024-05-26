/** @file sampleUF23Field.cxx

 @brief  Example program demonstrating the use of parameter sampling.
         Progates GMF parameter uncertainties to line-of-sight integrals.
 @return an integer 0 upon success

 command line parameters are

    <model name> <l> <b>

 where "l" and "b" denote the Galactic longitude and latitude in
 degrees of the direction to integrate over and the model is one of
 either base, cre10, expX, neCL, nebCor, spur, synCG or twistX (see
 arXiv:2311.12120 for details).

 Prints the line-of-sight integrals of B_\parallel and B_\perp^2
 and their standard deviation due to the parameter uncertainties.

 Please send bugs and suggestions to michael.unger@kit.edu and gf25@nyu.edu

 If you use this code, please cite arXiv:2311.12120

*/

#include "UF23Field.h"
#include "ParameterCovariance.h"

#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

using namespace std;
using ModelType = UF23Field::ModelType;
int readCommandLine(const int, const char**, Vector3&, ModelType&);
pair<double, double> losIntegral(const UF23Field&, const Vector3&,
                                 const Vector3&, const double);

const double kPi = 3.1415926535897932384626;
const double degree = kPi/180.;

int
main(const int argc, const char** argv)
{
  // position of observer
  const Vector3 sunPos(-8.178, 0, 0);

  // parameters for integration and error propagation
  const unsigned int nSamples = 1000;
  const double dL = 0.01; // 10 pc step length

  // command line parameters
  Vector3 uLos;
  ModelType model;
  if (const int iError = readCommandLine(argc, argv, uLos, model))
    return iError;

  // UF23 model
  UF23Field uf23Field(model);
  const auto nominalLos = losIntegral(uf23Field, sunPos, uLos, dL);

  // parameter covariance
  const ParameterCovariance pcov(model);
  const unsigned int dim = pcov.GetDimension();

  // copy original parameters
  const auto centralValues = uf23Field.GetParameters();

  // generator for standard-normal distributed random numbers
  const int seed = 123;
  std::mt19937_64 engine(seed);
  std::normal_distribution<double> ndist;
  auto gen = [&ndist, &engine](){ return ndist(engine); };

  // mapping of elements in parameter vector and covariance matrix
  const auto indices = pcov.GetParameterIndices();

  // draw n samples of the parameters and accumulate sums for 1st and 2nd moment
  double sumPara = 0;  double sumPara2 = 0;
  double sumPerp = 0;  double sumPerp2 = 0;

  for (unsigned int i = 0; i < nSamples; ++i) {
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
    // set new parameters UF23 parameters
    uf23Field.SetParameters(sampledParameters);
    auto los = losIntegral(uf23Field, sunPos, uLos, dL);

    sumPara += los.first;  sumPara2 += pow(los.first, 2);
    sumPerp += los.second; sumPerp2 += pow(los.second, 2);
  }

  // variance = central second moment
  const double vPar = sumPara2 / nSamples - pow(sumPara / nSamples, 2);
  const double vPerp = sumPerp2 / nSamples - pow(sumPerp / nSamples, 2);

  const int width = 11;
  cout << "==> \\int_0^\\infty B_\\parallel dl = ("
       << scientific << setprecision(4)
       << setw(width)
       << nominalLos.first << " +/- "
       << sqrt(vPar)
       << ") microGauss kpc" << endl;
  cout << "==> \\int_0^\\infty B_\\perp^2 dl   = ("
       << scientific << setprecision(4)
       << setw(width)
       << nominalLos.second << " +/- "
       << sqrt(vPerp)
       << ") microGauss^2 kpc\n" << endl;

  return 0;
}


// very simple integral, just for demonstration
std::pair<double, double>
losIntegral(const UF23Field& magField, const Vector3& startPos,
            const Vector3& direction, const double dL)
{
  double sumPara = 0;
  double sumPerp = 0;
  const double rMax2 = magField.GetMaximumSquaredRadius();
  Vector3 pos = startPos;
  double l = 0;
  while (pos.SquaredLength() < rMax2) {
    const auto b = magField(pos);
    const double bPara = dotprod(b, direction);
    const Vector3 bProj = crossprod(direction, crossprod(b, direction));
    const double bPerp2 = bProj.SquaredLength();
    sumPara += bPara;
    sumPerp += bPerp2;
    l += dL;
    pos = startPos + direction * l;
  }
  return make_pair(sumPara*dL, sumPerp*dL);
}


void
usage(const string& progName)
{
  cerr << " usage: " << progName << " <model name> <l> <b>\n"
       << "        " << " line-of-sight direction: longitude l and "
       << " latitude b (degree)\n"
       << "        " << " available models: ";
  const auto& modelNames = UF23Field::GetModelNames();
  for (const auto& m : modelNames)
    cerr << m.second << " ";
  cout << endl;
}


int
readCommandLine(const int argc, const char** argv,
                Vector3& uLos, ModelType& model)
{
  const auto& modelNames = UF23Field::GetModelNames();
  map<string, ModelType> uf23Models;
  for (const auto& mn : modelNames)
    uf23Models[mn.second] = mn.first;

  if (argc < 4 || !uf23Models.count(argv[1])) {
    usage(argv[0]);
    return 1;
  }

  model = uf23Models.at(argv[1]);

  try {
    const double longi = stod(argv[2])*degree;
    const double lat = stod(argv[3])*degree;
    cout << "line-of-sight direction: (l, b) = ("
         << longi / degree << ", " << lat / degree
         << ") degree\n" << endl;
    const double rxy = cos(lat);
    const double x = cos(longi) * rxy;
    const double y = sin(longi) * rxy;
    const double z = sin(lat);
    uLos = Vector3(x, y, z);
  }
  catch (...) {
    usage(argv[0]);
    return 2;
  }
  return 0;
}
