/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2014,2015, ReTiS Lab., Scuola Superiore Sant'Anna.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the ReTiS Lab. nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/**
 * \file tres_message_df.cpp
 */

// TODO: license, doxygen
#define S_FUNCTION_NAME tres_message_df
#define S_FUNCTION_LEVEL 2

#include <iostream>
#include <string>
#include <vector>
#include "simstruc.h"

#define NUMBER_OF_MSG_OBJECTS     0

namespace _tres_message
{

    /**
     * \brief Possible internal states of tres_message block during its life-cycle
     * \note Internal use only.
     *       Having states encoded as enums will help when/if more sophisticated
     *       block life-cycles are needed (e.g., error-recovery on
     *       failed transmissions/receptions)
     */
    enum class _MsgBlockState
    {
        READY_TO_SEND,
        READY_TO_RECEIVE
    };

    /**
     * \brief (Extremely simple) Manager of send/receive behavior of tres_message block
     *
     * It's only capable of performing "circular" dispatching of states
     *
     * \note Internal use only
     */
    struct _SendReceiveLifeCycleManager
    {
        // data
        _MsgBlockState state;
        // methods
        _SendReceiveLifeCycleManager();
        ~_SendReceiveLifeCycleManager();
        void dispatch();
    };
    _SendReceiveLifeCycleManager::_SendReceiveLifeCycleManager()
    {
        state = _MsgBlockState::READY_TO_SEND;
    }
    _SendReceiveLifeCycleManager::~_SendReceiveLifeCycleManager()
    {
    }
    void _SendReceiveLifeCycleManager::dispatch()
    {
        if ( state == _MsgBlockState::READY_TO_SEND )
            state = _MsgBlockState::READY_TO_RECEIVE;
        else if ( state == _MsgBlockState::READY_TO_RECEIVE )
            state = _MsgBlockState::READY_TO_SEND;
    }

}

/* Function: mdlInitializeSizes ===========================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, 1);  /* Number of expected parameters */

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortWidth(S, 0, 1);

    // Get the number of CPU cores parameter
    int num_msg_objects = static_cast<int>(mxGetScalar(ssGetSFcnParam(S, NUMBER_OF_MSG_OBJECTS)));
    // Set the output port and its dimensions
    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, 2*num_msg_objects);

    ssSetNumSampleTimes(S, 1);

    ssSetNumPWork(S, 1);  // store the _SendReceiveLifeCycleManager instance
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
}

#if 0
#if defined(MATLAB_MEX_FILE)
#define MDL_SET_INPUT_PORT_WIDTH
void mdlSetInputPortWidth(SimStruct *S, int_T port, int_T width)
{
    mexPrintf("\n%s, %s, IN port is %d, width is %d\n", ssGetPath(S), __FUNCTION__, port, width);
}

#define MDL_SET_OUTPUT_PORT_WIDTH
void mdlSetOutputPortWidth(SimStruct *S, int_T port, int_T width)
{
    mexPrintf("\n%s, %s, OUT port is %d, width is %d\n", ssGetPath(S), __FUNCTION__, port, width);
    ssSetOutputPortWidth(S, port, width);
}
#endif
#endif

/* Function: mdlInitializeSampleTimes =====================================
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    // Sample times
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, FIXED_IN_MINOR_STEP_OFFSET);
    
    // Initialize the Function Call output
    int_T port_width = ssGetOutputPortWidth(S,0);
    for (int i = 0; i < port_width; i++)
        ssSetCallSystemOutput(S,i);
}

// Function: mdlStart =====================================================
// Abstract:
//   This function is called once at start of model execution. If you
//   have states that should be initialized once, this is the place
//   to do it.
#define MDL_START
static void mdlStart(SimStruct *S)
{
    // Create a new _SendReceiveLifeCycleManager instance
    ssGetPWork(S)[0] = new _tres_message::_SendReceiveLifeCycleManager();
}

/* Function: mdlOutputs ===================================================
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    // Check if the message has pending activation
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);
    if (*uPtrs[0] > 0.0)
    {
        // Get the C++ object back from the pointers vector
        _tres_message::_SendReceiveLifeCycleManager *lcm = static_cast<_tres_message::_SendReceiveLifeCycleManager *>(ssGetPWork(S)[0]);

        // Get the port width
        int_T port_width = ssGetOutputPortWidth(S,0);

        // A tres_message block drives SEND/RECEIVE latches in this order:
        //
        //      send1,receive1,send2,receive2,...,sendn,receiven
        //        ^      ^       ^      ^           ^       ^
        //        0      1       2      3          m-2     m-1
        //
        // where m is the port-width and is always an even number. Therefore, activations
        // of latches can be managed by issuing fcn-call signals onto even-indexed ports
        // when the state of the Life Cycle Manager is READY_TO_SEND, and onto odd-indexed
        // ports when the state is READY_TO_RECEIVE
        int i = (lcm->state == _tres_message::_MsgBlockState::READY_TO_SEND) ? 0 : 1;
        for (i; i < port_width; i+=2)
        {
            mexPrintf("\n%s\n\t%s\n\t\tat time %.6f, firing latch '%s'\n",
                        ssGetPath(S), __FUNCTION__, ssGetT(S), ((i%2 == 0) ? "SEND":"RECEIVE"));
            ssCallSystemWithTid(S, i, tid);
        }

        // Update the internal state of the Life Cycle Manager
        lcm->dispatch();
    }
}

/* Function: mdlTerminate =================================================
 *    No termination needed, but we are required to have this routine.
 */
static void mdlTerminate(SimStruct *S)
{
    // Get the C++ object back from the pointers vector
    _tres_message::_SendReceiveLifeCycleManager *lcm = static_cast<_tres_message::_SendReceiveLifeCycleManager *>(ssGetPWork(S)[0]);
    delete lcm;
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif

