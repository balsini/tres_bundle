include_directories(${METASIM_SOURCE_DIR}/src)
include_directories(${RTLIB_SOURCE_DIR}/src)

# Environment-based settings.
if(WIN32)
    set(TRES_BASE_LIB_TYPE "STATIC")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++0x")
	set(TRES_BASE_LIB_TYPE "SHARED")
endif()

# Create a library which includes the source files.
list(GET tres_base_LIBRARIES 0 TRES_BASE_LIB_SOURCE)
add_library(${TRES_BASE_LIB_SOURCE} ${TRES_BASE_LIB_TYPE}   ParseUtils.cpp
                                                            RandomVar.cpp
                                                            GenericVar.cpp
                                                            Kernel.cpp
                                                            Network.cpp
                                                            Task.cpp
                                                            FixedExecSegment.cpp
                                                            RandExecSegment.cpp
                                                            reginstr.cpp
                                                            regvar.cpp)
