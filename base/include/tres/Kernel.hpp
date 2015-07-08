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
 * \file Kernel.hpp
 */

#ifndef TRES_KERNEL_HDR
#define TRES_KERNEL_HDR
#include <map>
#include <vector>
#include <tres/RTOSEvent.hpp>

namespace tres
{
    /**
     * \addtogroup tres_base_rtos_abstractions
     * @{
     */
    /**
     * \brief An \em abstract class for RTOS kernels
     *
     * Provide infrastructure code and behavior to enable RTOS scheduling simulation
     * by means of 3rd-party simulators.
     *
     * Concrete implementors derive from this abstract class, override the pure virtual
     * methods and complete this behavior with some custom (simulator-specific) code.
     */
    class Kernel
    {

    public:

        typedef std::string BASE_KEY_TYPE;

        /**
         * \brief The virtual destructor
         */
        virtual ~Kernel() = default;

        /**
         * \brief Initialization hook for 3rd-parties RTOS scheduling simulation engines
         */        
        virtual void initializeSimulation(const double, const double* const*) = 0;

        /**
         * \brief Process the next simulation step
         */
        virtual void processNextEvent() = 0;

        /**
         * \brief Return the next event in the RT simulation engine queue to be processed
         */        
        virtual RTOSEvent* getNextEvent() = 0;

        /**
         * \brief Return the RT-Simulator time of the next event in the engine event queue
         */
        virtual int getTimeOfNextEvent() = 0;

        /**
         * \brief Return the RT-Simulator time at which Kernel execution will be triggered again
         */
        virtual int getNextWakeUpTime() = 0;

        /**
         * \brief Update the list of running tasks
         */
        virtual void getRunningTasks() = 0;

        /**
         * \brief Activate the set of aperiodic tasks corresponding to the activation request indices
         */
        virtual void activateAperiodicTasks(std::vector<int>&, int) = 0;

        /**
         * \brief Add new scheduled tasks to the list of tasks to be scheduled
         */
        void addNewTasksToTriggerQueue();

        /**
         * \brief Mark running tasks
         */
        void markNewScheduledTasks();

        /**
         * \brief Clear the new Job Task Flag
         */
        void clearStartTaskMark(SimTask *);

        /**
         * \brief Add a task to the trigger queue
         */
        void addTaskToTriggerQueue(SimTask *);

        /**
         * \brief Return the block's port number of a task in a S/R implementation
         */
        int getPort(SimTask *);

        /**
         * \brief Return the a vector of block's ports to trigger in S/R implementation
         *
         * \note This function has side-effects, since the list of tasks scheduled for
         * a new job execution (\ref _tasks_to_trigger, see below) gets cleared once the
         * port indices are computed
         */
        std::vector<int> getPortsToTrigger();

        /**
         * \brief Get the job status for a given task
         */
        int getFlag(const std::string&);

        /**
         * \brief Get the name (univoque identifier) of the instance
         */
        const std::string& getName();

    protected:

        /** Instance ID */
        std::string _kernel_name;

        /** Map of the task-uid and (S/R)block-port correspondence */
        std::map<std::string, int> _task_port_map;

        /** Map the aperiodic request index into the corresponding task (index) */
        std::map<int, int> _aper_req_task_map;

        /** List of tasks scheduled for a new job execution */
        std::vector<std::string> _tasks_to_trigger;

        /** List of tasks in currently execution */
        std::vector<std::string> _running_tasks;

        /** List of job status for each task: not yet started since the activation of current job (0), at least an activation since the activation of current job (1) */
        std::map<std::string, int> _jobs_status;

    };
    /**
     * @}
     */
}
#endif // TRES_KERNEL_HDR
