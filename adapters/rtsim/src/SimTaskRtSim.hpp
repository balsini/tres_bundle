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
 * \file SimTaskRtSim.hpp
 */

#ifndef TRES_SIMTASKRTSIM_HDR
#define TRES_SIMTASKRTSIM_HDR
#include <string>
#include <task.hpp> // RTSim::Task
#include <tres/SimTask.hpp>

namespace tres
{
    /**
     * \addtogroup tres_rtsim
     * @{
     */
    /**
     * \brief Object adapter for RTSim tasks
     */
    class SimTaskRtSim : public tres::SimTask
    {

        friend class EventRtSim;

    public:

        /**
         * \brief Default constructor
         */
        SimTaskRtSim();

        /**
         * \brief The virtual destructor
         *
         * \note Default implementation is fine because the adaptee object
         * exists in RTSim and its correct destruction is performed by RTSim
         */
        virtual ~SimTaskRtSim() = default;

        virtual std::string getUID() const;

        virtual bool isEmpty();

        virtual void discardInstructions();

        virtual void addInstruction(int);

    protected:

        /**
         * \brief Utility method to initialize an instance of this class with an
         * RtSim task
         * \note It's used by tres::EventRtSim, which is a friend of this class
         */
        virtual void setAdapteePtr(RTSim::Task*);

    protected:

        /** The base RT task representation in RTSim (Adaptee) */
        RTSim::Task *_rts_task;

    };
    /** @} */
}
#endif // TRES_SIMTASKRTSIM_HDR
