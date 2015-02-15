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
 * \file EventRtSim.hpp
 */

#ifndef TRES_EVENTRTSIM_HDR
#define TRES_EVENTRTSIM_HDR
#include <string>
#include <event.hpp> // MetaSim::Event (RTSim)
#include <tres/RTOSEvent.hpp>
#include "SimTaskRtSim.hpp"

namespace tres
{
    /**
     * \addtogroup tres_rtsim
     * @{
     */
    /**
     * \brief Object adapter for RTSim events
     */
    class EventRtSim : public tres::RTOSEvent
    {

        friend class KernelRtSim;

    public:

        /**
         * \brief Default constructor
         */
        EventRtSim();

        /**
         * \brief The virtual destructor
         *
         * \note Default implementation is fine because the adaptee object
         * exists in RTSim and its correct destruction is performed by RTSim
         */
        virtual ~EventRtSim() = default;

        virtual std::string getName() const;

        virtual long int getTime() const;

        virtual tres::RTOSEventType getType() const;

        virtual tres::SimTask* getGeneratorTask();

    protected:

        /**
         * \brief Utility method to initialize an instance of this class with an
         * RtSim event
         * \note It's used by tres::KernelRtSim, which is a friend of this class
         */
        virtual void setAdapteePtr(MetaSim::Event*);

    protected:

        /** The base RTOS event representation in RTSim (Adaptee) */
        MetaSim::Event *_ms_evt;

        /** The RTSim task which has generated the event */
        SimTaskRtSim _gen_task;

    };
    /** @} */
}
#endif // TRES_EVENTRTSIM_HDR
