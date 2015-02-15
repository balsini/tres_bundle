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
 * \file Task.hpp
 */

#ifndef TRES_TASK_HDR
#define TRES_TASK_HDR
#include <iostream>
#include <string>
#include <vector>
#include "../../src/Segment.hpp"

namespace tres
{
    /**
     * \addtogroup tres_base_rtos
     * @{
     */
    /**
     * \brief A class that models a cyclic task.
     *
     * A cyclic task is a task that is cyclically activated by a timer (for example
     * a periodic task) or by an external event (sporadic or aperiodic task).
     */
    class Task
    {

    public:

        /**
         * \brief The virtual destructor
         */
        virtual ~Task() = default;

        /**
         * \brief Construct a Task object and assign it a sequence of pseudo instructions
         */
        Task(const std::vector<std::string>&);

        /**
         * \brief Get the number of pseudo instructions representing the task body
         */
        int getNumberOfSegments() const;

        /**
         * \brief Go to the next instruction and return the index
         * of the processed instruction
         */
        int processSegment();

        /**
         * \brief Get the total computation time of the current instruction
         */
        double getSegmentDuration() const;

    protected:

        /** Sequence of segments representing the task body */
        std::vector<Segment*> _segment_q;

        /** Pointer to the segment which is being executed */
        std::vector<Segment*>::iterator _run_seg;

        /** Total computation time of the current instruction */
        double _run_seg_duration;

    };
    /** @} */
}
#endif // TRES_TASK_HDR
