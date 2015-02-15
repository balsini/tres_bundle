cmake_minimum_required (VERSION 2.6)
project (tres_omnetpp)

set(TRES_OMNETPP_STANDALONE ON CACHE STRING "Build tres_omnetpp as stand-alone (ie, NOT included in the tres_bundle or similar high-level packaging projects")

if(TRES_OMNETPP_STANDALONE)
    # find project's modules
    # (otherwise deps are resolved by the parent (top-level) project)
    find_package(omnetpp REQUIRED
                    HINTS "${CMAKE_CURRENT_LIST_DIR}/cmake") # omnetpp_INCLUDE_DIR,
                                                             # omnetpp_LIBRARIES,
                                                             # omnetpp_LINK_DIRECTORIES
    find_package(tres_base   REQUIRED
                    HINTS "${CMAKE_CURRENT_LIST_DIR}/cmake") # tres_base_INCLUDE_DIR,
                                                             # tres_base_LIBRARIES,
                                                             # tres_base_LINK_DIRECTORIES
endif()

# Add dep headers to the search path
include_directories(${omnetpp_INCLUDE_DIRS})
include_directories(${tres_base_INCLUDE_DIRS})

# Interface headers are in "include/tres_omnetpp" (global header files,
# to be shared across libraries and bindings), and in "src"
# (local to this projects)
include_directories(include)
include_directories(src)

# Add dep libs to the search path
link_directories(${LINK_DIRECTORIES} ${omnetpp_LINK_DIRECTORIES})
link_directories(${LINK_DIRECTORIES} ${tres_base_LINK_DIRECTORIES})

# The code is inside the directory "src"
add_subdirectory (src)

# Export. FIXME
#export(PACKAGE tres_omnetpp)
