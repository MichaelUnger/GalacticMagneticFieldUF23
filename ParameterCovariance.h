#ifndef _ParameterCovariance_h_
#define _ParameterCovariance_h_
/**
 @class ParameterCovariance
 @brief Covariance matrix of the parameters of the UF23 GMF models

 See appendix C of M. Unger and G.R. Farrar, arXiv:2311.12120

 */

#include <vector>
#include "UF23Field.h"

class ParameterCovariance {
public:
  /**
     @brief constructor
     @param mt model type  (see Tab.2 of UF23 paper)
  */
  ParameterCovariance(const UF23Field::ModelType mt);

  /// no default constructor
  ParameterCovariance() = delete;

  /// covariance matrix V (units: microGauss, kpc, degree, Myr)
  const std::vector<std::vector<double>>& GetCovarianceMatrix() const
  { return fV; }
  /// lower-triagonal L matrix (Cholesky decomposition V = L * L^T)
  const std::vector<double>& GetLMatrix() const
  { return fL; }
  /// UF23 parameter indices corresponding to matrix indices
  const std::vector<UF23Field::EPar>& GetParameterIndices() const
  { return fIndices;}

  /// print correlation matrix
  void PrintCorrelationMatrix() const;

  /// get matrix dimension
  unsigned int GetDimension() const { return fIndices.size(); }

  /**
     parameter offsets delta = L * n distributed according to V given
     vector of standard normal random numbers n
  */
  std::vector<double> GetRandomOffset(const std::vector<double>& n) const;

private:
  /// model type
  const UF23Field::ModelType fModelType;
  // covariance matrix V
  std::vector<std::vector<double>> fV;
  // lower-triagonal L matrix (Cholesky decomposition V = L * L^T)
  std::vector<double> fL;
  // UF23 parameter indices corresponding to matrix indices
  std::vector<UF23Field::EPar> fIndices;
};
#endif
