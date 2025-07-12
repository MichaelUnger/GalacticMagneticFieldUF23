#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "UF23Field.h"

namespace py = pybind11;

class PyUF23 {
public:
  PyUF23(const std::string& model, const double maxR = 30) :
    field(model, maxR) {}

  std::tuple<double, double, double>
  get_field(double x, double y, double z)
    const
  {
    auto B = field({x, y, z});
    return {B.x, B.y, B.z};
  }

  py::array_t<double> get_field_many(py::array_t<double, py::array::c_style | py::array::forcecast> positions) const {
    if (positions.ndim() != 2 || positions.shape(1) != 3)
      throw std::runtime_error("Input must be Nx3 array of positions.");

    ssize_t N = positions.shape(0);
    py::array_t<double> result(std::vector<ssize_t>{N, 3});
    auto pos = positions.unchecked<2>();
    auto out = result.mutable_unchecked<2>();

    for (ssize_t i = 0; i < N; ++i) {
      auto B = field({pos(i,0), pos(i,1), pos(i,2)});
      out(i,0) = B.x;
      out(i,1) = B.y;
      out(i,2) = B.z;
    }

    return result;
  }

private:
  UF23Field field;
};

PYBIND11_MODULE(uf23, m) {
    m.doc() = "UF23 Galactic Magnetic Field Python bindings";

    py::class_<PyUF23>(m, "UF23")
      .def(py::init<const std::string&, const double>(), py::arg("model"), py::arg("maxR") = 30.0)
        .def("get_field", &PyUF23::get_field, "Evaluate B field at a single point")
        .def("get_field_many", &PyUF23::get_field_many, "Evaluate B field at many points (Nx3 numpy array)");
}
