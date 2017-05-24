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
 * \file SimTaskRtSim.cpp
 */

#include <sstream>          // std::stringstream
#include <vector>
#include <instr.hpp>        // getActInstr()
#include <exeinstr.hpp>     // TODO: RTSim must manage instructions priorities
#include "SimTaskRtSim.hpp"
#include "ActiveSimulationManagerRtSim.hpp"

namespace tres
{
    SimTaskRtSim::SimTaskRtSim()
    {
        _rts_task = NULL;
    }

    std::string SimTaskRtSim::getUID() const
    {
        return (_rts_task->getName());
    }

    bool SimTaskRtSim::isEmpty()
    {
        return(_rts_task->getInstrQueue().empty());
    }

    void SimTaskRtSim::discardInstructions()
    {
        _rts_task->discardInstrs();
    }

    void SimTaskRtSim::addInstruction(int duration)
    {
        // Transform the given instruction duration in
        // *one* fixed computation-time RTSim pseudoinstruction
        std::stringstream ss;
        ss << "fixed(" << duration << ");";

        // Check if task is empty
        bool was_empty = isEmpty();

        // Add the pseudoinstruction to the task's instruction_q
        _rts_task->insertCode(ss.str());
        std::vector<RTSim::Instr*>::iterator it;

        // If task was empty
        if (was_empty)
        {
            // Make actInstr to actually point that instruction
            _rts_task->resetInstrQueue();
            it = _rts_task->getActInstr();
        }
        else
            it = (_rts_task->getActInstr())+1;

        // Initialize the pseudoinstruction. Needed!
        // (because flag is left uninitialized in the ExecInstr() constructor)
        (*(it))->reset();

        // TODO
        // The following lines shouldn't be here. Managing the priority of an
        // added instruction should be taken into account by RTSim (insertCode())
        //
        // Take care to not reset priority levels in reset()!
        if ( dynamic_cast<RTSim::ExecInstr*>(*it) )
        {
            RTSim::ExecInstr* rts_ei = dynamic_cast<RTSim::ExecInstr*>(*it);
            int evt_priority = rts_ei->_endEvt.getPriority();
            int priority_bias = ActiveSimulationManagerRtSim::getInstance().getPriorityBias();
            if ( !((evt_priority >= _priority_level) &&
                      (evt_priority < priority_bias+_priority_level)) )
                rts_ei->_endEvt.setPriority(_priority_level + evt_priority);
        }
        // XXX
        // Is RTSim::ExecInstr the only kind of instruction with events
        // to be managed with priorities?
        ////////////////////////////////////////////////////////////////////////

        // Clear the stringstream
        ss.str(std::string());
    }

    void SimTaskRtSim::setAdapteePtr(RTSim::Task* rts_task)
    {
        _rts_task = rts_task;
    }
}
