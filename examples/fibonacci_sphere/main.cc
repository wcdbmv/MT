// NOLINTBEGIN

#include <matplot/matplot.h>
#include <random>
#include <tuple>

#include "math/geometry/fibonacci_sphere.h"

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
generate_data();

int main() {
  using namespace matplot;


  auto [x, y, z] = generate_data();

  std::vector<double> sizes(x.size() / 3, 16);
  std::fill_n(std::back_inserter(sizes), x.size() / 3, 8);
  std::fill_n(std::back_inserter(sizes), x.size() / 3, 2);

  std::vector<double> colors(x.size() / 3, 1);
  std::fill_n(std::back_inserter(colors), x.size() / 3, 2);
  std::fill_n(std::back_inserter(colors), x.size() / 3, 3);

  scatter3(x, y, z, sizes, colors);
  view(40, 35);

  show();
  return 0;
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
generate_data() {
  auto points = FibonacciSphere(100);
  std::vector<double> x, y, z;

  for (size_t i = 0; i < 100; ++i) {
    x.push_back(points[i].x());
    y.push_back(points[i].y());
    z.push_back(points[i].z());
  }

  return {x, y, z};
}

// NOLINTEND
