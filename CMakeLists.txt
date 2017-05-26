cmake_minimum_required (VERSION 3.1)
project (tres_bundle)

include(ExternalProject)

set (METASIM_SOURCE_DIR "${CMAKE_BINARY_DIR}/3rd_party/RTSIM/METASIM")
set (RTLIB_SOURCE_DIR "${CMAKE_BINARY_DIR}/3rd_party/RTSIM//RTLIB")

message("Using external project folders:")
message(" - " ${METASIM_SOURCE_DIR})
message(" - " ${RTLIB_SOURCE_DIR})

ExternalProject_Add (
    METASIM
    PREFIX METASIM
    URL ${CMAKE_SOURCE_DIR}/3rd_party/RTSIM/METASIM.zip
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    SOURCE_DIR ${METASIM_SOURCE_DIR}
    TEST_COMMAND ""
)

ExternalProject_Add (
    RTLIB
    DEPENDS METASIM
    PREFIX RTLIB
    URL ${CMAKE_SOURCE_DIR}/3rd_party/RTSIM/RTLIB.zip
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    SOURCE_DIR ${RTLIB_SOURCE_DIR}
    TEST_COMMAND ""
)

# find modules
#find_package(rtsim REQUIRED
#                    HINTS "${CMAKE_CURRENT_LIST_DIR}/cmake") # metasim_INCLUDE_DIR,
                                                             # metasim_LIBRARIES,
                                                             # metasim_LINK_DIRECTORIES
                                                             # rtlib_INCLUDE_DIR,
                                                             # rtlib_LIBRARIES,
                                                             # rtlib_LINK_DIRECTORIES
#find_package(omnetpp REQUIRED
#                    HINTS "${CMAKE_CURRENT_LIST_DIR}/cmake") # omnetpp_INCLUDE_DIR,
#                                                             # omnetpp_LIBRARIES,
#                                                             # omnetpp_LINK_DIRECTORIES
find_package(tres_base REQUIRED
                    HINTS "${CMAKE_CURRENT_LIST_DIR}/cmake") # tres_base_INCLUDE_DIR,
                                                             # tres_base_LIBRARIES,
                                                             # tres_base_LINK_DIRECTORIES
find_package(tres_rtsim REQUIRED
                    HINTS "${CMAKE_CURRENT_LIST_DIR}/cmake") # tres_rtsim_INCLUDE_DIR,
                                                             # tres_rtsim_LIBRARIES,
                                                             # tres_rtsim_LINK_DIRECTORIES
#find_package(tres_omnetpp REQUIRED
#                    HINTS "${CMAKE_CURRENT_LIST_DIR}/cmake") # tres_omnetpp_INCLUDE_DIR,
#                                                             # tres_omnetpp_LIBRARIES,
#                                                             # tres_omnetpp_LINK_DIRECTORIES

# some config vars
SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
MESSAGE("tres_bundle build info: CMAKE_BUILD_TYPE is set to ${CMAKE_BUILD_TYPE}")
set(RTSIM_STANDALONE OFF CACHE STRING "Build RTSim as stand-alone (ie, NOT included in a 3rd-party project")
set(TRES_RTSIM_STANDALONE OFF CACHE STRING "Build tres_rtsim as stand-alone (ie, NOT included in the tres_bundle or similar high-level packaging projects")
#set(TRES_OMNETPP_STANDALONE OFF CACHE STRING "Build tres_omnetpp as stand-alone (ie, NOT included in the tres_bundle or similar high-level packaging projects")

# add modules' source code
#add_subdirectory (3rdparty/rtsim)
add_subdirectory (base)
add_subdirectory (adapters/rtsim)
#add_subdirectory (adapters/omnetpp)
