UF23 Galactic Magnetic Field Models
===================================

C++ implementation of Galactic Magnetic Field models from the paper [New Models of the Coherent Magnetic Field of the Milky Way](https://arxiv.org/abs/2311.12120) by [Michael Unger](mailto:michael.unger@kit.edu) and [Glennys R. Farrar](mailto:gf25@nyu.edu).


## License

Released under BSD 2-Clause "Simplified" License.

Scientific or technical publications resulting from projects using this code are kindly requested to cite [arXiv:2311.12120](https://arxiv.org/abs/2311.12120).


## Usage

All the relevant code is in the UF23Field class. The magnetic field of the base model at a certain position can be obtained as
```C++
const UF23Field uf23Field(UF23Field::base);
const Vector3 position(1, 3, 2);
const auto field = uf23Field(position);
cout << field.x << " " << field.y << " " << field.z << endl;
```

Here the constructor takes an argument specifiying one of the eight UF23 models (base, cre10, expX, neCL, nebCor, spur, synCG or twistX). The position is given in cartesian galacto-centric coordinates in units of kpc (Earth at negative x, North at positive z). The output magnetic field vector is again in cartesian galacto-centric coordinates, the unit of its components is &mu;G (micro-Gauss).

## Example program

Type
```
make
```
to compile a simple example program. Then type e.g.
```
./uf23Field base 1 3 2
```
to obtain the following output:
```
(x,y,z)    = (1.0000e+00, 3.0000e+00, 2.0000e+00) kpc
(bx,by,bz) = (-1.7697e+00, 8.5706e-01, 6.6356e-01) microgauss
```

Note: this is just a test program. If many evaluations are needed it is very inefficient to execute the uf23Field program repeatedly due to the overhead of initialization. For that case it is better to implement a loop over positions inside the code.

 Please send bugs and suggestions to [Michael Unger](mailto:michael.unger@kit.edu) and [Glennys R. Farrar](mailto:gf25@nyu.edu).
