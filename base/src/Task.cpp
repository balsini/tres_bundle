/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2014,2015, ReTiS Lab., Scuola Superiore Sant'Anna.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the ReTiS Lab. nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/**
 * \file Task.cpp
 */

#include <iostream>
#include <memory>
#include <tres/Factory.hpp>
#include <tres/ParseUtils.hpp>
#include <tres/Task.hpp>

namespace tres
{
    Task::Task(const std::vector<std::string>& instr)
    {
        // Add pseudo instructions
        for (unsigned int i=0; i < instr.size(); ++i)
        {
            using namespace tres_parse_utils;
            // Extract the token ("fixed", "delay", ...)
            std::string token = get_token(instr[i]);

            // Extract the list of parameters
            std::string param = get_param(instr[i]);
            std::vector<std::string> par_list = split_param(param);

            // Create the corresponding Segment
            std::unique_ptr<Segment> curr = Factory<Segment>::instance().create(token, par_list);

            // Add it to the segment_q
            Segment *curr_instr = curr.release();
            if (!curr_instr) throw ParseExc("Task", token);
            _segment_q.push_back(curr_instr);
        }

        // Initialize the pointer to the running segment
        _run_seg = _segment_q.begin();

        // Initialize the residual time to segment completion
        _run_seg_duration = (*_run_seg)->getDuration();
    }

    int Task::getNumberOfSegments() const
    {
        return (_segment_q.size());
    }

    double Task::getSegmentDuration() const
    {
        return (_run_seg_duration);
    }

    int Task::processSegment()
    {
        // Index of the segment which is being activated
        // for the execution -- act_seg_idx indicates that
        // the barrier of the last segment must be activated
        // when _run_seg == _segment_q.end() (see below)
        int act_seg_idx = _run_seg - _segment_q.begin();

        // Note that when _run_seg points to the
        // _past-the-end_ segment (i.e., the
        // theoretical segment that would follow
        // the last segment in the task) the _run_seg_duration
        // has already been set in the previous call
        // of this function.
        //
        // If _run_seg already points to the
        // past-the-end segment
        if (_run_seg == _segment_q.end())
        {
            // The next task segment to be
            // executed is the first one
            _run_seg = _segment_q.begin();

            // do nothing else! (_run_seg_duration has
            // already initialized)
        }
        else
        {
            // Go to the next segment and
            // check if the increment
            // operation caused _run_seg
            // to point the past-the-end
            // segment
            if (++_run_seg == _segment_q.end())
                // Re-Initialize _run_seg_duration with
                // the duration of first segment: use the
                // negative sign to indicate the task has
                // completed its execution
                _run_seg_duration = -(*_segment_q.begin())->getDuration();
            else
                // Re-Initialize _run_seg_duration
                _run_seg_duration = (*_run_seg)->getDuration();
        }

        // Return act_seg_idx
        return(act_seg_idx);
    }
}
