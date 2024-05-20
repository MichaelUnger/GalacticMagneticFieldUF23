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
     UF23Field::base, UF23Field::neCL, //, UF23Field::expX,
     UF23Field::spur, UF23Field::cre10, UF23Field::synCG,
     UF23Field::twistX, UF23Field::nebCor
    };

  // original MINOS (up+low)/2 values
  vector<vector<double>> parUncertainties;
  // base
  parUncertainties.push_back({1.39562e-01, 2.07490e-01, 1.50666e-01,
                              8.50628e+00, 2.79908e+00, 2.17837e+00,
                              1.29000e-01, 3.13721e-01, 2.95585e-01,
                              1.71916e-01, 4.01536e-01, 6.98928e-01,
                              3.31716e-02, 9.21029e-02, 5.67779e-02,
                              2.85741e-02, 4.03012e-01, 3.23158e-02,
                              2.54924e-02, 3.35535e-02});
  // neCL
  parUncertainties.push_back({2.60831e-01, 4.42244e-01, 3.35928e-01,
                              1.10198e+01, 1.15391e+01, 4.40871e+00,
                              3.81541e-01, 1.68869e-01, 1.70805e-01,
                              1.96978e-01, 2.90310e-01, 8.15647e-01,
                              3.14437e-02, 1.06173e-01, 4.78309e-02,
                              2.97868e-02, 2.80728e-01, 1.82163e-02,
                              1.95662e-02, 2.93311e-02});
  // spur
  parUncertainties.push_back({1.78487e-01, 2.96851e+00, 3.02899e+00,
                              1.39776e+00, 6.00781e-01, 6.03047e-01,
                              2.31994e-01, 2.09938e-01, 1.26714e-01,
                              3.00170e-01, 1.44234e+00, 3.70432e-02,
                              9.42209e-02, 6.26714e-02, 3.42806e-02,
                              4.21980e-01, 2.74039e-02, 2.37013e-02,
                              3.33899e-02});
  // cre10
  parUncertainties.push_back({1.37551e-01, 2.12666e-01, 1.54310e-01,
                              7.63918e+00, 2.78251e+00, 2.16503e+00,
                              1.27801e-01, 3.67982e-01, 3.44655e-01,
                              1.98773e-01, 4.27906e-01, 3.68252e-01,
                              3.37385e-02, 9.22778e-02, 6.17850e-02,
                              2.68308e-02, 4.15623e-01, 3.25121e-02,
                              2.46556e-02, 3.25265e-02});

  // synCG
  parUncertainties.push_back({1.15009e-01, 1.95630e-01, 1.37095e-01,
                              1.25124e+01, 3.32470e+00, 2.37141e+00,
                              1.32221e-01, 1.21392e-01, 1.07223e-01,
                              7.59803e-02, 1.60971e-01, 8.17610e-01,
                              2.43071e-02, 9.39734e-02, 4.78600e-02,
                              2.15930e-02, 2.41330e-01, 1.76676e-02,
                              1.81933e-02, 3.60745e-02});
  // twistX
  parUncertainties.push_back({1.72433e-01, 3.02179e-01, 2.58401e-01,
                              1.14489e+01, 1.02541e+01, 6.05790e+00,
                              4.42235e-01, 1.96082e-02, 9.94098e-02,
                              3.55133e-02, 3.49911e-02, 9.74237e-02,
                              1.08954e+00, 4.79921e-02, 6.92482e-02,
                              3.71599e-02});
  // nebCor
  parUncertainties.push_back({1.86530e-01, 2.70900e-01, 2.08219e-01,
                              8.47780e+00, 3.22057e+00, 2.47712e+00,
                              1.43782e-01, 4.09198e-01, 3.99025e-01,
                              1.72479e-01, 4.05704e-01, 5.76313e-01,
                              3.64884e-02, 9.78361e-02, 6.55231e-02,
                              3.30546e-02, 5.12207e-01, 2.69670e-02,
                              2.48811e-02});

  for (unsigned int iModel = 0; iModel < models.size(); ++iModel) {
    const auto& model = models[iModel];
    cout << " " << UF23Field::GetModelName(model) << " ..." << flush;
    const ParameterCovariance pcov(model);
    const auto& covMatrix = pcov.GetCovarianceMatrix();
    const auto& sigmas = parUncertainties[iModel];
    if (covMatrix.size() != sigmas.size()) {
      cerr << " inconsistent dimensions " << endl;
      return 1;
    }
    if (covMatrix.size() != pcov.GetParameterIndices().size()) {
      cerr << " matrix dimension != index size " << endl;
      return 2;
    }

    for (unsigned int iPar = 0; iPar < covMatrix.size(); ++iPar) {
      const double sigma = sigmas[iPar];
      const double diag = covMatrix[iPar][iPar];
      if (!CloseTo(sigma, sqrt(diag))) {
        cerr << scientific << setprecision(10) << "parameter " << iPar
             << ": (" << sigma << ") not close to (" << sqrt(diag) << ")"
             << endl;
        return 3;
      }
    }
    cout << " ok " << endl;
  }

  cout << " ==> test of covariance successful " << endl;
  return 0;
}
