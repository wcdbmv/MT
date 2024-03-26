#pragma once

#include <functional>

using TemperatureFunc = std::function<Float(Float z)>;
using IntensityFunc = std::function<Float(Float T)>;
using AttenuationFunc = std::function<Float(Float T)>;
