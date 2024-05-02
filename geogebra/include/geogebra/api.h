#pragma once

#include <string>
#include <string_view>

#include "base/config/float.h"
#include "math/linalg/vector.h"

namespace geogebra {

std::string CylinderInfiniteZ(std::string_view name, Float radius);
std::string Point3D(std::string_view name, Vec3 point);
std::string Ray3D(std::string_view name,
                  std::string_view pos,
                  std::string_view dir);

std::string Circle(std::string_view name, Float radius);
std::string Point(std::string_view name, Vec3 point);
std::string Ray(std::string_view name,
                std::string_view pos,
                std::string_view dir);

}  // namespace geogebra
