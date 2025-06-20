#include "UF23Field.h"

#include <exception>
#include <limits>
#include <string>
#include <cmath>

// local helper functions and constants
namespace utl {

  template<typename T>
  Vector3 CylToCart(const T v, const double cosPhi, const double sinPhi)
  {
    return Vector3(v[0] * cosPhi - v[1] * sinPhi,
                   v[0] * sinPhi + v[1] * cosPhi,
                   v[2]);
  }

  template<typename T>
  Vector3 CartToCyl(const T v, const double cosPhi, const double sinPhi)
  {
    return Vector3(v[0] * cosPhi + v[1] * sinPhi,
                   -v[0] * sinPhi + v[1] * cosPhi,
                   v[2]);
  }

  // logistic sigmoid function
  inline
  double
  Sigmoid(const double x, const double x0, const double w)
  {
    return 1 / (1 + exp(-(x-x0)/w));
  }

  // angle between v0 = (cos(phi0), sin(phi0)) and v1 = (cos(phi1), sin(phi1))
  inline
  double
  DeltaPhi(const double phi0, const double phi1)
  {
    return acos(cos(phi1)*cos(phi0) + sin(phi1)*sin(phi0));
  }

  constexpr double kPi = 3.1415926535897932384626;
  constexpr double kLn10    = 2.3025850929940456840180;
  constexpr double kTwoPi = 2*kPi;
  constexpr double degree = kPi/180.;
  constexpr double kpc = 1;
  constexpr double microgauss = 1;
  constexpr double megayear = 1;
  constexpr double Gpc = 1e6*kpc;
  constexpr double pc = 1e-3*kpc;
  constexpr double second  = megayear / (1e6*60*60*24*365.25);
  constexpr double kilometer = kpc / 3.0856775807e+16;
}

// initialization of static members
const std::map<UF23Field::ModelType, std::string> UF23Field::fModelNames =
  { {base, "base"},
    {neCL, "neCL"},
    {expX, "expX"},
    {spur, "spur"},
    {cre10,"cre10"},
    {synCG, "synCG"},
    {twistX, "twistX"},
    {nebCor, "nebCor"}
  };

const std::map<std::string, UF23Field::ModelType> UF23Field::fModelTypes =
  { {"base", base},
    {"neCL", neCL},
    {"expX", expX},
    {"spur", spur},
    {"cre10", cre10},
    {"synCG", synCG},
    {"twistX", twistX},
    {"nebCor", nebCor}
  };

UF23Field::UF23Field(const ModelType mt, const double maxRadiusInKpc) :
  fModelType(mt),
  fMaxRadiusSquared(pow(maxRadiusInKpc*utl::kpc, 2))
{
  using namespace utl;

  // all but expX model have a-->\infty, Eq.(38)
  fPoloidalA    =  1 * Gpc;

  switch (fModelType) {
    // ---------------------------------------------
  case base: {
    fDiskB1        =  1.0878565e+00 * microgauss;
    fDiskB2        =  2.6605034e+00 * microgauss;
    fDiskB3        =  3.1166311e+00 * microgauss;
    fDiskH         =  7.9408965e-01 * kpc;
    fDiskPhase1    =  2.6316589e+02 * degree;
    fDiskPhase2    =  9.7782269e+01 * degree;
    fDiskPhase3    =  3.5112281e+01 * degree;
    fDiskPitch     =  1.0106900e+01 * degree;
    fDiskW         =  1.0720909e-01 * kpc;
    fPoloidalB     =  9.7775487e-01 * microgauss;
    fPoloidalP     =  1.4266186e+00 * kpc;
    fPoloidalR     =  7.2925417e+00 * kpc;
    fPoloidalW     =  1.1188158e-01 * kpc;
    fPoloidalZ     =  4.4597373e+00 * kpc;
    fStriation     =  3.4557571e-01;
    fToroidalBN    =  3.2556760e+00 * microgauss;
    fToroidalBS    = -3.0914569e+00 * microgauss;
    fToroidalR     =  1.0193815e+01 * kpc;
    fToroidalW     =  1.6936993e+00 * kpc;
    fToroidalZ     =  4.0242749e+00 * kpc;
    break;
  }
  case cre10: {
    // ---------------------------------------------
    fDiskB1        =  1.2035697e+00 * microgauss;
    fDiskB2        =  2.7478490e+00 * microgauss;
    fDiskB3        =  3.2104342e+00 * microgauss;
    fDiskH         =  8.0844932e-01 * kpc;
    fDiskPhase1    =  2.6515882e+02 * degree;
    fDiskPhase2    =  9.8211313e+01 * degree;
    fDiskPhase3    =  3.5944588e+01 * degree;
    fDiskPitch     =  1.0162759e+01 * degree;
    fDiskW         =  1.0824003e-01 * kpc;
    fPoloidalB     =  9.6938453e-01 * microgauss;
    fPoloidalP     =  1.4150957e+00 * kpc;
    fPoloidalR     =  7.2987296e+00 * kpc;
    fPoloidalW     =  1.0923051e-01 * kpc;
    fPoloidalZ     =  4.5748332e+00 * kpc;
    fStriation     =  2.4950386e-01;
    fToroidalBN    =  3.7308133e+00 * microgauss;
    fToroidalBS    = -3.5039958e+00 * microgauss;
    fToroidalR     =  1.0407507e+01 * kpc;
    fToroidalW     =  1.7398375e+00 * kpc;
    fToroidalZ     =  2.9272800e+00 * kpc;
    break;
  }
  case nebCor: {
    // ---------------------------------------------
    fDiskB1        =  1.4081935e+00 * microgauss;
    fDiskB2        =  3.5292400e+00 * microgauss;
    fDiskB3        =  4.1290147e+00 * microgauss;
    fDiskH         =  8.1151971e-01 * kpc;
    fDiskPhase1    =  2.6447529e+02 * degree;
    fDiskPhase2    =  9.7572660e+01 * degree;
    fDiskPhase3    =  3.6403798e+01 * degree;
    fDiskPitch     =  1.0151183e+01 * degree;
    fDiskW         =  1.1863734e-01 * kpc;
    fPoloidalB     =  1.3485916e+00 * microgauss;
    fPoloidalP     =  1.3414395e+00 * kpc;
    fPoloidalR     =  7.2473841e+00 * kpc;
    fPoloidalW     =  1.4318227e-01 * kpc;
    fPoloidalZ     =  4.8242603e+00 * kpc;
    fStriation     =  3.8610837e-10;
    fToroidalBN    =  4.6491142e+00 * microgauss;
    fToroidalBS    = -4.5006610e+00 * microgauss;
    fToroidalR     =  1.0205288e+01 * kpc;
    fToroidalW     =  1.7004868e+00 * kpc;
    fToroidalZ     =  3.5557767e+00 * kpc;
    break;
  }
  case neCL: {
    // ---------------------------------------------
    fDiskB1        =  1.4259645e+00 * microgauss;
    fDiskB2        =  1.3543223e+00 * microgauss;
    fDiskB3        =  3.4390669e+00 * microgauss;
    fDiskH         =  6.7405199e-01 * kpc;
    fDiskPhase1    =  1.9961898e+02 * degree;
    fDiskPhase2    =  1.3541461e+02 * degree;
    fDiskPhase3    =  6.4909767e+01 * degree;
    fDiskPitch     =  1.1867859e+01 * degree;
    fDiskW         =  6.1162799e-02 * kpc;
    fPoloidalB     =  9.8387831e-01 * microgauss;
    fPoloidalP     =  1.6773615e+00 * kpc;
    fPoloidalR     =  7.4084361e+00 * kpc;
    fPoloidalW     =  1.4168192e-01 * kpc;
    fPoloidalZ     =  3.6521188e+00 * kpc;
    fStriation     =  3.3600213e-01;
    fToroidalBN    =  2.6256593e+00 * microgauss;
    fToroidalBS    = -2.5699466e+00 * microgauss;
    fToroidalR     =  1.0134257e+01 * kpc;
    fToroidalW     =  1.1547728e+00 * kpc;
    fToroidalZ     =  4.5585463e+00 * kpc;
    break;
  }
  case spur: {
    // ---------------------------------------------
    fDiskB1        = -4.2993328e+00 * microgauss;
    fDiskH         =  7.5019749e-01 * kpc;
    fDiskPhase1    =  1.5589875e+02 * degree;
    fDiskPitch     =  1.2074432e+01 * degree;
    fDiskW         =  1.2263120e-01 * kpc;
    fPoloidalB     =  9.9302987e-01 * microgauss;
    fPoloidalP     =  1.3982374e+00 * kpc;
    fPoloidalR     =  7.1973387e+00 * kpc;
    fPoloidalW     =  1.2262244e-01 * kpc;
    fPoloidalZ     =  4.4853270e+00 * kpc;
    fSpurCenter    =  1.5718686e+02 * degree;
    fSpurLength    =  3.1839577e+01 * degree;
    fSpurWidth     =  1.0318114e+01 * degree;
    fStriation     =  3.3022369e-01;
    fToroidalBN    =  2.9286724e+00 * microgauss;
    fToroidalBS    = -2.5979895e+00 * microgauss;
    fToroidalR     =  9.7536425e+00 * kpc;
    fToroidalW     =  1.4210055e+00 * kpc;
    fToroidalZ     =  6.0941229e+00 * kpc;
    break;
  }
  case synCG: {
    // ---------------------------------------------
    fDiskB1        =  8.1386878e-01 * microgauss;
    fDiskB2        =  2.0586930e+00 * microgauss;
    fDiskB3        =  2.9437335e+00 * microgauss;
    fDiskH         =  6.2172353e-01 * kpc;
    fDiskPhase1    =  2.2988551e+02 * degree;
    fDiskPhase2    =  9.7388282e+01 * degree;
    fDiskPhase3    =  3.2927367e+01 * degree;
    fDiskPitch     =  9.9034844e+00 * degree;
    fDiskW         =  6.6517521e-02 * kpc;
    fPoloidalB     =  8.0883734e-01 * microgauss;
    fPoloidalP     =  1.5820957e+00 * kpc;
    fPoloidalR     =  7.4625235e+00 * kpc;
    fPoloidalW     =  1.5003765e-01 * kpc;
    fPoloidalZ     =  3.5338550e+00 * kpc;
    fStriation     =  6.3434763e-01;
    fToroidalBN    =  2.3991193e+00 * microgauss;
    fToroidalBS    = -2.0919944e+00 * microgauss;
    fToroidalR     =  9.4227834e+00 * kpc;
    fToroidalW     =  9.1608418e-01 * kpc;
    fToroidalZ     =  5.5844594e+00 * kpc;
    break;
  }
  case twistX: {
    // ---------------------------------------------
    fDiskB1        =  1.3741995e+00 * microgauss;
    fDiskB2        =  2.0089881e+00 * microgauss;
    fDiskB3        =  1.5212463e+00 * microgauss;
    fDiskH         =  9.3806180e-01 * kpc;
    fDiskPhase1    =  2.3560316e+02 * degree;
    fDiskPhase2    =  1.0189856e+02 * degree;
    fDiskPhase3    =  5.6187572e+01 * degree;
    fDiskPitch     =  1.2100979e+01 * degree;
    fDiskW         =  1.4933338e-01 * kpc;
    fPoloidalB     =  6.2793114e-01 * microgauss;
    fPoloidalP     =  2.3292519e+00 * kpc;
    fPoloidalR     =  7.9212358e+00 * kpc;
    fPoloidalW     =  2.9056201e-01 * kpc;
    fPoloidalZ     =  2.6274437e+00 * kpc;
    fStriation     =  7.7616317e-01;
    fTwistingTime  =  5.4733549e+01 * megayear;
    break;
  }
  case expX: {
    // ---------------------------------------------
    fDiskB1        =  9.9258148e-01 * microgauss;
    fDiskB2        =  2.1821124e+00 * microgauss;
    fDiskB3        =  3.1197345e+00 * microgauss;
    fDiskH         =  7.1508681e-01 * kpc;
    fDiskPhase1    =  2.4745741e+02 * degree;
    fDiskPhase2    =  9.8578879e+01 * degree;
    fDiskPhase3    =  3.4884485e+01 * degree;
    fDiskPitch     =  1.0027070e+01 * degree;
    fDiskW         =  9.8524736e-02 * kpc;
    fPoloidalA     =  6.1938701e+00 * kpc;
    fPoloidalB     =  5.8357990e+00 * microgauss;
    fPoloidalP     =  1.9510779e+00 * kpc;
    fPoloidalR     =  2.4994376e+00 * kpc;
    // internally, xi is fitted and z = tan(xi)*a
    fPoloidalXi    =  2.0926122e+01 * degree;
    fPoloidalZ     =  fPoloidalA*tan(fPoloidalXi);
    fStriation     =  5.1440500e-01;
    fToroidalBN    =  2.7077434e+00 * microgauss;
    fToroidalBS    = -2.5677104e+00 * microgauss;
    fToroidalR     =  1.0134022e+01 * kpc;
    fToroidalW     =  2.0956159e+00 * kpc;
    fToroidalZ     =  5.4564991e+00 * kpc;
    break;
  }
  default: {
    throw std::runtime_error("unknown field model");
    break;
  }
  }

  fSinPitch = sin(fDiskPitch);
  fCosPitch = cos(fDiskPitch);
  fTanPitch = tan(fDiskPitch);

}

UF23Field::UF23Field(const std::string name, const double maxRadiusInKpc) :
  UF23Field(fModelTypes.at(name), maxRadiusInKpc) {}

Vector3
UF23Field::operator()(const Vector3& posInKpc)
  const
{
  const auto pos = posInKpc * utl::kpc;
  if (pos.SquaredLength() > fMaxRadiusSquared)
    return Vector3(0, 0, 0);
  else {
    const auto diskField = GetDiskField(pos);
    const auto haloField = GetHaloField(pos);
    return (diskField + haloField) / utl::microgauss;
  }
}

Vector3
UF23Field::GetDiskField(const Vector3& pos)
  const
{
  if (fModelType == spur)
    return GetSpurField(pos.x, pos.y, pos.z);
  else
    return GetSpiralField(pos.x, pos.y, pos.z);
}


Vector3
UF23Field::GetHaloField(const Vector3& pos)
  const
{
  if (fModelType == twistX)
    return GetTwistedHaloField(pos.x, pos.y, pos.z);
  else
    return
      GetToroidalHaloField(pos.x, pos.y, pos.z) +
      GetPoloidalHaloField(pos.x, pos.y, pos.z);
}


Vector3
UF23Field::GetTwistedHaloField(const double x, const double y, const double z)
  const
{
  const double r = sqrt(x*x + y*y);
  const double cosPhi = r > std::numeric_limits<double>::min() ? x / r : 1;
  const double sinPhi = r > std::numeric_limits<double>::min() ? y / r : 0;

  const Vector3 bXCart = GetPoloidalHaloField(x, y, z);
  const double bXCartTmp[3] = {bXCart.x, bXCart.y, bXCart.z};
  const Vector3 bXCyl = utl::CartToCyl(bXCartTmp, cosPhi, sinPhi);

  const double bZ = bXCyl.z;
  const double bR = bXCyl.x;

  double bPhi = 0;

  if (fTwistingTime != 0 && r != 0) {
    // radial rotation curve parameters (fit to Reid et al 2014)
    const double v0 = -240 * utl::kilometer/utl::second;
    const double r0 = 1.6 * utl::kpc;
    // vertical gradient (Levine+08)
    const double z0 = 10 * utl::kpc;

    // Eq.(43)
    const double fr = 1 - exp(-r/r0);
    // Eq.(44)
    constexpr double maxArg =
      std::numeric_limits<double>::max_exponent10 * utl::kLn10;
    const double arg = 2*std::abs(z)/z0;
    if (arg > maxArg)
      bPhi= 0;
    else {
      const double t0 = exp(arg);
      const double gz =  2 / (1 + t0);

      // Eq. (46)
      const double signZ = z < 0 ? -1 : 1;
      const double deltaZ =  -signZ * v0 * fr / z0  * t0 * pow(gz, 2);
      // Eq. (47)
      const double deltaR = v0 * ((1-fr)/r0 - fr/r) * gz;

      // Eq.(45)
      bPhi = (bZ * deltaZ + bR * deltaR) * fTwistingTime;
    }
  }
  const double bCylX[3] = {bR, bPhi , bZ};
  return utl::CylToCart(bCylX, cosPhi, sinPhi);
}

Vector3
UF23Field::GetToroidalHaloField(const double x, const double y, const double z)
  const
{
  const double r2 = x*x + y*y;
  const double r = sqrt(r2);
  const double absZ = std::abs(z);

  const double b0 = z >= 0 ? fToroidalBN : fToroidalBS;
  const double rh = fToroidalR;
  const double z0 = fToroidalZ;
  const double fwh = fToroidalW;
  const double sigmoidR = utl::Sigmoid(r, rh, fwh);
  const double sigmoidZ = utl::Sigmoid(absZ, fDiskH, fDiskW);

  // Eq. (21)
  const double bPhi = b0 * (1. - sigmoidR) * sigmoidZ * exp(-absZ/z0);

  const double bCyl[3] = {0, bPhi, 0};
  const double cosPhi = r > std::numeric_limits<double>::min() ? x / r : 1;
  const double sinPhi = r > std::numeric_limits<double>::min() ? y / r : 0;
  return utl::CylToCart(bCyl, cosPhi, sinPhi);
}

Vector3
UF23Field::GetPoloidalHaloField(const double x, const double y, const double z)
  const
{
  const double r2 = x*x + y*y;
  const double r = sqrt(r2);

  const double c = pow(fPoloidalA/fPoloidalZ, fPoloidalP);
  const double a0p = pow(fPoloidalA, fPoloidalP);
  const double rp = pow(r, fPoloidalP);
  const double abszp = pow(std::abs(z), fPoloidalP);
  const double cabszp = c*abszp;

  /*
    since $\sqrt{a^2 + b} - a$ is numerical unstable for $b\ll a$,
    we use $(\sqrt{a^2 + b} - a) \frac{\sqrt{a^2 + b} + a}{\sqrt{a^2
    + b} + a} = \frac{b}{\sqrt{a^2 + b} + a}$}
  */

  const double t0 = a0p + cabszp - rp;
  const double t1 = sqrt(pow(t0, 2) + 4*a0p*rp);
  const double ap = 2*a0p*rp / (t1  + t0);

  double a = 0;
  if (ap < 0) {
    if (r > std::numeric_limits<double>::min()) {
      // this should never happen
      throw std::runtime_error("ap = " + std::to_string(ap));
    }
    else
      a = 0;
  }
  else
    a = pow(ap, 1/fPoloidalP);

  // Eq.(29) and Eq.(32)
  const double radialDependence =
    fModelType == expX ?
    exp(-a/fPoloidalR) :
    1 - utl::Sigmoid(a, fPoloidalR, fPoloidalW);

  // Eq.(28)
  const double Bzz = fPoloidalB * radialDependence;

  // (r/a)
  const double rOverA =  1 / pow(2*a0p / (t1  + t0), 1/fPoloidalP);

  // Eq.(35) for p=n
  const double signZ = z < 0 ? -1 : 1;
  const double Br =
    Bzz * c * a / rOverA * signZ * pow(std::abs(z), fPoloidalP - 1) / t1;

  // Eq.(36) for p=n
  const double Bz = Bzz * pow(rOverA, fPoloidalP-2) * (ap + a0p) / t1;

  if (r < std::numeric_limits<double>::min())
    return Vector3(0, 0, Bz);
  else {
    const double bCylX[3] = {Br, 0 , Bz};
    const double cosPhi =  x / r;
    const double sinPhi =  y / r;
    return utl::CylToCart(bCylX, cosPhi, sinPhi);
  }
}

Vector3
UF23Field::GetSpurField(const double x, const double y, const double z)
  const
{
  // reference approximately at solar radius
  const double rRef = 8.2*utl::kpc;

  // cylindrical coordinates
  const double r2 = x*x + y*y;
  const double r = sqrt(r2);
  if (r < std::numeric_limits<double>::min())
    return Vector3(0, 0, 0);

  double phi = atan2(y, x);
  if (phi < 0)
    phi += utl::kTwoPi;

  const double phiRef = fDiskPhase1;
  int iBest = -2;
  double bestDist = -1;
  for (int i = -1; i <= 1; ++i) {
    const double pphi = phi - phiRef + i*utl::kTwoPi;
    const double rr = rRef*exp(pphi * fTanPitch);
    if (bestDist < 0 || std::abs(r-rr) < bestDist) {
      bestDist =  std::abs(r-rr);
      iBest = i;
    }
  }
  if (iBest == 0) {
    const double phi0 = phi - log(r/rRef) / fTanPitch;

    // Eq. (16)
    const double deltaPhi0 = utl::DeltaPhi(phiRef, phi0);
    const double delta = deltaPhi0 / fSpurWidth;
    const double B = fDiskB1 * exp(-0.5*pow(delta, 2));

    // Eq. (18)
    const double wS = 5*utl::degree;
    const double phiC = fSpurCenter;
    const double deltaPhiC = utl::DeltaPhi(phiC, phi);
    const double lC = fSpurLength;
    const double gS = 1 - utl::Sigmoid(std::abs(deltaPhiC), lC, wS);

    // Eq. (13)
    const double hd = 1 - utl::Sigmoid(std::abs(z), fDiskH, fDiskW);

    // Eq. (17)
    const double bS = rRef/r * B * hd * gS;
    const double bCyl[3] = {bS * fSinPitch, bS * fCosPitch, 0};
    const double cosPhi = x / r;
    const double sinPhi = y / r;
    return utl::CylToCart(bCyl, cosPhi, sinPhi);
  }
  else
    return Vector3(0, 0, 0);

}

Vector3
UF23Field::GetSpiralField(const double x, const double y, const double z)
  const
{
  // reference radius
  const double rRef = 5*utl::kpc;
  // inner boundary of spiral field
  const double rInner = 5*utl::kpc;
  const double wInner = 0.5*utl::kpc;
  // outer boundary of spiral field
  const double rOuter = 20*utl::kpc;
  const double wOuter = 0.5*utl::kpc;

  // cylindrical coordinates
  const double r2 = x*x + y*y;
  if (r2 == 0)
    return Vector3(0, 0, 0);
  const double r = sqrt(r2);
  const double phi = atan2(y, x);

  // Eq.(13)
  const double hdz = 1 - utl::Sigmoid(std::abs(z), fDiskH, fDiskW);

  // Eq.(14) times rRef divided by r
  const double rFacI = utl::Sigmoid(r, rInner, wInner);
  const double rFacO = 1 - utl::Sigmoid(r, rOuter, wOuter);
  // (using lim r--> 0 (1-exp(-r^2))/r --> r - r^3/2 + ...)
  const double rFac =  r > 1e-5*utl::pc ? (1-exp(-r*r)) / r : r * (1 - r2/2);
  const double gdrTimesRrefByR = rRef * rFac * rFacO * rFacI;

  // Eq. (12)
  const double phi0 = phi - log(r/rRef) / fTanPitch;

  // Eq. (10)
  const double b =
    fDiskB1 * cos(1 * (phi0 - fDiskPhase1)) +
    fDiskB2 * cos(2 * (phi0 - fDiskPhase2)) +
    fDiskB3 * cos(3 * (phi0 - fDiskPhase3));

  // Eq. (11)
  const double fac = hdz * gdrTimesRrefByR;
  const double bCyl[3] =
    { b * fac * fSinPitch,
      b * fac * fCosPitch,
      0};

  const double cosPhi = x / r;
  const double sinPhi = y / r;
  return utl::CylToCart(bCyl, cosPhi, sinPhi);
}


namespace utl {
  const std::vector<double> unitConv =
    {
     microgauss, //eDiskB1,
     microgauss, //eDiskB2,
     microgauss, //eDiskB3,
     kpc, //eDiskH,
     degree, //eDiskPhase1,
     degree, //eDiskPhase2,
     degree, //eDiskPhase3,
     degree, //eDiskPitch,
     kpc, //eDiskW,
     kpc, //ePoloidalA,
     microgauss, //ePoloidalB,
     1, //ePoloidalP,
     kpc, //ePoloidalR,
     kpc, //ePoloidalW,
     kpc, //ePoloidalZ,
     degree, //ePoloidalXi,
     degree, //eSpurCenter,
     degree, //eSpurLength,
     degree, //eSpurWidth,
     1, //eStriation,
     microgauss, //eToroidalBN,
     microgauss, //eToroidalBS,
     kpc, //eToroidalR,
     kpc, //eToroidalW,
     kpc, //eToroidalZ,
     megayear //eTwistingTime
    };
}

std::vector<double>
UF23Field::GetParameters()
  const
{
  using namespace utl;
  if (unitConv.size() != eNpar)
    throw std::runtime_error("invalid unit vector");

  std::vector<double> retVec;
  for (unsigned int i = 0; i < eNpar; ++i)
    retVec.push_back(fParameters[i] / unitConv[i]);
  return retVec;
}

void
UF23Field::SetParameters(const std::vector<double>& newpar)
{
  using namespace utl;
  if (newpar.size() != eNpar)
    throw std::runtime_error("invalid parameter vector");

  if (unitConv.size() != eNpar)
    throw std::runtime_error("invalid unit vector");

  for (unsigned int i = 0; i < eNpar; ++i)
    fParameters[i] = newpar[i] * unitConv[i];

  fSinPitch = sin(fDiskPitch);
  fCosPitch = cos(fDiskPitch);
  fTanPitch = tan(fDiskPitch);
  if (fModelType == expX)
    fPoloidalZ     =  fPoloidalA*tan(fPoloidalXi);

}

double
UF23Field::GetMaximumSquaredRadius()
  const
{
  return fMaxRadiusSquared / utl::kpc;
}
