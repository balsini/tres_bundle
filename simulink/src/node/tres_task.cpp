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
 * \file tres_task.cpp
 */

// TODO: license, doxygen
#define S_FUNCTION_NAME tres_task
#define S_FUNCTION_LEVEL 2

#include <iostream>
#include <string>
#include <vector>
#include <tres/Task.hpp>
#include "simstruc.h"

#define INSTRSET_DESCR_VARNAME 0

static const mxArray * getTaskInstructionSet(SimStruct *S)
{
    char *bufIsd;       // INSTRSET_DESCR_VARNAME
    int bufIsdLen;
    const mxArray *mxIsd;

    // Get the instruction-set description (Isd)
    // (block's callbacks guarantee it's a MATLAB cell array)
    bufIsdLen = mxGetN( ssGetSFcnParam(S,INSTRSET_DESCR_VARNAME) )+1;
    bufIsd = new char[bufIsdLen];
    mxGetString(ssGetSFcnParam(S,INSTRSET_DESCR_VARNAME), bufIsd, bufIsdLen);
    mxIsd = mexGetVariablePtr("base", bufIsd);
    delete bufIsd;

    return mxIsd;
}

/* Function: mdlInitializeSizes ===========================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    int num_segments;

    ssSetNumSFcnParams(S, 1);  /* Number of expected parameters */

    // Get the number of segments
    num_segments = mxGetNumberOfElements(getTaskInstructionSet(S));

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    if (!ssSetNumInputPorts(S, 0)) return;

    if (!ssSetNumOutputPorts(S, 2)) return;
    ssSetOutputPortWidth(S, 0, 2*num_segments);
    ssSetOutputPortWidth(S, 1, 1);

    ssSetNumSampleTimes(S, 1);

    ssSetNumPWork(S, 1); // tres::Task (set in mdlInitializeConditions())
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
}

/* Function: mdlInitializeSampleTimes =====================================
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    int i, num_segments;

    // Sample times
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, FIXED_IN_MINOR_STEP_OFFSET);

    // Get the number of segments
    num_segments = mxGetNumberOfElements(getTaskInstructionSet(S));

    // Initialize the Function Call output
    for (i = 0; i < 2*num_segments; i++)
        ssSetCallSystemOutput(S,i);
}

#define MDL_INITIALIZE_CONDITIONS
/* Function: mdlInitializeConditions ======================================
 * Abstract:
 *    Initialize discrete state to zero.
 */
static void mdlInitializeConditions(SimStruct *S)
{
    const mxArray *mxIsd;
    int num_segments;
    std::vector<std::string> instr_list;

    // Get the number of segments
    mxIsd = getTaskInstructionSet(S);
    num_segments = mxGetNumberOfElements(mxIsd);

    // Construct the sequence of pseudo instructions
    // Note that the consistency of the task-code description (cell array)
    // has already been checked in get_sfun_param()
    for (int index = 0; index < num_segments; index++)
    {
        instr_list.push_back( std::string(mxArrayToString(mxGetCell(mxIsd, index))) );
    }

    // Create a new tres::Task instance from the task-code description vector
    tres::Task *task = new tres::Task(instr_list);

    // Save the new C++ object for later usage (mdlOutputs())
    ssGetPWork(S)[0] = task;

    // Initial (real) Output
    real_T  *y = ssGetOutputPortRealSignal(S,1);
#ifdef TRES_DEBUG_1
        mexPrintf("%s: task->ni_duration is %.3f\n", __FILE__, task->getSegmentDuration());
#endif
    y[0] = task->getSegmentDuration();
}

/* Function: mdlOutputs ===================================================
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    // Get the C++ object back from the pointers vector
    tres::Task *task = static_cast<tres::Task *>(ssGetPWork(S)[0]);

    // Process the current segment
    int subsys_idx = task->processSegment();

    // Write (duration of next_instr)
    real_T  *y = ssGetOutputPortRealSignal(S,1);
    y[0] = task->getSegmentDuration();

    // Function-call generation (subsys_act_idx)
    if (subsys_idx == 0)
        ssCallSystemWithTid(S, 0, tid);
    else
    {
        ssCallSystemWithTid(S, 2*subsys_idx - 1, tid);
        if (subsys_idx < task->getNumberOfSegments())
            ssCallSystemWithTid(S, 2*subsys_idx, tid);
    }
#ifdef TRES_DEBUG_1
        mexPrintf("%s: activated segment #%d task->ni_duration is %.3f\n", __FILE__,
			subsys_idx, task->getSegmentDuration());

#endif
}

/* Function: mdlTerminate =================================================
 *    No termination needed, but we are required to have this routine.
 */
static void mdlTerminate(SimStruct *S)
{
    // Retrieve and destroy C++ object
    tres::Task *task = static_cast<tres::Task *>(ssGetPWork(S)[0]);
    delete task;
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif