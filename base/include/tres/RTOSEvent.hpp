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
 * \file RTOSEvent.hpp
 */

#ifndef TRES_RTOSEVENT_HDR
#define TRES_RTOSEVENT_HDR
#include <tres/SimTask.hpp>

namespace tres
{
    /**
     * \addtogroup tres_base_rtos_abstractions
     * @{
     */
    /**
     * \brief Simple collection of enumerators related to RT task execution and scheduling events
     */
    enum class RTOSEventType
    {
        END_INSTRUCTION,
        END_TASK,
        PREEMPTION,
        OTHER
    };

    /**
     * \brief The base class for every RTOS event
     *
     * Provide infrastructure code related to RTOS events, enabling RTOS scheduling
     * simulation by \em concrete \em implementations of tres::Kernel
     */
    class RTOSEvent
    {

    public:

        typedef std::string BASE_KEY_TYPE;

        /**
         * \brief The virtual destructor
         */
        virtual ~RTOSEvent() = default;

        /**
         * \brief Get event name
         */
        virtual std::string getName() const = 0;

        /**
         * \brief Get time of event occurrence
         */
        virtual long int getTime() const = 0;

        /**
         * Get the event type
         */
        virtual RTOSEventType getType() const = 0;

        /**
         * Get a pointer to the task which has generated this event
         */
        virtual SimTask* getGeneratorTask() = 0;
    };
    /** @} */
}
#endif // TRES_RTOSEVENT_HDR
