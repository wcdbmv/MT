#include "geogebra/api.h"

#include <format>
#include <string>
#include <string_view>

#include "base/float.h"
#include "math/linalg/vector3f.h"

namespace geogebra {

std::string CylinderInfiniteZ(const std::string_view name, const Float radius) {
  constexpr std::string_view fmt =
      R"F(<expression label="{}" exp="x^(2) + y^(2) = {}^(2)" type="quadric"/>
<element type="quadric" label="eq1">
	<show object="true" label="false" ev="4"/>
	<objColor r="211" g="47" b="47" alpha="0.6499999761581421"/>
	<layer val="0"/>
	<labelMode val="0"/>
	<fixed val="true"/>
	<lineStyle thickness="5" type="0" typeHidden="1"/>
	<eqnStyle style="user"/>
</element>
)F";
  return std::format(fmt, name, radius);
}

std::string Point3D(const std::string_view name, const Vector3F point) {
  constexpr std::string_view fmt =
      R"F(<expression label="{0}" exp="({1}, {2}, {3})" type="point"/>
<element type="point3d" label="{0}">
	<show object="true" label="true" ev="4"/>
	<objColor r="77" g="77" b="255" alpha="0"/>
	<layer val="0"/>
	<labelMode val="0"/>
	<animation step="0.1" speed="1" type="1" playing="false"/>
	<pointSize val="5"/>
</element>
)F";
  return std::format(fmt, name, point.x(), point.y(), point.z());
}

std::string Ray3D(const std::string_view name,
                  const std::string_view pos,
                  const std::string_view dir) {
  constexpr std::string_view fmt =
      R"F(<expression label="{1}{2}" exp="{1} + {2}" type="point" />
<element type="point3d" label="{1}{2}">
	<show object="true" label="true" ev="4"/>
	<objColor r="68" g="68" b="68" alpha="0"/>
	<layer val="0"/>
	<labelMode val="0"/>
	<pointSize val="4"/>
</element>
<command name="Ray">
	<input a0="{1}" a1="{1}{2}"/>
	<output a0="{0}"/>
</command>
<element type="ray3d" label="{0}">
	<show object="true" label="false" ev="4"/>
	<objColor r="0" g="0" b="0" alpha="0"/>
	<layer val="0"/>
	<labelMode val="0"/>
	<lineStyle thickness="5" type="0" typeHidden="1"/>
	<eqnStyle style="parametric" parameter="λ"/>
	<outlyingIntersections val="false"/>
	<keepTypeOnTransform val="true"/>
</element>
)F";
  return std::format(fmt, name, pos, dir);
}

}  // namespace geogebra
