if(NOT CMAKE_CXX_INCLUDE_WHAT_YOU_USE)
  message(WARNING "'IWYU' is enabled, but the program has NOT been found")
  return()
endif()

string(CONCAT IWYU
  "${CMAKE_CXX_INCLUDE_WHAT_YOU_USE};"
  "-Xiwyu;--no_default_mappings;"
  "-Xiwyu;--mapping_file=${CMAKE_CURRENT_SOURCE_DIR}/scripts/iwyu/mappings.imp;"
  "-Xiwyu;--max_line_length=180;"
  "-Xiwyu;--comment_style=long;"
# "-Xiwyu;--quoted_includes_first;"
  "-Xiwyu;--cxx17ns"
)

set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${IWYU}")
message(STATUS "'IWYU' is enabled")
