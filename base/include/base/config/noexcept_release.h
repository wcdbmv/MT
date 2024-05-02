#pragma once

#include "base/config/build_type.h"

#ifdef MT_RELEASE
#define MT_NOEXCEPT_RELEASE noexcept
#else
#define MT_NOEXCEPT_RELEASE
#endif
