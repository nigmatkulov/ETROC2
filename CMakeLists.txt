cmake_minimum_required(VERSION 3.10)
project(ETROC2
        VERSION 1.0
        LANGUAGES C CXX)

set(libname ETROC2)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

message(STATUS "Looking for CERN ROOT")

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

# Find ROOT package
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree)

# if (ROOT_FOUND)
#         message(STATUS "ROOT ${ROOT_VERSION} found at ${ROOT_BINDIR}") 
#         message(STATUS "ROOT include dir found at ${ROOT_INCLUDE_DIRS}")
#         message(STATUS "ROOT libraries found at ${ROOT_LIBRARIES}")
# else()
#         message(FATAL_ERROR "ROOT not found. CMake will exit!")
# endif()

#Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

# Add the ROOT include directories
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR} 
        ${ROOT_INCLUDE_DIRS}
)

# Add same CXX flags that were used to combile and build ROOT (including CXX standard)
add_definitions(${ROOT_CXX_FLAGS})

# Add the ROOT library directories
link_directories(${ROOT_LIBRARY_DIR})

# List of headers
set(HEADERS
    Hit.h
)

# List of sources
set(SOURCES
    Hit.cc
    #G__${libname}.cxx
    # etrocTrans2ROOT.cxx
)

# Generate ROOT dictionary
root_generate_dictionary(G__${libname} 
                         ${HEADERS} 
                         LINKDEF LinkDef.h)

# Generate rootmap
root_generate_rootmap(${libname} LINKDEF LinkDef.h)

# Create a shared library with geneated dictionary
add_library(${libname} SHARED ${SOURCES} G__${libname}.cxx)

# Link libraries need to run the code
target_link_libraries(${libname} ${ROOT_LIBRARIES})


# Executable
add_executable(etrocTrans2ROOT etrocTrans2ROOT.cxx)
# Link the ROOT libraries
target_link_libraries(etrocTrans2ROOT ${libname} ${ROOT_LIBRARIES})

# Executable
add_executable(readEtroc2Boards readEtroc2Boards.cxx)
# Link the ROOT libraries
target_link_libraries(readEtroc2Boards ${libname} ${ROOT_LIBRARIES})

# Custom target for cleaning
add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/lib${libname}*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/G__${libname}*
    COMMENT "Clean generated files"
)