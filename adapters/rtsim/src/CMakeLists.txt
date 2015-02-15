# Environment-based settings.
if(WIN32)
    set(TRES_RTSIM_LIB_TYPE "STATIC")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++0x")
	set(TRES_RTSIM_LIB_TYPE "SHARED")
endif()

# Create a library which includes the source files.
list(GET tres_rtsim_LIBRARIES 0 TRES_RTSIM_LIB_SOURCE)
add_library(${TRES_RTSIM_LIB_SOURCE} ${TRES_RTSIM_LIB_TYPE} KernelRtSim.cpp
                                                            SimTaskRtSim.cpp
                                                            EventRtSim.cpp
                                                            regsched.cpp
                                                            regtask.cpp)
# Link the deps for the library
target_link_libraries( ${TRES_RTSIM_LIB_SOURCE} ${tres_base_LIBRARIES}
                                                ${metasim_LIBRARIES} ${rtlib_LIBRARIES} )
