/*-----------------------------------------------------------------------------------
 *  Copyright (c) 2015, ReTiS Lab., Scuola Superiore Sant'Anna.
 *
 *  This file is part of tres_rtsim.
 *
 *  tres_rtsim is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  tres_rtsim is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with tres_rtsim; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *--------------------------------------------------------------------------------- */

/**
 * \file ActiveSimulationManagerRtSim.hpp
 */

#ifndef TRES_ACTIVESIMULATIONMANAGERRTSIM_HDR
#define TRES_ACTIVESIMULATIONMANAGERRTSIM_HDR
#include <string>
#include <map>
#include <unordered_set>
#include <memory>

namespace tres
{
    /**
     * \addtogroup tres_rtsim
     * @{
     */
    /**
     * \brief Manage the MetaSim event queue to allow simultaneous utilization
     * by many RTSim::RTKernel objects
     */
    class ActiveSimulationManagerRtSim
    {

    public:

        /**
         * \brief Get the singleton instance of the manager
         */
        static ActiveSimulationManagerRtSim& getInstance();

        /**
         * \brief Reset the singleton instance at the end of the simulation life-cycle
         */
        static void reset();

        /**
         * \brief The destructor
         */
        ~ActiveSimulationManagerRtSim() = default;

        /**
         * \brief Get the priority level of a given kernel identified by its UID
         *
         * \note The function inserts a new priority level for the UID, when the
         * UID does not match any key element in the register (and returns a
         * reference to its priority level).
         */
        int getPriorityLevel(const std::string&);

        /**
         * \brief Get the maximum priority level in the shared event queue
         *
         * \note The maximum priority level depends on the number of
         * RTSim::RTKernel objects sharing the queue 
         */
        int getMaxPriorityLevel();

        /**
         * \brief Get the bias between priority levels
         *
         * \note The bias value is hardcoded for now (we need some simple
         * mechanism to pass it as parameter.
         */
        int getPriorityBias();

        /**
         * \brief Register a kernel instance ready to take an action
         *
         * \note The action logics is in the caller. An ActiveSimulationManagerRtSim
         * is agnostic wrt the action to take. It just provides a mechanism to ease
         * the registration of kernels, when there is the need for taking an action
         * after that \em all the kernel are in a particular state (application
         * dependent). Examples are all the kernels have been initialized with tasks
         * and instructions to execute, so the whole simulation context can be safely
         * initialized; or all the kernels have no other events to simulate, so the
         * whole simulation can stop and the ActiveSimulationManagerRtSim instance
         * deleted
         */
        void registerKernel(const std::string&);

        /**
         * \brief Check if all the kernel are ready to take an action coded in the
         * caller context
         *
         * \note This function must be called after \ref registerKernel()
         */
        bool kernelsReady();

    private:

        /**
         * \brief Prevent default construction
         */
        ActiveSimulationManagerRtSim();

        /**
         * \brief Prevent copy construction
         */
        ActiveSimulationManagerRtSim(const ActiveSimulationManagerRtSim&);

    private:

        /** kernel-uid/priority-level correspondence */
        std::map<std::string, int> _kuid_priority_map;

        /** The instance of active simulation manager */
        static ActiveSimulationManagerRtSim* _instance;

        /** Bias between priority levels (hardcoded for now) */
        int _priority_bias;

        /** Maximum priority level (depends on the number of registered kernels) */
        int _max_priority_level;

        /** List of kernels ready to take an action (action logics is in the
         * caller) */
        std::unordered_set<std::string> _ready_kernels;

        /** Flag to signal if all the registered kernels are ready to take an
         * action (action logics is in the caller) */
        bool _sim_ready;

    };
    /** @} */
}
#endif // TRES_ACTIVESIMULATIONMANAGERRTSIM_HDR
