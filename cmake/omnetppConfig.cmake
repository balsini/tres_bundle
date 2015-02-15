set(OMNETPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/omnetpp")
set(omnetpp_INCLUDE_DIRS        ${OMNETPP_DIR}/include
                                ${OMNETPP_DIR}/include/platdep
                                ${OMNETPP_DIR}/src/envir
                                ${OMNETPP_DIR}/src/common
                                ${OMNETPP_DIR}/inet/src/base
                                ${OMNETPP_DIR}/inet/src/applications
                                ${OMNETPP_DIR}/inet/src/linklayer/can)
set(omnetpp_LIBRARIES           oppcommond
                                oppenvird
                                oppeventlogd
                                opplayoutd
                                oppnedxmld
                                oppsimd)
set(omnetpp_LINK_DIRECTORIES    ${OMNETPP_DIR}/lib)
mark_as_advanced(OMNETPP_DIR)
