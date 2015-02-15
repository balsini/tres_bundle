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
 * \file EventRtSim.cpp
 */

#include <typeinfo>
#include <instr.hpp>
#include <taskevt.hpp>
#include "EventRtSim.hpp"

namespace tres
{
    EventRtSim::EventRtSim()
    {
        _ms_evt = NULL;
    }

    std::string EventRtSim::getName() const
    {
        return (typeid(*_ms_evt).name());
    }

    long int EventRtSim::getTime() const
    {
        return (_ms_evt->getTime());
    }

    tres::RTOSEventType EventRtSim::getType() const
    {
        if (dynamic_cast<RTSim::EndInstrEvt*>(_ms_evt))
            return tres::RTOSEventType::END_INSTRUCTION;

        else if (dynamic_cast<RTSim::EndEvt*>(_ms_evt))
            return tres::RTOSEventType::END_TASK;

        else if (dynamic_cast<RTSim::SchedEvt*>(_ms_evt))
            return tres::RTOSEventType::PREEMPTION;

        else
            return tres::RTOSEventType::OTHER;
    }

    tres::SimTask* EventRtSim::getGeneratorTask()
    {
        if (dynamic_cast<RTSim::EndInstrEvt*>(_ms_evt))
        {
            RTSim::EndInstrEvt* rts_evt = dynamic_cast<RTSim::EndInstrEvt*>(_ms_evt);
            _gen_task.setAdapteePtr( rts_evt->getInstruction()->getTask() );
            return &_gen_task;
        }
        else if (dynamic_cast<RTSim::TaskEvt*>(_ms_evt))
        {
            RTSim::TaskEvt* rts_evt = dynamic_cast<RTSim::TaskEvt*>(_ms_evt);
            _gen_task.setAdapteePtr( rts_evt->getTask() );
            return &_gen_task;
        }
        return NULL;
    }

    void EventRtSim::setAdapteePtr(MetaSim::Event* ms_evt)
    {
        _ms_evt = ms_evt;
    }
}
