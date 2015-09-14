# based on http://www.cmake.org/cmake-tutorial/
cmake_minimum_required (VERSION 2.8)
project (Dumbnes)
add_compile_options(-std=c++14)
# set up project directory structure
set (PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set (PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}/bin")

# project version number
set (Dumbnes_VERSION_MAJOR 0)
set (Dumbnes_VERSION_MINOR 0)
# configure a header file to pass CMake settings into source code
configure_file (
    "${PROJECT_SOURCE_DIR}/dumbnes_config.h.in"
    "${PROJECT_BINARY_DIR}/dumbnes_config.h"
    )
# add the binary tree to the search path for include files
# so that we'll find dumbnes_config.h
include_directories ("${PROJECT_BINARY_DIR}"
                     "${PROJECT_SOURCE_DIR}")

# list source files
set (SOURCE ${SOURCE}
    ${PROJECT_SOURCE_DIR}/op_info.cc
    ${PROJECT_SOURCE_DIR}/dumbnes.cc
    ${PROJECT_SOURCE_DIR}/flat_memory.cc
    ${PROJECT_SOURCE_DIR}/nes_6502.cc
    ${PROJECT_SOURCE_DIR}/nes_6502_opcodes.cc
    )

#add the executable
add_executable (Dumbnes ${SOURCE})

# add the install targets
install (TARGETS Dumbnes DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/dumbnes_config.h"
         DESTINATION include)

