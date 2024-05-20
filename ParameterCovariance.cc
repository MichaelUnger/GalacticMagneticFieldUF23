#include "ParameterCovariance.h"


namespace utl {

  using namespace std;

  // calculat V = L * L^T
  vector<std::vector<double>>
  VfromL(const std::vector<double>& L) {

    const unsigned int n = (sqrt(1+8*L.size()) - 1) / 2;
    vector<vector<double>> V(n, vector<double>(n, 0.));

    auto index = [](unsigned int i, unsigned int j) {
       return (i * (i + 1)) / 2 + j;
    };

    for (unsigned int i = 0; i < n; ++i) {
      for (unsigned int j = 0; j <= i; ++j) {
        double sum = 0;
        for (unsigned int k = 0; k <= std::min(i, j); ++k)
          sum += L[index(i, k)] * L[index(j, k)];
        V[i][j] = sum;
        if (i != j)
          V[j][i] = sum;
      }
    }
    return V;
  }
}

ParameterCovariance::ParameterCovariance(const UF23Field::ModelType mt) :
  fModelType(mt)
{

  using mf = UF23Field;

  switch (fModelType) {
  case UF23Field::base: {
    fL = {
           1.39562e-01, -6.54085e-02,  1.96911e-01,  2.66170e-03,  7.39673e-02,
           1.31233e-01,  4.38627e+00, -5.73125e+00,  3.96343e-01,  4.48478e+00,
          -2.76039e-01, -8.39084e-01, -1.27464e+00,  2.03763e+00,  1.13046e+00,
           6.70593e-01,  1.35176e-01, -3.82145e-01,  1.77802e+00,  8.54741e-01,
           4.89239e-01,  5.32912e-02, -2.53639e-02, -8.78323e-05,  9.60822e-02,
           5.59280e-02,  1.99711e-02,  1.99797e-02,  8.09574e-02, -5.22914e-02,
           1.04681e-01,  4.01155e-02,  1.24853e-02, -1.57598e-02,  3.80346e-02,
           2.73338e-01, -6.09083e-02,  5.35443e-02, -1.02965e-01, -4.51922e-02,
           5.21954e-03,  5.73934e-03, -4.37831e-02, -2.50744e-01,  5.74450e-02,
           4.31914e-03,  1.09432e-02,  1.91788e-02,  3.00872e-02, -1.02555e-02,
          -8.85333e-03,  3.31359e-02, -7.99971e-03,  3.82178e-04,  1.63701e-01,
           2.51630e-02,  2.73540e-02,  7.49086e-02,  5.13695e-02,  1.03048e-02,
          -1.91850e-02,  4.37730e-02,  5.99822e-02,  2.77696e-03,  1.79149e-01,
           3.36912e-01, -5.43659e-02,  6.73414e-02, -2.52585e-01, -1.28087e-01,
           9.72050e-03,  4.55441e-02, -1.02084e-01, -5.26943e-01,  8.65182e-02,
           1.38017e-02,  2.19318e-01,  2.34028e-01,  7.30502e-03, -5.96256e-03,
          -2.77896e-03, -1.96214e-03, -1.29403e-03,  3.57595e-03, -1.57752e-03,
           1.16935e-02, -3.15322e-03,  5.50079e-03,  2.58879e-03,  1.28416e-02,
           2.51867e-02, -7.80606e-04, -6.08968e-04, -3.31634e-03, -1.00560e-03,
          -1.90903e-03,  5.22164e-04,  2.15408e-03,  3.40526e-03,  6.10413e-03,
           5.25004e-03, -3.04494e-03, -1.19098e-03, -3.57288e-02,  8.42481e-02,
          -4.72065e-03,  4.26466e-03, -4.48180e-03,  1.10559e-03, -3.83847e-03,
          -7.47686e-05,  3.33721e-03, -2.21504e-03, -1.66635e-03, -2.13078e-03,
          -1.00434e-03, -9.41848e-03, -2.72426e-02,  2.75402e-02,  3.91601e-02,
          -1.38041e-03,  1.49083e-03, -7.93107e-05,  1.25911e-04, -4.67348e-04,
          -9.21439e-04,  9.43735e-04, -2.52732e-03,  1.10854e-03,  2.83980e-04,
          -1.17611e-05, -2.39549e-03,  9.44256e-03,  2.97175e-03,  2.94127e-03,
           2.62734e-02,  3.00997e-03,  9.66463e-03, -2.27886e-03,  2.20331e-03,
           4.40898e-03,  2.65953e-04, -9.37628e-03, -2.45932e-02, -2.22911e-02,
          -2.93409e-02,  2.67895e-02,  9.63532e-03,  1.64344e-01, -3.39396e-01,
           8.55500e-02,  7.72724e-03,  9.92386e-02,  1.19129e-02, -6.56950e-03,
           9.83770e-03,  4.00257e-03,  9.26317e-04, -1.47288e-03,  5.54780e-03,
           1.98617e-02,  7.23500e-04,  1.89532e-03, -1.19248e-03,  1.27725e-02,
          -2.40595e-03, -2.43196e-03, -1.39745e-03,  1.28483e-03,  1.70057e-03,
           1.14572e-02,  8.04304e-03, -4.29058e-03,  1.63228e-03, -7.40672e-04,
           1.03992e-03,  1.84515e-03,  2.39947e-03,  7.22288e-03,  4.45706e-03,
          -1.99596e-03, -9.43494e-04,  6.35893e-03, -6.28036e-03, -3.77930e-03,
          -2.35889e-03,  2.89818e-03,  3.10305e-03,  8.84379e-03,  1.67461e-02,
          -1.03959e-02,  8.63190e-03,  5.41233e-03,  3.96455e-03,  1.28257e-03,
          -4.17208e-03,  1.65391e-03, -1.58028e-02,  3.41188e-03, -8.83596e-03,
          -1.81508e-03, -1.79711e-02, -6.11229e-03, -3.02850e-03, -1.75780e-03,
           3.12418e-03,  3.24976e-03,  5.01505e-03,  4.19117e-03,  9.90420e-03
    };
    fIndices = {mf::eDiskB1, mf::eDiskB2, mf::eDiskB3, mf::eDiskPhase1,
                mf::eDiskPhase2, mf::eDiskPhase3, mf::eDiskPitch,
                mf::eToroidalBN, mf::eToroidalBS, mf::eToroidalR,
                mf::eToroidalR, mf::eToroidalZ, mf::ePoloidalB, mf::ePoloidalP,
                mf::ePoloidalR, mf::ePoloidalW, mf::ePoloidalZ, mf::eDiskH,
                mf::eDiskW, mf::eStriation};
    break;
  }
  default: {
    throw std::runtime_error("unknown field model");
    break;
  }
  }
  fV = utl::VfromL(fL);
}
