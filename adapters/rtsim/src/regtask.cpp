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
 * \file regtask.cpp
 *
 * Register RTSim task types into the tres::Factory registry.
 *
 * \warning Users should never access objects of this file; it is used just
 * for initialization of the objects needed for the abstract factory that
 * creates RTSim tasks.
 */

#include <rttask.hpp>
#include <tres/Factory.hpp>

namespace tres
{
    static registerInFactory<RTSim::Task,
                             RTSim::Task,
                             std::string>
    registerRT("Task");

    static registerInFactory<RTSim::Task,
                             RTSim::PeriodicTask,
                             std::string>
    registerPeriodic("PeriodicTask");
}
