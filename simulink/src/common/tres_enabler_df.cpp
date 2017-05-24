#define S_FUNCTION_NAME tres_enabler_df
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

/* Function: mdlInitializeSizes ===========================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    // Number of expected parameters
    ssSetNumSFcnParams(S, 0);

    // States
    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    // I/O ports
    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortWidth(S, 0, 1);
    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, 1);

    ssSetNumDWork(S, 1);  // status of flag `request_served'
    ssSetDWorkWidth(S, 0, 1);
    ssSetDWorkDataType(S, 0, SS_BOOLEAN);

    // Sample times
    ssSetNumSampleTimes(S, 1);
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
}

/* Function: mdlInitializeSampleTimes =====================================
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    // Sample times
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, FIXED_IN_MINOR_STEP_OFFSET);

    // Initialize the Function Call output
    ssSetCallSystemOutput(S,0);
}

/**
 * \brief Initialize discrete state to zero
 */
#define MDL_INITIALIZE_CONDITIONS
static void mdlInitializeConditions(SimStruct *S)
{
    // Set `request_served' to FALSE
    boolean_T *request_served = (boolean_T*) ssGetDWork(S,0);
    request_served[0] = false;
}

/* Function: mdlOutputs ===================================================
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    // Check if the incoming request of execution is pending (not yet served)
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);
    boolean_T *request_served = (boolean_T*) ssGetDWork(S,0);
    if ( (*uPtrs[0] > 0.0) && (!request_served[0]) )
    {
        mexPrintf( "%s, %s, FIRING at time %.16f\n", ssGetPath(S), __FUNCTION__, ssGetT(S) );

        // Function-call generation
        ssCallSystemWithTid(S, 0, tid);

        // Pending request served
        request_served[0] = true;
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
    // Check if the incoming request has already been served
    boolean_T *request_served = (boolean_T*) ssGetDWork(S,0);
    if ( request_served[0] )
    {
        mexPrintf( "%s, %s, MarkSv.at time %.16f\n", ssGetPath(S), __FUNCTION__, ssGetT(S) );

        // Reset to initial conditions (mdlInitializeConditions)
        request_served[0] = false;
    }
}

/* Function: mdlTerminate =================================================
 *    No termination needed, but we are required to have this routine.
 */
static void mdlTerminate(SimStruct *S)
{
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
