#! /bin/bash
#
export TRES_SIMULINK_DIR=$(cd `dirname "${BASH_SOURCE[0]}"` && pwd)
TRES_BASE_LIB=$TRES_SIMULINK_DIR/../build/base/src
TRES_RTSIM_LIB=$TRES_SIMULINK_DIR/../build/adapters/rtsim/src
export TRES_OMNETPP_LIB=$TRES_SIMULINK_DIR/../build/adapters/omnetpp/src
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$TRES_BASE_LIB:$TRES_RTSIM_LIB:$TRES_OMNETPP_LIB
