#pragma once

#include <functional>

#include "base/config/float.h"

using TemperatureFunc = std::function<Float(Float z)>;
using IntensityFunc = std::function<Float(Float t)>;
using AttenuationFunc = std::function<Float(Float t)>;
