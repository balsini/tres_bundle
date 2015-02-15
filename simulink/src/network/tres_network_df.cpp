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
 * \file tres_network_df.cpp
 */

#define S_FUNCTION_NAME tres_network_df
#define S_FUNCTION_LEVEL 2

#include <iostream>
#include <sstream>
#include <memory>
#include <cstdlib>

#include <tres/Factory.hpp>
#include <tres/Network.hpp>
#include <tres/NetworkEvent.hpp>
#include <tres/SimMessage.hpp>

#include "regnets.cpp"      // registration of tres::Network adapters

#include "simstruc.h"
#include "matrix.h"

#define MSG_DESCR_VARNAME   0
#define TIME_RESOLUTION     1
#define SIMULATION_ENGINE   2
#define NTWK_DESCR_VARNAME  3
#define OTHER_DEPS          4

/**
 * \brief Convert cell-array-based entity descriptions into vector-of-strings-based descriptions
 *
 * All parameters in each row are guaranteed to be std::strings. Error checking is performed at
 * MATLAB level by using mask callbacks.
 *
 * This function converts cell-array-based entity descriptions into entity descriptions that are
 * made of a vector of strings. Each string represents a row of the cell array. A string is made
 * of a list of parameters separated by the token ';'. Parameters coincide with the entries in
 * the corresponding row of the cell-array.
 */
static std::vector<std::string> cellArrayDescrToVectorOfStrings(const mxArray *mx_var)
{
    // The vector-of-strings-based description (output)
    std::vector<std::string> out_descr;

    // Get the number of entries (rows of an entity description)
    // and the number of parameters
    int num_entries = mxGetM(mx_var);
    int num_params = mxGetN(mx_var);

    // Convert the description of each message
    for (int i = 0; i < num_entries; ++i)
    {
        // The stringstream
        std::stringstream ss;

        // Put each entry item (string) into the stringstream
        for (int j = 0; j < num_params; ++j)
        {
            mxArray *entry_item = mxGetCell(mx_var, (i + num_entries * j));
            char *entry_item_val = new char[mxGetN(entry_item)+1];
            mxGetString(entry_item, entry_item_val, mxGetN(entry_item)+1);
            ss << entry_item_val;
            delete entry_item_val;

            // Put a separator (token)
            ss << ';';
        }

        // Insert the stringstream into the description vector
        out_descr.push_back(ss.str());
    }

    // Return the vector-of-string-based description
    return out_descr;
}

/**
 * \brief Build the list of parameters that configure the tres::Network
 *
 * The tres::Network class (or, more precisely, its underlying concrete
 * implementation) needs to be configured with a bunch of information, such
 * as the description of the message set, the network topology, initialization
 * informations and time resolution, to name only a few. This information is
 * stored inside the T-Res Network block as mask parameters.
 *
 * This function reads the mask parameters and returns a vector-of-string-based
 * description of the information needed by the tres::Network class.
 * Specifically, the information returned by this function has the following form:
 *  - the number of messages in the message-set (#msgs)    - std::string (1)   <-- vector.begin()
 *  - the message-set description                          - std::string (#msgs)
 *      - "message_type;message_UID;"
 *  - the number of items describing the network (#ndescr) - std::string (1)
 *  - the network description                              - std::string (#ndescr)
 *      - "description_type;description_file_paths"
 *  - path to additional libraries for the simulation      - std::string (1)
 *  - the time resolution                                  - std::string (1)
 *                                                                             <-- vector.end()
 *
 * No err checking is performed when reading mask parameters since these are already
 * guaranteed to be valid at MATLAB level (err checking performed by mask callbacks).
 */
static std::vector<std::string> readMaskAndBuildConfVector(SimStruct *S)
{
    char *bufMsgDescr,  // MSG_DESCR_VARNAME
         *bufTimeRes,   // TIME_RESOLUTION
         *bufNtwkDescr, // NTWK_DESCR_VARNAME
         *bufAddLibr;   // OTHER_DEPS

    int bufMsgDescrLen, bufTimeResLen, bufNtwkDescrLen, bufAddLibrLen;
    std::vector<std::string> net_params; // The return list of parameters
    std::stringstream ss;                // A convenience stringstream

    // Get the number of messages (it is equal to the size of output port)
    int_T num_msgs = ssGetOutputPortWidth(S,0);

    // Get the name of the workspace variable for the message set
    bufMsgDescrLen = mxGetN( ssGetSFcnParam(S,MSG_DESCR_VARNAME) )+1;
    bufMsgDescr = new char[bufMsgDescrLen];
    mxGetString(ssGetSFcnParam(S,MSG_DESCR_VARNAME), bufMsgDescr, bufMsgDescrLen);

    // Get the actual message set description
    std::vector<std::string> msg_descr = cellArrayDescrToVectorOfStrings(mexGetVariablePtr("base", bufMsgDescr));
    delete bufMsgDescr;

    // **Insert** the number of messages and
    //            the message set description in the return list
    ss << num_msgs;
    net_params.push_back(ss.str());
    ss.str(std::string());                    // Flush the ss
    net_params.insert(net_params.end(),
                           msg_descr.begin(),
                               msg_descr.end());

    // Get the name of the workspace variable for the Network description
    bufNtwkDescrLen = mxGetN( ssGetSFcnParam(S,NTWK_DESCR_VARNAME) )+1;
    bufNtwkDescr = new char[bufNtwkDescrLen];
    mxGetString(ssGetSFcnParam(S,NTWK_DESCR_VARNAME), bufNtwkDescr, bufNtwkDescrLen);

    // Get the number of Network description entries and
    // the actual Network description
    int_T num_ndescr = mxGetM(mexGetVariablePtr("base", bufNtwkDescr));
    std::vector<std::string> ntwk_descr = cellArrayDescrToVectorOfStrings(mexGetVariablePtr("base", bufNtwkDescr));
    delete bufNtwkDescr;

    // **Insert** the number of Network description entries and
    //            the Network description in the return list
    ss << num_ndescr;
    net_params.push_back(ss.str());
    ss.str(std::string());                    // Flush the ss
    net_params.insert(net_params.end(),
                           ntwk_descr.begin(),
                               ntwk_descr.end());

    // Get the path to additional libraries
    bufAddLibrLen = mxGetN( ssGetSFcnParam(S,OTHER_DEPS) )+1;
    bufAddLibr = new char[bufAddLibrLen];
    mxGetString(ssGetSFcnParam(S,OTHER_DEPS), bufAddLibr, bufAddLibrLen);

    // **Insert** the additional libraries in the return list
    std::string add_libs(bufAddLibr);
    net_params.push_back(add_libs);

    // And finally, get the time resolution
    bufTimeResLen = mxGetN( ssGetSFcnParam(S,TIME_RESOLUTION) )+1;
    bufTimeRes = new char[bufTimeResLen];
    mxGetString(ssGetSFcnParam(S,TIME_RESOLUTION), bufTimeRes, bufTimeResLen);

    // Convert the time resolution to a double
    std::string time_resolution(bufTimeRes);
    delete bufTimeRes;
    if (time_resolution == "Seconds")
        ss << 1.0;
    else if (time_resolution == "Milli_Seconds")
        ss << 1.0e3;
    else if (time_resolution == "Micro_Seconds")
        ss << 1.0e6;
    else if (time_resolution == "Nano_Seconds")
        ss << 1.0e9;

    // **Insert** the time resolution in the return list
    net_params.push_back(ss.str());
    ss.str(std::string());                    // Flush the ss

    // Done, return to the caller
    return (net_params);
}

/* Function: mdlInitializeSizes ===========================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
#define MDL_INIT_SIZE
static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, 5);  /* Number of expected parameters */

#ifndef TRES_SIMULINK_DISABLE_MASK_PROTECTION
    // Perform mask params validity check
    // TODO very basic error check (to be improved).
    const mxArray *mxMsdVarName = ssGetSFcnParam(S,MSG_DESCR_VARNAME);
    if ((mxGetM(mxMsdVarName) != 1) || (mxGetN(mxMsdVarName) == 0))
    {
        ssSetErrorStatus(S, "The message-set description variable cannot be empty");
        return;
    }
    const mxArray *mxNdVarName = ssGetSFcnParam(S,NTWK_DESCR_VARNAME);
    if ((mxGetM(mxNdVarName) != 1) || (mxGetN(mxNdVarName) == 0))
    {
        ssSetErrorStatus(S, "The network description variable cannot be empty (you must specify at least the network topology!)");
        return;
    }
    const mxArray *mxAddLibsPath = ssGetSFcnParam(S,OTHER_DEPS);
    if ((mxGetM(mxAddLibsPath) != 1) || (mxGetN(mxAddLibsPath) == 0))
    {
        ssSetErrorStatus(S, "The Additional Libraries (see the Simulator tab) field cannot be empty");
        return;
    }
#endif

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    // Set the number of input ports to 0
    if (!ssSetNumInputPorts(S, 0)) return;

    // Set the output port to have a dynamic dimension
    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, DYNAMICALLY_SIZED);

    ssSetNumSampleTimes(S, 1);

    ssSetNumDWork(S, 1);  // store the `New pending activations available' flag
    ssSetDWorkWidth(S, 0, 1);
    ssSetDWorkDataType(S, 0, SS_BOOLEAN);
    ssSetNumPWork(S, 1);  // store the tres::Network
    ssSetNumRWork(S, 1);  // store the time_resolution
    ssSetNumNonsampledZCs(S, 1);    // next hit
}

#if defined(MATLAB_MEX_FILE)
#define MDL_SET_OUTPUT_PORT_WIDTH
void mdlSetOutputPortWidth(SimStruct *S, int_T port, int_T width)
{
    ssSetOutputPortWidth(S, port, width); // TODO check if correct
}
#endif

/* Function: mdlInitializeSampleTimes =====================================
 */
#define MDL_INIT_ST
static void mdlInitializeSampleTimes(SimStruct *S)
{
    // Set the sample time
    ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, FIXED_IN_MINOR_STEP_OFFSET);
}

// Function: mdlStart =====================================================
// Abstract:
//   This function is called once at start of model execution. If you
//   have states that should be initialized once, this is the place
//   to do it.
#define MDL_START
static void mdlStart(SimStruct *S)
{
    char *bufSimEng;  // SIMULATION_ENGINE
    int bufSimEngLen;

    // Build the list of parameters that configure the tres::Network
    std::vector<std::string> ns_params = readMaskAndBuildConfVector(S);

    // Get the type of the adapter, i.e., the concrete implementation of tres::Network
    bufSimEngLen = mxGetN( ssGetSFcnParam(S,SIMULATION_ENGINE) )+1;
    bufSimEng = new char[bufSimEngLen];
    mxGetString(ssGetSFcnParam(S,SIMULATION_ENGINE), bufSimEng, bufSimEngLen);
    std::string engine(bufSimEng);
    delete bufSimEng;

    // Instantiate the concrete representation of tres::Network
    std::unique_ptr<tres::Network> ns = Factory<tres::Network>::instance()
                                                    .create(engine, ns_params);

    // Save the C++ object to the pointers vector
    tres::Network *_ns = ns.release();
    ssGetPWork(S)[0] = _ns;

    // Set the `New pending activations available' flag to false
    boolean_T *pendingActivsAvail = (boolean_T*) ssGetDWork(S,0);
    pendingActivsAvail[0] = false;

    // Get the time resolution (actually, its floating point representation)
    std::vector<std::string>::iterator it = ns_params.end()-1;
    double time_resolution = atof((*it).c_str());

    // Save the time resolution to the real vector workspace
    ssGetRWork(S)[0] = time_resolution;
}

#define MDL_INITIALIZE_CONDITIONS
/* Function: mdlInitializeConditions ======================================
 * Abstract:
 *    Initialize outputs to zero.
 */
static void mdlInitializeConditions(SimStruct *S)
{
    // Initialize the standard output port to 0.0
    real_T *y = ssGetOutputPortRealSignal(S,0);
    int_T width = ssGetOutputPortWidth(S,0);
    for (int i = 0; i < width; i++)
        y[i] = 0.0;
}

/* Function: mdlOutputs ===================================================
 */
#define MDL_OUTPUT
static void mdlOutputs(SimStruct *S, int_T tid)
{
    real_T mdl_time = ssGetT(S);

    // Get the C++ object back from the pointers vector
    tres::Network *ns = static_cast<tres::Network *>(ssGetPWork(S)[0]);

    // Get the time resolution back from the real vector workspace
    double time_resolution = ssGetRWork(S)[0];

    // Save the time of next block hit
    long int next_hit_tick = ns->getNextWakeUpTime();

    // If the current time is greater or equal than the next block hit
    if (ssGetT(S) - next_hit_tick/(time_resolution) >= 0.0)
    {
        mexPrintf("\n%s\n\t%s\n\t\tat *time* %.6f\n", ssGetPath(S), __FUNCTION__, mdl_time);
        do
        {
            // Get the first incoming event from the network simulator
            tres::NetworkEvent *e = ns->getNextEvent();

            // Classify the event
            if (e->isGeneratedByAppLevelTraffic())
            {
                // If the event is due to network traffic from/to the application layer,
                // get the instance of network message that has generated the event
                tres::SimMessage *m = e->getGeneratorMessage();

                // debug
                mexPrintf("\t\t\tmessage '%s' transferred (3rdparty sim. time is %.6f)\n", m->getUID().c_str(), e->getTime()/(time_resolution));

                // Add this message to the to-be-triggered message queue
                ns->addMessageToTriggerQueue(m);
            }

            // Process the next event in the underlying network simulator's event queue
            ns->processNextEvent();
        }
        while ( ns->getTimeOfNextEvent() == next_hit_tick );

        // Read which message have to be triggered
        std::vector<int> ports = ns->getPortsToTrigger();

        // For each Message to trigger, set a value of 1.0
        // onto the corresponding output port
        real_T *y = ssGetOutputPortRealSignal(S,0);
        for (std::vector<int>::iterator port = ports.begin();
                port != ports.end();
                    ++port)
        {
            y[*port] = 1.0;
        }

        // Eventually set the `New pending activations available' flag to true
        if (!ports.empty())
        {
            boolean_T *pendingActivsAvail = (boolean_T*) ssGetDWork(S,0);
            pendingActivsAvail[0] = true;
        }
    }
}

/* Function: mdlUpdate ====================================================
 * From Mathworks user's guide: "The method should compute the S-function's
 * states at the current time step and store the states in the S-function's
 * state vector. __The method can also perform any other tasks that the
 * S-function needs to perform at each major time step__.
 */
#define MDL_UPDATE
static void mdlUpdate(SimStruct *S, int_T tid)
{
    // Check the `New pending activations available' flag
    boolean_T *pendingActivsAvail = (boolean_T*) ssGetDWork(S,0);
    if (pendingActivsAvail[0])
    {
        mexPrintf("\n%s\n\t%s\n\t\tat time %.6f\n", ssGetPath(S), __FUNCTION__, ssGetT(S));

        // Reset the values on the out ports
        real_T *y = ssGetOutputPortRealSignal(S,0);
        int_T width = ssGetOutputPortWidth(S,0);
        for (int i = 0; i < width; i++)
            y[i] = 0.0;

        // Reset the `New pending activations available' flag
        pendingActivsAvail[0] = false;
    }
}

#define MDL_ZERO_CROSSINGS
static void mdlZeroCrossings(SimStruct *S)
{
    // Get the C++ object back from the pointers vector
    tres::Network *ns = static_cast<tres::Network *>(ssGetPWork(S)[0]);

    // Get the time resolution back from the real vector workspace
    double time_resolution = ssGetRWork(S)[0];

    ssGetNonsampledZCs(S)[0] = ns->getTimeOfNextEvent()/(time_resolution) - ssGetT(S);
}

// Function: mdlTerminate =================================================
// Abstract:
//   In this function, you should perform any actions that are necessary
//   at the termination of a simulation.  For example, if memory was
//   allocated in mdlStart, this is the place to free it.
static void mdlTerminate(SimStruct *S)
{
    // Get the C++ object back from the pointers vector
    tres::Network *ns = static_cast<tres::Network *>(ssGetPWork(S)[0]);

    // Call its destructor
    delete ns;
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
