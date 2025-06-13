# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\finalproject_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\finalproject_autogen.dir\\ParseCache.txt"
  "finalproject_autogen"
  )
endif()
