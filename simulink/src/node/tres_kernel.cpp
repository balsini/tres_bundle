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
 * \file tres_kernel.cpp
 */

#define S_FUNCTION_NAME tres_kernel
#define S_FUNCTION_LEVEL 2

#include <iostream>
#include <sstream>
#include <memory>
#include <cstdlib>           // atof
#include <tres/Factory.hpp>
#include <tres/SimTask.hpp>
#include <tres/Kernel.hpp>
#include <tres/RTOSEvent.hpp>

#include "regkern.cpp"      // registration of tres::Kernel adapters

#include "simstruc.h"
#include "matrix.h"

#define TS_DESCR_VARNAME    0
#define SCHEDULING_POLICY   1
#define SP_DESCR_VARNAME    2
#define DEADLINE_MISS_RULE  3
#define TIME_RESOLUTION     4
#define NUMBER_OF_CORES     5
#define SIMULATION_ENGINE   6

#include "tres_kernel_utils.cpp"

/**
 * \addtogroup tres_simulink
 * @{
 */
namespace _tres_kernel
{
    /**
     * \brief Aperiodic-activation requests' Manager
     * \note Only for internal use
     */
    struct _AperiodicReqsManager
    {
        // data
        bool receivedAperiodicReq;
        std::vector<boolean_T> prev_reqs;
        std::vector<int> aper_activ_idx;
        // methdos
        _AperiodicReqsManager(int, InputBooleanPtrsType);
        ~_AperiodicReqsManager();
        void evaluateIncomingReqs(InputBooleanPtrsType);
    };
    _AperiodicReqsManager::_AperiodicReqsManager(int num_aper_reqs, InputBooleanPtrsType aper_reqs)
    {
		receivedAperiodicReq = false;
        prev_reqs.assign(*aper_reqs, *aper_reqs+num_aper_reqs);
        aper_activ_idx.reserve(num_aper_reqs);
    }
    _AperiodicReqsManager::~_AperiodicReqsManager()
    {
    }
    void _AperiodicReqsManager::evaluateIncomingReqs(InputBooleanPtrsType aper_reqs)
    {
        int num_aper_reqs = prev_reqs.size();
        aper_activ_idx.clear();
        for (int i = 0; i < num_aper_reqs; ++i)
        {
            if (*aper_reqs[i] > prev_reqs[i])
                aper_activ_idx.push_back(i);
        }
        receivedAperiodicReq = (aper_activ_idx.size() > 0);
        prev_reqs.assign(*aper_reqs, *aper_reqs+num_aper_reqs);
    }
}

/**
 * \brief Initialize sizes
 *
 * The sizes information is used by Simulink to determine the S-function
 * block's characteristics (number of inputs, outputs, states, etc.).
 */
#define MDL_INIT_SIZE
static void mdlInitializeSizes(SimStruct *S)
{
    // Number of expected parameters
    ssSetNumSFcnParams(S, 7);

#ifndef TRES_DISABLE_MASK_PROTECTION
    // Perform mask params validity check
    // TODO.
    //  - more MATLAB vars (custom sched pol, other?)
    //  - the hidden valid_mask parameter
    const mxArray *mxTsdVarName = ssGetSFcnParam(S,TS_DESCR_VARNAME);
    if ((mxGetM(mxTsdVarName) != 1) || (mxGetN(mxTsdVarName) == 0))
    {
        ssSetErrorStatus(S, "Task-set description variable is not correct");
        return;
    }
#endif

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    // Set the input port to have a dynamic dimension
    if (!ssSetNumInputPorts(S, 2)) return;

    ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
    if(!ssSetInputPortDataType(S, 0, DYNAMICALLY_TYPED)) return;

    ssSetInputPortWidth(S, 1, DYNAMICALLY_SIZED);
	if(!ssSetInputPortDataType(S, 1, SS_BOOLEAN)) return;

    // Set the input ports as Direct Feed Through
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortDirectFeedThrough(S, 1, 1);

    // Set the output port to have a dynamic dimension
    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, DYNAMICALLY_SIZED);

    ssSetNumSampleTimes(S, 1);

    ssSetNumPWork(S, 2);  // store the tres::Kernel and the _tres_kernel::_AperiodicReqsManager
    ssSetNumRWork(S, 1);  // store the time_resolution
    ssSetNumNonsampledZCs(S, 1);    // next hit

    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
}

#if defined(MATLAB_MEX_FILE)
#define MDL_SET_INPUT_PORT_WIDTH
void mdlSetInputPortWidth(SimStruct *S, int_T port, int_T width)
{
	ssSetInputPortWidth(S, port, width);
	if (port == 0) ssSetOutputPortWidth(S, 0, width);
}

#define MDL_SET_OUTPUT_PORT_WIDTH
void mdlSetOutputPortWidth(SimStruct *S, int_T port, int_T width)
{
}
#endif

/**
 * \brief Initialize sample times
 */
#define MDL_INIT_ST
static void mdlInitializeSampleTimes(SimStruct *S)
{
    // Sample time
    ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, FIXED_IN_MINOR_STEP_OFFSET);

    // Get the number of tasks (it is equal to the size of output port)
    int_T num_tasks = ssGetOutputPortWidth(S,0);

    // Initialize the Function Call output
    for (int i = 0; i < num_tasks; i++)
        ssSetCallSystemOutput(S, i);
}

/**
 * \brief Model start
 *
 * This function is called once at start of model execution
 * If you have states that should be initialized once, this is the place
 * to do it
 */
#define MDL_START
static void mdlStart(SimStruct *S)
{
    char *bufSimEng;  // SIMULATION_ENGINE
    int bufSimEngLen;

    // Build the list of parameters that configure the tres::Kernel
    std::vector<std::string> kern_params = readMaskAndBuildConfVector(S);

    // Get the type of the adapter, i.e., the concrete implementation of tres::Kernel
    bufSimEngLen = mxGetN( ssGetSFcnParam(S,SIMULATION_ENGINE) )+1;
    bufSimEng = new char[bufSimEngLen];
    mxGetString(ssGetSFcnParam(S,SIMULATION_ENGINE), bufSimEng, bufSimEngLen);
    std::string engine(bufSimEng);
    delete bufSimEng;

    // Instantiate the concrete representation of tres::Kernel
    std::unique_ptr<tres::Kernel> kern = Factory<tres::Kernel>::instance()
                                            .create(engine, kern_params);

    // Save the C++ object to the pointers vector
    tres::Kernel *_kern = kern.release();
    ssGetPWork(S)[0] = _kern;

    // Save the (C++) manager of aperiodic requests
	ssGetPWork(S)[1] = new _tres_kernel::_AperiodicReqsManager(ssGetInputPortWidth(S,1),
                                                                (InputBooleanPtrsType) ssGetInputPortSignalPtrs(S,1));

    // Get the time resolution (actually, its floating point representation)
    std::vector<std::string>::iterator it = kern_params.end()-1;
    double time_resolution = atof((*it).c_str());

    // Save the time resolution to the real vector workspace
    ssGetRWork(S)[0] = time_resolution;
}

/**
 * \brief Initialize discrete state to zero
 */
#define MDL_INITIALIZE_CONDITIONS
static void mdlInitializeConditions(SimStruct *S)
{
    // Get the C++ object back from the pointers vector
    tres::Kernel *kern = static_cast<tres::Kernel *>(ssGetPWork(S)[0]);

    // Get the time resolution back from the real vector workspace
    double time_resolution = ssGetRWork(S)[0];

    // Get a pointer to the input port
    InputRealPtrsType u = ssGetInputPortRealSignalPtrs(S,0);

    // Initializes the kernel for the simulation
    kern->initializeSimulation(time_resolution, u);
}

/**
 * \brief Model output
 *
 * Perform a sequence of co-simulation steps together with the RT scheduling simulator
 */
#define MDL_OUTPUT
static void mdlOutputs(SimStruct *S, int_T tid)
{
    // Get ports access
    InputRealPtrsType  u = ssGetInputPortRealSignalPtrs(S,0);
    InputBooleanPtrsType aper_reqs = (InputBooleanPtrsType) ssGetInputPortSignalPtrs(S,1);

    // Get the C++ object back from the pointers vector
    tres::Kernel *kern = static_cast<tres::Kernel *>(ssGetPWork(S)[0]);
    _tres_kernel::_AperiodicReqsManager *aper_reqs_mgr = static_cast<_tres_kernel::_AperiodicReqsManager *>(ssGetPWork(S)[1]);

    // Get the time resolution back from the real vector workspace
    double time_resolution = ssGetRWork(S)[0];

#ifdef TRES_DEBUG_1
mexPrintf("\n%s: at _time_: %.3f\n", __FUNCTION__, ssGetT(S));
#endif

    // Manage aperiodic activation requests (if any)
    // Note: Aperiodic requests generate new events in the kernel
    // event queue, therefore they must processed _before_
    // computing the next_hit_tick
    aper_reqs_mgr->evaluateIncomingReqs(aper_reqs);
    if (aper_reqs_mgr->receivedAperiodicReq)
    {
#ifdef TRES_DEBUG_1
mexPrintf("\n*** Aperiodic activ(s) at time: %.3f***\n", ssGetT(S));
#endif
        kern->activateAperiodicTasks(aper_reqs_mgr->aper_activ_idx, ssGetT(S)*(time_resolution));
    }

    // Save the time of next block hit
    long int next_hit_tick = kern->getNextWakeUpTime();

    // If the current time is greater or equal than the next block hit
    if (ssGetT(S) - next_hit_tick/(time_resolution) >= 0.0)
    {
        do
        {
            tres::RTOSEvent *e = kern->getNextEvent();
            tres::SimTask *t = e->getGeneratorTask();

            // Classify the first incoming kernel event
            // within the time-span
            switch (e->getType())
            {
                case tres::RTOSEventType::END_INSTRUCTION:
                {
                    // Add this task to the to-be-triggered task queue
                    // (due to an end instruction)
                    kern->addTaskToTriggerQueue(t);

                    // Read the duration of the next time-consuming activity
                    // for the SimTask
                    real_T duration = *u[kern->getPort(t)];
                    if (duration > 0.0)
                    {
                        // The task is _not_ completed, so add another instruction
                        t->addInstruction(duration*(time_resolution));
                    }
                    break;
                }

                case tres::RTOSEventType::END_TASK:
                {
                    // On Task completion, clear the Start flag of the task
                    kern->clearStartTaskMark(t);

                    // Clear the instruction queue of the task
                    t->discardInstructions();

                    // Initialize the task with the duration of first instruction
                    // (Note that the following relationship holds for tasks
                    // that have completed their execution: duration < 0.0)
                    // Read the duration of the next time-consuming activity
                    // for the SimTask
                    real_T duration = *u[kern->getPort(t)];
                    t->addInstruction(-duration*(time_resolution));
                    break;
                }

                default:
                    break;
            }

            // Process the next event in the RT engine queue
            kern->processNextEvent();

        }
        while ( kern->getTimeOfNextEvent() == next_hit_tick );

        // Update the list of running tasks
        kern->getRunningTasks();

        // Add new scheduled tasks to the list of tasks to be scheduled
        kern->addNewTasksToTriggerQueue();

        // Mark running tasks
        kern->markNewScheduledTasks();

        // Read which tasks have to be triggered
        std::vector<int> ports = kern->getPortsToTrigger();

        // For each Task to trigger, send a Function generation
        // signal onto the corresponding port
        for (std::vector<int>::iterator port = ports.begin();
                port != ports.end();
                    ++port)
        {
            ssCallSystemWithTid(S, *port, tid);
        }
    }
}

#if 0
/**
 * \brief Model Update
 */
#define MDL_UPDATE
static void mdlUpdate(SimStruct *S, int_T tid)
{
}
#endif

/**
 * \brief Detect zero-crossing points
 *
 * Use zero-crossing points to set time instants at which the execution of the
 * tres_kernel block will be triggered on
 */
#define MDL_ZERO_CROSSINGS
static void mdlZeroCrossings(SimStruct *S)
{
    // Get the C++ object back from the pointers vector
    tres::Kernel *kern = static_cast<tres::Kernel *>(ssGetPWork(S)[0]);

    // Get the time resolution back from the real vector workspace
    double time_resolution = ssGetRWork(S)[0];

#ifdef TRES_DEBUG_1
mexPrintf("\n%s at _time_: %.3f\n", __FUNCTION__, ssGetT(S));
#endif

    ssGetNonsampledZCs(S)[0] = kern->getTimeOfNextEvent()/(time_resolution) - ssGetT(S);
}

/**
 * \brief Model Terminate
 * Perform all the actions that are necessary at the termination of a simulation
 */
static void mdlTerminate(SimStruct *S)
{
    mexPrintf("%s - %s @ %.3f\n", __FILE__, __FUNCTION__, ssGetT(S));

    // Get the C++ object back from the pointers vector
    tres::Kernel *kern = static_cast<tres::Kernel *>(ssGetPWork(S)[0]);
    _tres_kernel::_AperiodicReqsManager *aper_reqs_mgr = static_cast<_tres_kernel::_AperiodicReqsManager *>(ssGetPWork(S)[1]);

    // Call its destructor
    delete kern;
    delete aper_reqs_mgr;
}
/** @} */

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
