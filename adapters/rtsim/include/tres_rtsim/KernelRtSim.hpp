/*-----------------------------------------------------------------------------------
 *  Copyright (c) 2014,2015, ReTiS Lab., Scuola Superiore Sant'Anna.
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
 * \file KernelRtSim.hpp
 */

#ifndef TRES_KERNELRTSIM_HDR
#define TRES_KERNELRTSIM_HDR
#include <scheduler.hpp> // RTSim::Scheduler
#include <texttrace.hpp> // RTSim::TextTrace
#include <jtrace.hpp>    // RTSim::JavaTrace
#include <tres/Kernel.hpp>
#include "../../src/EventRtSim.hpp"

namespace tres
{
    /**
     * \addtogroup tres_rtsim
     * @{
     */
    /**
     * \brief Object adapter for RTSim kernels
     */
    class KernelRtSim : public tres::Kernel
    {

    public:

        /**
         * \brief Creator function used for object construction
         * according to the Factory Method pattern
         */
        static tres::Kernel* createInstance(std::vector<std::string>&);

        /**
         * \brief The destructor
         */
        virtual ~KernelRtSim();

        /**
         * \brief RTSim-specific implementation of the initialization-hook procedure
         */
        virtual void initializeSimulation(const double, const double * const *);

        virtual void processNextEvent();

        /**
         * \brief Return the next event from the RTSim event queue
         */
        virtual tres::RTOSEvent* getNextEvent();

        /**
         * \brief Return the time of the next event in the RTSim event queue
         */
        virtual int getTimeOfNextEvent();

        /**
         * \brief Return the time at which RTSim will drive the next Kernel execution
         */
        virtual int getNextWakeUpTime();

        virtual void getRunningTasks();   

        virtual void activateAperiodicTasks(std::vector<int>&, int);

    protected:

        /** The base kernel representation in RTSim (Adaptee) */
        RTSim::RTKernel *_rts_kern;

        /**
         * \name Bunch of RTSim entities needed by RTSim::RTKernel
         *
         * These entities must be pre-allocated and initialized to be
         * used by \ref _rts_kern and finally must be destroyed when
         * the RT scheduling simulation ends
         * @{
         */
        /** The real-time scheduler */
        RTSim::Scheduler *_rts_sched;
        /** The resource manager (not used for now by this adapter) */
        RTSim::ResManager *_rts_resMng;
        /** The (RTSim) task objects making the task set */
        std::vector<RTSim::Task*> _rts_tasks;
        /**
         * @}
         */

        /** Next RTSim engine event */
        EventRtSim _next_event;

        /**
         * \name RTSim tracers (for debugging purposes)
         * @{
         */
        /** Enable a textual representation of the simulation trace */
        RTSim::TextTrace *ttrace;
        /** Enable a graphical representation of the simulation trace
         * \warning A (very!) old version of JTracer and JDK (v1.5) is required
         * to visualize the output of a RTSim::JavaTrace
         */
        RTSim::JavaTrace *jtrace;
        /**
         * @}
         */

    private:

        /**
         * \brief Prevent default construction
         */
        KernelRtSim();

        /**
         * \brief Construct from external parameters
         */
        KernelRtSim(const std::string&, const int, const std::vector<std::string>&);

    };
    /** @} */
}
#endif // TRES_KERNELRTSIM_HDR
