function(enable_ipo_lto_for_release _target)
  message(STATUS "Configure IPO/LTO for ${_target}")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "IPO/LTO disabled in debug")
    return()
  endif()

  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipoSupported OUTPUT ipoOutput)
  if(NOT ipoSupported)
    # TODO(a.kerimov): Figure out why not working
    message(STATUS "IPO/LTO not supported: ${ipoOutput}")
    return()
  endif()

  message(STATUS "IPO/LTO enabled")
  set_property(TARGET ${_target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endfunction()
