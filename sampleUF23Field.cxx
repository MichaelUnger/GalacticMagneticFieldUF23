/** @file sampleUF23Field.cxx

 @brief  Example program demonstrating the use of parameter sampling
         Progates GMF parameter uncertainties to line-of-sight integrals
 @return an integer 0 upon success

 command line parameters are

    <model name> <l> <b>

 where "l" and "b" denote the Galactic longitude and latitude in
 degrees of the direction to integrate over and the model is one of
 either base, cre10, expX, neCL, nebCor, spur, synCG or twistX (see
 arXiv:2311.12120 for details)

 prints the line-of-sight integrals of B_\parallel, B_\perp and B_\perp^2
 and their uncertainties due to the parameter uncertainties.

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
  // parameters for integration and error propagation
  const unsigned int nSamples = 1000;
  const double dL = 0.1;
  const Vector3 sunPos(-8.178, 0, 0);

  // command line parameters
  Vector3 uLos;
  ModelType model;
  if (const int iErr = readCommandLine(argc, argv, uLos, model))
    return iErr;

  // UF23 model
  UF23Field uf23Field(model);
  auto los = losIntegral(uf23Field, sunPos, uLos, dL);
  const int colWidth = 22;
  const int firstColWidth = 20;
  cout << setw(firstColWidth) << " "
       << setw(colWidth) << "\\int B_\\parallel dl"
       << setw(colWidth) << "\\int B_\\perp^2 dl \n";
  cout << setw(firstColWidth) << " "
       << setw(colWidth) << "(microGauss kpc)"
       << setw(colWidth) << "(microGauss^2 kpc)\n";
  ostringstream hrule;
  hrule << string(firstColWidth, '-') << "+"
        << string(colWidth, '-') << "+"
        << string(colWidth, '-') <<"+\n";
  cout << hrule.str();
  cout << setw(firstColWidth)  << " nominal parameters "  << "|"
       << scientific << setprecision(4)
       << setw(colWidth) << los.first << "|"
       << setw(colWidth) << los.second << "|" << endl;

  // parameter covariance
  const ParameterCovariance pcov(model);
  const unsigned int dim = pcov.GetDimension();

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
  double sumParallel = 0;  double sumParallel2 = 0;
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

    sumParallel += los.first;  sumParallel2 += pow(los.first, 2);
    sumPerp += los.second; sumPerp2 += pow(los.second, 2);
  }

  // mean and variance
  const double muPar = sumParallel / nSamples;
  const double vPar = sumParallel2 / nSamples - pow(muPar, 2);
  const double muPerp = sumPerp / nSamples;
  const double vPerp = sumPerp2 / nSamples - pow(muPerp, 2);

  cout << setw(firstColWidth)  << " standard deviation " << "|"
       << scientific << setprecision(4)
       << setw(colWidth) << sqrt(vPar)  << "|"
       << setw(colWidth) << sqrt(vPerp)  << "|" << endl;
  cout << hrule.str() << endl;

  return 0;
}


std::pair<double, double>
losIntegral(const UF23Field& magField, const Vector3& startPos,
            const Vector3& direction, const double dL)
{
  double sumParallel = 0;
  double sumPerpendicular2 = 0;
  const double rMax2 = magField.GetMaximumSquaredRadius();
  Vector3 pos = startPos;
  double l = 0;
  while (pos.SquaredLength() < rMax2) {
    const auto b = magField(pos);
    const double bParallel = dotprod(b, direction);
    const Vector3 bProj = crossprod(direction, crossprod(b, direction));
    const double bPerp2 = bProj.SquaredLength();
    sumParallel += bParallel;
    sumPerpendicular2 += bPerp2;
    l += dL;
    pos = startPos + direction * l;
  }
  return make_pair(sumParallel*dL, sumPerpendicular2*dL);
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
         << ") degree" << endl;
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
