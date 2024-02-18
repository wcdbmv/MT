#pragma once

#ifdef DEBUG
#define NOEXCEPT_RELEASE
#else
#define NOEXCEPT_RELEASE noexcept
#endif
