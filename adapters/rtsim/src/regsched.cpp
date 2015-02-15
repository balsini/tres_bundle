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
 * \file regsched.cpp
 *
 * Register RTSim schedulers into the tres::Factory registry.
 *
 * \warning Users should never access objects of this file; it is used just
 * for initialization of the objects needed for the abstract factory that
 * creates RTSim schedulers.
 */

#include <fifosched.hpp>
#include <fpsched.hpp>
#include <edfsched.hpp>
#include <rrsched.hpp>
#include <tres/Factory.hpp>

namespace tres
{
    static registerInFactory<RTSim::Scheduler,
                             RTSim::FIFOScheduler,
                             std::string>
    registerfifo("FIFOSched");

    static registerInFactory<RTSim::Scheduler,
                             RTSim::FPScheduler,
                             std::string>
    registerfp("FPSched");

    static registerInFactory<RTSim::Scheduler,
                             RTSim::EDFScheduler,
                             std::string>
    registeredf("EDFSched");

    static registerInFactory<RTSim::Scheduler,
                             RTSim::RRScheduler,
                             std::string>
    registerrr("RRSched");
}
