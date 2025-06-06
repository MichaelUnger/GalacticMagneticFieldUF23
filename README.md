UF23 Galactic Magnetic Field Models
===================================

[![DOI](https://zenodo.org/badge/753818568.svg)](https://zenodo.org/doi/10.5281/zenodo.10627090)  [![arXiv](https://img.shields.io/badge/arXiv-2311.12120-b31b1b.svg?style=plastic)](https://arxiv.org/abs/2311.12120)

C++ implementation of Galactic Magnetic Field models from the paper [The Coherent Magnetic Field of the Milky Way](https://doi.org/10.3847/1538-4357/ad4a54) by [Michael Unger](mailto:michael.unger@kit.edu) and [Glennys R. Farrar](mailto:gf25@nyu.edu).



## Usage

To calculate the magnetic field of the models, only the code implemented in the UF23Field class is needed. E.g., the magnetic field of the `base` model at a certain position can be obtained as
```C++
const UF23Field uf23Field(UF23Field::base);
const Vector3 position(1, 3, 2);
const auto field = uf23Field(position);
cout << field.x << " " << field.y << " " << field.z << endl;
```

Here the constructor takes an argument specifying one of the eight UF23 models (`base`, `cre10`, `expX`, `neCL`, `nebCor`, `spur`, `synCG` or `twistX`). The position is given in Cartesian galacto-centric coordinates in units of kpc (Earth at negative *x*, North at positive *z*). The output magnetic field vector is again in Cartesian galacto-centric coordinates, the unit of its components is &mu;G (micro-Gauss).

## Example programs

Type
```
make
```
to compile two simple example programs. Then type e.g.
```
./calcUF23Field base 1 3 2
```
to obtain the following output:
```
(x,y,z)    = (1.0000e+00, 3.0000e+00, 2.0000e+00) kpc
(bx,by,bz) = (-1.7697e+00, 8.5706e-01, 6.6356e-01) microgauss
```

Note: this is just a test program. If many evaluations are needed it is very inefficient to execute the `calcUF23Field` program repeatedly due to the overhead of initialization. For that case it is better to implement a loop over positions inside the code.

Another example program called `sampleUF23Field` illustrates the sampling of parameter uncertainties for advanced users.

For further technical tests, run
```
make test
```

## License

Released under BSD 2-Clause "Simplified" License.

Please cite  [Unger and Farrar (2024)](https://doi.org/10.3847/1538-4357/ad4a54) if you find this code useful in your research. The BibTeX entry for the paper is:
```
@article{UngerFarrar2023,
    author = "Unger, Michael and Farrar, Glennys R.",
    title = "{The Coherent Magnetic Field of the Milky Way}",
    eprint = "2311.12120",
    archivePrefix = "arXiv",
    primaryClass = "astro-ph.GA",
    doi = "10.3847/1538-4357/ad4a54",
    journal = "Astrophys. J.",
    volume = "970",
    number = "1",
    pages = "95",
    year = "2024"
}
```

## Python bindings
If `pybind` is installed, then Python bindings can be compiled via
```
make pybind
```
Then test via
```
python3 Python/uf23_field.py
```
