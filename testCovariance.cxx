#include "ParameterCovariance.h"

#include <iostream>
#include <iomanip>
using namespace std;

bool
CloseTo(const double a, const double b, const double relTol = 1e-5) {
  return std::abs(a - b) <= relTol * std::max(std::abs(a), std::abs(b));
}

int
main(const int /*argc*/, const char** /*argv*/)
{
  const vector<UF23Field::ModelType> models =
    {
     UF23Field::base //, UF23Field::neCL, UF23Field::expX, UF23Field::spur,
     //     UF23Field::cre10, UF23Field::synCG, UF23Field::twistX, UF23Field::nebCor
    };

  // original MINOS (up+low)/2 values
  vector<vector<double>> parUncertainties;
  // base
  parUncertainties.push_back({1.39562e-01,2.07490e-01,1.50666e-01,
                              8.50628e+00,2.79908e+00,2.17837e+00,1.29000e-01,
                              3.13721e-01,2.95585e-01,1.71916e-01,4.01536e-01,
                              6.98928e-01,3.31716e-02,9.21029e-02,5.67779e-02,
                              2.85741e-02,4.03012e-01,3.23158e-02,2.54924e-02,
                              3.35535e-02});

  for (unsigned int iModel = 0; iModel < models.size(); ++iModel) {
    const auto& model = models[iModel];
    const ParameterCovariance pcov(model);
    const auto& covMatrix = pcov.GetCovarianceMatrix();
    const auto& sigmas = parUncertainties[iModel];
    if (covMatrix.size() != sigmas.size()) {
      cerr << " inconsistent dimensions " << endl;
      return 1;
    }
    for (unsigned int iPar = 0; iPar < covMatrix.size(); ++iPar) {
      const double sigma = sigmas[iPar];
      const double diag = covMatrix[iPar][iPar];
      if (!CloseTo(sigma, sqrt(diag))) {
        cerr << scientific << setprecision(10) << "parameter " << iPar
             << ": (" << sigma << ") not close to (" << sqrt(diag) << ")"
             << endl;
        return 2;
      }
      /*
      for (unsigned int jPar = 0; jPar < covMatrix.size(); ++jPar) {
        const double denom = sqrt(covMatrix[jPar][jPar]*covMatrix[iPar][iPar]);
        const double rho = covMatrix[iPar][jPar] / denom;
        cout << setw(4) << round(rho*100);
      }
      cout << endl;
      */
    }

  }

  cout << " ==> test of covariance successful " << endl;
  return 0;
}
