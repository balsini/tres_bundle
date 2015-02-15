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
 * \file CanEventOpp.hpp
 */

#ifndef TRES_CANEVENTOPP_HDR
#define TRES_CANEVENTOPP_HDR
#include "EventOpp.hpp"
#include "SimMessageOpp.hpp"

namespace tres
{
    /**
     * \addtogroup tres_omnetpp
     * @{
     */
    /**
     * \brief Object adapter for events in CAN-based network simulated with OMNeT++
     */
    class CanEventOpp : public EventOpp
    {

    public:

        /**
         * \brief Creator function used  for object construction
         * according to the Factory Method pattern
         */
        static EventOpp* createInstance(std::vector<std::string>&);

        /**
         * \brief Construct from parameters
         */
        CanEventOpp(const int);

        /**
         * \brief The virtual destructor
         */
        virtual ~CanEventOpp() = default;

        virtual bool isGeneratedByAppLevelTraffic();

        virtual SimMessage* getGeneratorMessage();

    protected:

        /** OMNeT++ message which has generated the event */
        SimMessageOpp _gen_msg;

    };
    /** @} */
}
#endif // TRES_CANEVENTOPP_HDR
