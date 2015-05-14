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
 * \file KernelRtSim.cpp
 */

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <rmsched.hpp>   // RTSim::RMScheduler (does not have a creator fct)
#include <mrtkernel.hpp> // RTSim::MRTKernel
#include <tres/Factory.hpp>
#include <tres/ParseUtils.hpp>
#include <tres_rtsim/KernelRtSim.hpp>

namespace tres
{
    KernelRtSim::KernelRtSim(const std::string& sp_descr, const int num_cores, const std::vector<std::string>& ts_descr)
    {
        using namespace tres_parse_utils;

        // Get the sched. policy description parameters
        std::vector<std::string> sp_parms = split_instr(sp_descr);

        // Build a suitable parameter vector for use with the factory
        std::vector<std::string> sp_fact(sp_parms.begin()+1, sp_parms.end());

        // **Build instance** (the RTSim::Scheduler)
        std::unique_ptr<RTSim::Scheduler> rts_sched;
        if (sp_parms[0] == "DEADLINE_MONOTONIC")
            // A deadline monotonic scheduler is built
            // without using the factory, since the class that
            // represents it does not have a creator function
            rts_sched = std::unique_ptr<RTSim::Scheduler>(new RTSim::RMScheduler());
        else
            rts_sched = Factory<RTSim::Scheduler>::instance().create(sp_parms[0], sp_fact);
        if (rts_sched.get() == NULL) throw std::runtime_error(sp_parms[0]);
        _rts_sched = rts_sched.release();

        // **Build instance** (the RTSim Kernel, single-/multi-core)
        if (num_cores > 1)
            _rts_kern = new RTSim::MRTKernel(_rts_sched, num_cores);
        else
            _rts_kern = new RTSim::RTKernel(_rts_sched);

        // **Build instances** (the RTSim Tracers)
        ttrace = new RTSim::TextTrace("trace.txt");
        jtrace = new RTSim::JavaTrace("trace.trc");

        // Manage tasks in the task-set
        int aper_req_idx = 0;
        for (std::vector<std::string>::size_type i = 0; i < ts_descr.size(); i++)
        {
            // Get the task-set description parameters
            //
            // It is _guaranteed_ that the parameters in the i-th
            // entry of the ts_descr vector (which is provided by the caller)
            // are in the following order
            //   - type (e.g., 'PeriodicTask', etc.)
            //   - name
            //   - iat  (InterArrival Time)
            //   - rdl  (Relative DeadLine)
            //   - ph   (activation PHase)
            //
            // The order of name, iat, rdl and ph matches the one required by the
            // RTSim Task creator function. Hence, the parameters can be given
            // in that order to the creator function (see below).
            //
            std::vector<std::string> ts_parms = split_instr(ts_descr[i]);

            // Get the task name
            std::stringstream ss;
            ss << ts_parms[1];

            // Build a suitable parameter vector for use with the factory
            std::vector<std::string> ts_fact(ts_parms.begin()+2, ts_parms.begin()+5);

            // Add the task name currently stored in the stringstream.
            // (This is required by the RTSim Task creator function)
            ts_fact.push_back(ss.str());

            // **Build instance** (the RTSim Task)
            std::unique_ptr<RTSim::Task> task = Factory<RTSim::Task>::instance()
                                                    .create( ts_parms[0], ts_fact );
            if (task.get() == NULL) throw std::runtime_error(ts_parms[0]);
            RTSim::Task *tsk = task.release();

            // Attach the Tracers
            ttrace->attachToTask(tsk);
            tsk->setTrace(jtrace);

            // Register the task/port correspondency
            _task_port_map[ss.str()] = i;

            // Register the correspondency between
            // aperiodic-activation request index
            // and task (if any)
            if (ts_parms[0] == "Task")
                // AperiodicTask
                _aper_req_task_map[aper_req_idx++] = i;

            // Initialize the flags of Job's status (default 0)
            _jobs_status[ss.str()] = 0;

            // Add the task to the RTSim scheduler/kernel, with priority (if any).
            // The priority is only considered when a task is attached to
            // a FPSched Scheduler; in all the other cases it's ignored
            std::string tsk_prio("");
            if (ts_parms.size() > 5)
                tsk_prio = ts_parms[5];
            _rts_kern->addTask(*tsk, tsk_prio);

            // Add the task to the list of handled tasks
            _rts_tasks.push_back(tsk);
        }
    }

    KernelRtSim::~KernelRtSim() noexcept(true)
    {
        for (unsigned int i = 0; i < _rts_tasks.size(); i++)
            delete _rts_tasks[i];
        delete _rts_sched;
        delete _rts_kern;        
        MetaSim::Simulation::getInstance().clearEventQueue();
        delete ttrace;
        delete jtrace;
    }

    tres::Kernel* KernelRtSim::createInstance(std::vector<std::string>& par)
    {
        // The input information for this function is _guaranteed to have the following form:
        //  - the number of tasks in the task-set (#tasks) - std::string (1)      <-- vector.begin()
        //  - the task-set description                     - std::string (#tasks)
        //  - the scheduling policy description            - std::string (1)
        //  - the number of CPU cores                      - std::string (1)
        //  - the time resolution                          - std::string (1)
        //                                                                        <-- vector.end()
        using namespace tres_parse_utils;
        std::vector<std::string>::iterator it;  // Convenience iterator

        // Get the number of tasks
        int num_tasks = atoi(par[0].c_str());

        // Get the time resolution
        it = par.end() - 1;
        double time_resolution = atof((*it).c_str());

        // Modify the task-set description
        // (according to the time resolution and tasks' type)
        std::vector<std::string> mod_ts_descr;
        for (int i = 0; i < num_tasks; i++)
        {
            std::stringstream ss;

            // Get the task-set description parameters
            std::vector<std::string> ts_parms = split_instr(par[1+i]);

            // Check (and eventually convert) the task type
            if (ts_parms[0] == "AperiodicTask")
                // Convert the task type so that it's compatible with RTSim
                ss << "Task;";
            else
                // Keep the task type as is
                ss << ts_parms[0] << ';';

            // ts_parms[1] is the NAME -- DON'T MODIFY (Keep the task name as is)
            ss << ts_parms[1] << ';';

            // Modify IAT
            ss << time_resolution*atof(ts_parms[2].c_str()) << ';';

            // Modify RDL
            ss << time_resolution*atof(ts_parms[3].c_str()) << ';';

            // Modify OFFSET
            ss << time_resolution*atof(ts_parms[4].c_str()) << ';';

            // Keep the rest as is
            for (std::vector<std::string>::size_type j = 5; j < ts_parms.size(); j++)
                ss << ts_parms[j] << ';';

            mod_ts_descr.push_back(ss.str());
        }

        // Now point to the sched policy description given by the caller
        it = par.begin() + 1 + num_tasks;

        // Get the sched. policy description parameters
        std::vector<std::string> sp_parms = split_instr(*it);

        // Convert the "abstract" policy into the RTSim equivalent
        std::stringstream ss;   // Convenience stringstream
        if (sp_parms[0] == "EDF")
            ss << "EDFSched;";
        else if (sp_parms[0] == "FIXED_PRIORITY")
            ss << "FPSched;";
        else
            // Keep the sched. policy type as is.
            // This also holds for "DEADLINE_MONOTONIC",
            // which does not have a creator function
            ss << sp_parms[0] << ';';

        // Keep the rest (if any) as is
        for (std::vector<std::string>::size_type j = 1; j < sp_parms.size(); j++)
            ss << sp_parms[j] << ';';

        // Create a scheduling policy description that is
        // suitable for use with RTSim
        std::string sp_descr = ss.str();
        ss.str(std::string());  // Flush the ss

        // Get the number of CPU cores
        it = par.end() - 2;
        int num_cores = atoi((*it).c_str());

        // Finally, construct the object
        return new KernelRtSim(sp_descr, num_cores, mod_ts_descr);
    }

    void KernelRtSim::initializeSimulation(const double time_resolution, const double * const *c_time)
    {
        //static int my_counter = 0; // XXX

        for (auto task = _rts_tasks.begin();
                    task != _rts_tasks.end();
                        ++task)
        {
            std::stringstream ss;
            ss << "fixed(" << time_resolution*( **(c_time + _task_port_map[(*task)->getName()]) ) << ");";

            // Add the pseudoinstruction to the task's instruction_q
            (*task)->insertCode(ss.str());

            // Flush the stream...
            ss.str(std::string());
        }

        //if (my_counter++%2 == 1) { // XXX
        MetaSim::Simulation::getInstance().dbg.enable("All");
        MetaSim::Simulation::getInstance().dbg.setStream("debug.txt");

        MetaSim::Simulation::getInstance().initRuns();
        MetaSim::Simulation::getInstance().initSingleRun();
        //} // XXX
    }

    void KernelRtSim::processNextEvent()
    {
        MetaSim::Simulation::getInstance().sim_step();
    }

    tres::RTOSEvent* KernelRtSim::getNextEvent()
    {
        _next_event.setAdapteePtr(MetaSim::Event::getFirst());
        return &_next_event;
    }

    int KernelRtSim::getTimeOfNextEvent()
    {
        return (MetaSim::Event::getFirst()->getTime());
    }

    int KernelRtSim::getNextWakeUpTime()
    {
        return (getTimeOfNextEvent());
    }

    void KernelRtSim::getRunningTasks()
    {
        _running_tasks.clear();
        _running_tasks = _rts_kern->getRunningTasks();
    }

    void KernelRtSim::activateAperiodicTasks(std::vector<int>& aper_activ_idx, int sim_time)
    {
        int num_aper_activs = aper_activ_idx.size();
        RTSim::Task* aper_task;
        for (int i = 0; i < num_aper_activs; ++i)
        {
            aper_task = _rts_tasks[_aper_req_task_map[aper_activ_idx[i]]];
            aper_task->arrEvt.post(sim_time);
        }
    }
}
