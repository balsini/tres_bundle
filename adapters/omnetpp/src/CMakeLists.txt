# Environment-based settings.
if(WIN32)
    set(TRES_OMNETPP_LIB_TYPE "STATIC")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++0x")
	set(TRES_OMNETPP_LIB_TYPE "SHARED")
endif()

# Create a library called which includes the source files.
list(GET tres_omnetpp_LIBRARIES 0 TRES_OMNETPP_LIB_SOURCE)
add_library(${TRES_OMNETPP_LIB_SOURCE} ${TRES_OMNETPP_LIB_TYPE} Tresenv.cpp
                                                                NetworkOpp.cpp
                                                                EventOpp.cpp
                                                                CanEventOpp.cpp
                                                                regevts.cpp
                                                                SimMessageOpp.cpp)
# Create the library for the gateway
list(GET tres_omnetpp_LIBRARIES 1 TRES_OMNETPP_LIB_GATEWAY)
add_library(${TRES_OMNETPP_LIB_GATEWAY} ${TRES_OMNETPP_LIB_TYPE} NetworkOppGateway.cpp)

# Link the deps for tres_omnetpp
target_link_libraries( ${TRES_OMNETPP_LIB_SOURCE}   ${tres_base_LIBRARIES}
                                                    ${omnetpp_LIBRARIES} )

# Link the deps for tres_omnetpp_gw
target_link_libraries( ${TRES_OMNETPP_LIB_GATEWAY} dl )
