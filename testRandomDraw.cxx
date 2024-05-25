#include "ParameterCovariance.h"

#include <random>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

bool
CloseTo(const double a, const double b, const double relTol = 1e-5) {
  return std::abs(a - b) <= relTol * std::max(std::abs(a), std::abs(b));
}

int
main()
{
  const vector<UF23Field::ModelType> models =
    {
     UF23Field::base, UF23Field::neCL, UF23Field::expX,
     UF23Field::spur, UF23Field::cre10, UF23Field::synCG,
     UF23Field::twistX, UF23Field::nebCor
    };

  const unsigned int nDraw = 1e5;

  // accuracy depends on number of draws
  // variances: relative error
  const double uncertainty = sqrt(2./(nDraw+1)); // 1 sigma
  const double relTolVar = 4 * uncertainty; // 4 sigma, since we test O(1e3)
  // off diagonal : at least 1% accuracy for 1e6 samples
  const double maxDeltaRho = 0.01*1000/sqrt(nDraw);


  const int seed = 123;
  std::mt19937_64 engine(seed);
  std::normal_distribution<double> ndist;
  auto gen = [&ndist, &engine](){ return ndist(engine); };

  int nTest = 0;
  for (const auto& model : models) {
    cout << " " << UF23Field::GetModelName(model) << " ..." << flush;
    const ParameterCovariance pcov(model);
    const unsigned int n = pcov.GetDimension();
    vector<double> normal(n);

    // calculate sample covariance for nDraw draws ...
    vector<double> cov((n*(n+1))/2, 0.);
    for (unsigned int iDraw = 0; iDraw < nDraw; ++iDraw) {
      generate(begin(normal), end(normal), gen);
      const auto delta = pcov.GetRandomOffset(normal);
      int k = 0;
      for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j <= i; ++j) {
          cov[k] += delta[i] * delta[j];
          ++k;
        }
      }
    }
    for (auto& c : cov)
      c /= (nDraw - 1);

    // ... and compare to nominal covariance
    const auto& V = pcov.GetCovarianceMatrix();
    int k = 0;
    for (unsigned int i = 0; i < n; ++i) {
      const double Vii = V[i][i];
      for (unsigned int j = 0; j <= i; ++j) {
        ++nTest;
        // check relative uncertainty for variances
        if (i == j) {
          if (!CloseTo(V[i][j], cov[k], relTolVar)) {
            cerr << scientific << setprecision(10) << "sampled covariance "
                 << cov[k] << " not close to " << V[i][j]
                 << ", i=" << i  << ", j=" << j << endl;
            return 1;
          }
        }
        // check absolute difference of correlation coefficient
        else {
          const double Vjj = V[j][j];
          const double denom = sqrt(Vii*Vjj);
          const double rho1 = V[i][j] / denom;
          const double rho2 = cov[k] / denom;
          if (std::abs(rho1 - rho2) > maxDeltaRho) {
            cerr << scientific << setprecision(10) << "sampled correlation "
                 << rho1 << " not close to " << rho2
                 << ", i=" << i  << ", j=" << j << endl;
            return 2;
          }
        }
        ++k;
      }
    }
    cout << " ok " << endl;
  }
  cout << " ==> test of " << nTest << " matrix elements successful" << endl;
}
