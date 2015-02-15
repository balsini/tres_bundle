#!/bin/bash
#
# Get the tres_bundle directory
TRES_BUNDLE_DIR=$(cd `dirname "${BASH_SOURCE[0]}"` && pwd)

# Set-up the environment variables for OMNeT++
source $TRES_BUNDLE_DIR/3rdparty/omnetpp/setenv

# Set-up the environment variables for tres_simulink
source $TRES_BUNDLE_DIR/simulink/tres_simulink-setup.bash
