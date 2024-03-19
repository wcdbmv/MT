#pragma once

#include <string>
#include <string_view>

#include "base/float.h"
#include "math/linalg/vector3f.h"

namespace geogebra {

std::string CylinderInfiniteZ(std::string_view name, Float radius);
std::string Point3D(std::string_view name, Vector3F point);
std::string Ray3D(std::string_view name,
                  std::string_view pos,
                  std::string_view dir);

}  // namespace geogebra
