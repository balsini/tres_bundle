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
 * \file ActiveSimulationManagerRtSim.cpp
 */

#include "ActiveSimulationManagerRtSim.hpp"

namespace tres
{
    ActiveSimulationManagerRtSim* ActiveSimulationManagerRtSim::_instance = NULL;

    ActiveSimulationManagerRtSim::ActiveSimulationManagerRtSim() : _priority_bias(50),
                                                        _max_priority_level(0),
                                                        _sim_ready(false)
    {
    }

    ActiveSimulationManagerRtSim& ActiveSimulationManagerRtSim::getInstance()
    {
        if (!_instance)
            _instance = new ActiveSimulationManagerRtSim();
        return (*_instance);
    }

    void ActiveSimulationManagerRtSim::reset()
    {
        if (_instance)
        {
            delete _instance;
            _instance = NULL;
        }
    }

    int ActiveSimulationManagerRtSim::getPriorityLevel(const std::string& kuid)
    {
        // If the kernel is already registered, return the priority level
        std::map<std::string,int>::const_iterator it = _kuid_priority_map.find(kuid);
        if (it != _kuid_priority_map.end())
            return (it->second);

        // Otherwise, register the kernel and return its priority level
        if (_kuid_priority_map.size() > 0)
            _max_priority_level += _priority_bias;
        return (_kuid_priority_map[kuid] = _max_priority_level);
    }

    int ActiveSimulationManagerRtSim::getMaxPriorityLevel()
    {
        return (_max_priority_level);
    }

    int ActiveSimulationManagerRtSim::getPriorityBias()
    {
        return (_priority_bias);
    }

    void ActiveSimulationManagerRtSim::registerKernel(const std::string& kuid)
    {
        // The kernel must already have been assigned a priority level 
        std::map<std::string,int>::const_iterator it = _kuid_priority_map.find(kuid);
        if (it != _kuid_priority_map.end())
            _ready_kernels.insert(kuid);

        // Check if all the registered kernels signaled they are ready to start
        // the simulation
        _sim_ready = (_ready_kernels.size()==_kuid_priority_map.size());
        if (_sim_ready)
            _ready_kernels.clear();
    }

    bool ActiveSimulationManagerRtSim::kernelsReady()
    {
        return (_sim_ready);
    }
}
