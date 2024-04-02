#ifndef _UF23FIELD_H_
#define _UF23FIELD_H_
/**
 @class UF23Field
 @brief UF23Field Galactic magnetic field model

 Implements the eight coherent magnetic field models of UF23
 See: M. Unger and G.R. Farrar, arXiv:2311.12120

 Assumes a galactocentric coordinate system with the Galactic center
 at the origin, the x-axis pointing in the opposite direction of the
 Sun, and the z-axis pointing towards Galactic North.

 */

#include <vector>
#include "Vector3.h"

class UF23Field {
public:
  /// model variations (see Tab.2 of UF23 paper)
  enum ModelType {
    base,
    neCL,
    expX,
    spur,
    cre10,
    synCG,
    twistX,
    nebCor
  };

public:
  /**
     @brief constructir
     @param mt model type  (see Tab.2 of UF23 paper)
     @param maxRadiusInKpc maximum radius of field in kpc
  */
  UF23Field(const ModelType mt, const double maxRadiusInKpc = 30);
  /// no default constructor
  UF23Field() = delete;
  /**
     @brief calculate coherent magnetic field at a given position
     @param posInKpc position with components given in kpc
     @return coherent field in microgauss
  */
  Vector3 operator()(const Vector3& posInKpc) const;

private:

  /// model type given in constructor
  const ModelType fModelType;
  /// maximum galacto-centric radius beyond which B=0
  const double fMaxRadiusSquared;

  /// model parameters, see Table 3 of UF23 paper
  double fDiskB1 = 0;
  double fDiskB2 = 0;
  double fDiskB3 = 0;
  double fDiskH = 0;
  double fDiskPhase1 = 0;
  double fDiskPhase2 = 0;
  double fDiskPhase3 = 0;
  double fDiskPitch = 0;
  double fDiskW = 0;
  double fPoloidalA = 0;
  double fPoloidalB = 0;
  double fPoloidalP = 0;
  double fPoloidalR = 0;
  double fPoloidalW = 0;
  double fPoloidalZ = 0;
  double fSpurCenter = 0;
  double fSpurLength = 0;
  double fSpurWidth = 0;
  double fStriation = 0;
  double fToroidalBN = 0;
  double fToroidalBS = 0;
  double fToroidalR = 0;
  double fToroidalW = 0;
  double fToroidalZ = 0;
  double fTwistingTime = 0;

  // some pre-calculated derived parameter values
  double fSinPitch = 0;
  double fCosPitch = 0;
  double fTanPitch = 0;

  /// major field components
  Vector3 GetDiskField(const Vector3& pos) const;
  Vector3 GetHaloField(const Vector3& pos) const;

  /// sub-components depending on model type
  /// -- Sec. 5.2.2
  Vector3 GetSpiralField(const double x, const double y, const double z) const;
  /// -- Sec. 5.2.3
  Vector3 GetSpurField(const double x, const double y, const double z) const;
  /// -- Sec. 5.3.1
  Vector3 GetToroidalHaloField(const double x, const double y, const double z) const;
  /// -- Sec. 5.3.2
  Vector3 GetPoloidalHaloField(const double x, const double y, const double z) const;
  /// -- Sec. 5.3.3
  Vector3 GetTwistedHaloField(const double x, const double y, const double z) const;

};
#endif
