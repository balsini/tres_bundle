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
 * \file tres_kernel_utils.cpp
 */

/**
 * \brief Convert cell-array-based entity descriptions into vector-of-strings-based descriptions
 *
 * A number of kernel entities (e.g., task sets, scheduling policies) are described
 * through MATLAB variables that are cell arrays. Such arrays have one or more rows
 * (depending on what they are used for).
 *
 * The first parameter in each row is guaranteed to be a std::string. The other parameters are
 * guaranteed to be numeric or empty (MATLAB) cells. Error checking is performed at MATLAB level
 * by using mask callbacks
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

    // Convert the description of each task
    for (int i = 0; i < num_entries; ++i)
    {
        // The stringstream
        std::stringstream ss;

        // Put the entity type into the stringstream
        mxArray *entity_type = mxGetCell(mx_var, i);
        char *type_name = new char[mxGetN(entity_type)+1];
        mxGetString(entity_type, type_name, mxGetN(entity_type)+1);
        ss << type_name << ';';
        delete type_name;

        // Put the entity name into the stringstream
        mxArray *entity_name = mxGetCell(mx_var, i+num_entries);
        char *name_string = new char[mxGetN(entity_name)+1];
        mxGetString(entity_name, name_string, mxGetN(entity_name)+1);
        ss << name_string << ';';
        delete name_string;

        // Put the other parameters (numerical) into the stringstream
        for (int j = 2; j < num_params; ++j)
        {
            mxArray *entity_p = mxGetCell(mx_var, (i + num_entries * j));

            // if the cell is _not_ empty
            if (mxGetM(entity_p)*mxGetN(entity_p) != 0)
            {
                double *param = mxGetPr(entity_p);
                ss << *param << ';';
            }
        }

        // Insert the stringstream into the description vector
        out_descr.push_back(ss.str());
    }

    // Return the vector-of-string-based description
    return out_descr;
}

/**
 * \brief Build the list of parameters that configure the tres::Kernel
 *
 * The tres::Kernel class (or, more precisely, its underlying concrete
 * implementation) needs to be configured with a bunch of information, such
 * as the description of the task set, the scheduling policy, some computing-platform
 * details like number of CPU cores, and time resolution, to name only a few. This
 * information is stored inside the tres_kernel block as mask parameters.
 *
 * This function reads the mask parameters and returns a vector-of-string-based
 * description of the information needed by the tres::Kernel class. Specifically,
 * the information returned by this function has the following form:
 *   - the number of tasks in the task-set (#tasks) - std::string (1)      <-- vector.begin()
 *   - the task-set description                     - std::string (#tasks)
 *   - the scheduling policy description            - std::string (1)
 *   - the number of CPU cores                      - std::string (1)
 *   - the time resolution                          - std::string (1)
 *                                                                         <-- vector.end()
 *
 * No err checking is performed when reading mask parameters since these are already
 * guaranteed to be valid at MATLAB level (err checking performed by mask callbacks).
 */
static std::vector<std::string> readMaskAndBuildConfVector(SimStruct *S)
{
    char *bufTsd,       // TS_DESCR_VARNAME
         *bufSchedPol,  // SCHEDULING_POLICY
         *bufSpd,       // SP_DESCR_VARNAME
         *bufDeadMiss,  // DEADLINE_MISS_RULE
         *bufTimeRes;   // TIME_RESOLUTION
    int bufTsdLen, bufSchedPolLen, bufSpdLen, bufDeadMissLen, bufTimeResLen;
    std::vector<std::string> kern_params; // The return list of parameters
    std::stringstream ss;                 // A convenience stringstream

    // Get the number of tasks (it is equal to the size of output port)
    int_T num_tasks = ssGetOutputPortWidth(S,0);

    // Get the name of the workspace variable for the task set
    bufTsdLen = mxGetN( ssGetSFcnParam(S,TS_DESCR_VARNAME) )+1;
    bufTsd = new char[bufTsdLen];
    mxGetString(ssGetSFcnParam(S,TS_DESCR_VARNAME), bufTsd, bufTsdLen);

    // Get the actual task set description
    std::vector<std::string> ts_descr = cellArrayDescrToVectorOfStrings(mexGetVariablePtr("base", bufTsd));
    delete bufTsd;

    // **Insert** the number of tasks and
    //            the task set description in the return list
    ss << num_tasks;
    kern_params.push_back(ss.str());
    ss.str(std::string());                    // Flush the ss
    kern_params.insert(kern_params.begin()+1,
                           ts_descr.begin(),
                               ts_descr.end());

    // Get the scheduling policy
    bufSchedPolLen = mxGetN( ssGetSFcnParam(S,SCHEDULING_POLICY) )+1;
    bufSchedPol = new char[bufSchedPolLen];
    mxGetString(ssGetSFcnParam(S,SCHEDULING_POLICY), bufSchedPol, bufSchedPolLen);
    std::vector<std::string> sp_descr;
	sp_descr.push_back(std::string(bufSchedPol) + ';');
    delete bufSchedPol;

    // Check if it's a custom sched. policy
    if (sp_descr[0] == "OTHER;")
    {
        // Get the name of the workspace variable for the custom sched. policy
        bufSpdLen = mxGetN( ssGetSFcnParam(S,SP_DESCR_VARNAME) )+1;
        bufSpd = new char[bufSpdLen];
        mxGetString(ssGetSFcnParam(S,SP_DESCR_VARNAME), bufSpd, bufSpdLen);

        // Get the actual custom sched. policy description
        // (It's a std::vector<std::string> with size() == 1)
        sp_descr = cellArrayDescrToVectorOfStrings(mexGetVariablePtr("base", bufSpd));
        delete bufSpd;
    }

    // **Insert** the sched. policy description in the return list
    kern_params.push_back(sp_descr[0]);

    // TODO. Get the scheduler action on deadline miss
    // TODO. **Insert** the scheduler action on deadline miss in the return list

    // Get the number of CPU cores parameter
    int num_cores = static_cast<int>(mxGetScalar(ssGetSFcnParam(S, NUMBER_OF_CORES)));

    // **Insert** the number of cores in the return list
    ss << num_cores;
    kern_params.push_back(ss.str());
    ss.str(std::string());                    // Flush the ss

    // Get the time resolution
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
    kern_params.push_back(ss.str());
    ss.str(std::string());                    // Flush the ss

    // Done, return to the caller
    return (kern_params);
}
