# CMakeList.txt : CMake project for DicomVisualizer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.25.1)



project ("DicomVisualizer")



set(CMAKE_CXX_STANDARD 17)

# Force colored output
option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

add_subdirectory(external)
add_subdirectory(src)

if(MSVC)
  target_compile_options(DicomVisualizer PUBLIC "/Zc:__cplusplus")
endif()

# TODO: Add tests and install targets if needed.
