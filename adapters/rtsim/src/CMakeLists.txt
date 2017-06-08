message("RTSIM: Looking for headers in:")
message(" - " ${METASIM_SOURCE_DIR}/src)
message(" - " ${RTLIB_SOURCE_DIR}/src)

include_directories(${METASIM_SOURCE_DIR}/src)
include_directories(${RTLIB_SOURCE_DIR}/src)

# Create a library which includes the source files.
list(GET tres_rtsim_LIBRARIES 0 TRES_RTSIM_LIB_SOURCE)
add_library(${TRES_RTSIM_LIB_SOURCE} STATIC KernelRtSim.cpp
                                                            SimTaskRtSim.cpp
                                                            EventRtSim.cpp
                                                            ActiveSimulationManagerRtSim.cpp
                                                            regsched.cpp
                                                            regtask.cpp)
target_compile_features(${TRES_RTSIM_LIB_SOURCE} PRIVATE cxx_range_for)
# Link the deps for the library
target_link_libraries( ${TRES_RTSIM_LIB_SOURCE} ${tres_base_LIBRARIES}
                                                ${metasim_LIBRARIES} ${rtlib_LIBRARIES} )
