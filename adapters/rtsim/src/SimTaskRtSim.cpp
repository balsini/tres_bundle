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
#include <instr.hpp>
#include "SimTaskRtSim.hpp"

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

        // If task was empty
        if (was_empty)
        {
            // Make actInstr to actually point that instruction
            _rts_task->resetInstrQueue();
            (*(_rts_task->getActInstr()))->reset();
        }
        else
        {
            // Initialize the pseudoinstruction
            // needed! (because flag is left uninitialized in the
            // ExecInstr() constructor)
            (*((_rts_task->getActInstr())+1))->reset();
        }

        // Clear the stringstream
        ss.str(std::string());
    }

    void SimTaskRtSim::setAdapteePtr(RTSim::Task* rts_task)
    {
        _rts_task = rts_task;
    }
}
