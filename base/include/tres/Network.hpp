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
 * \file Network.hpp
 */

#ifndef TRES_NETWORK_HDR
#define TRES_NETWORK_HDR
#include <map>
#include <vector>
#include <unordered_set>
#include <tres/NetworkEvent.hpp>

namespace tres
{
    /**
     * \addtogroup tres_base_network_abstractions
     * @{
     */
    /**
     * \brief An \em abstract class for COM networks
     *
     * Provide infrastructure code and behavior to enable COM network simulation
     * by means of 3rd-party simulators.
     *
     * Concrete implementors derive from this abstract class, override the pure virtual
     * methods and complete this behavior with some custom (simulator-specific) code.
     */
    class Network
    {        

    public:

        typedef std::string BASE_KEY_TYPE;

        /**
         * \brief The virtual destructor
         */
        virtual ~Network() = default;

        /**
         * \brief Process the next simulation step
         */
        virtual void processNextEvent() = 0;

        /**
         * \brief Return the next event in the COM network simulation engine queue
         * to be processed
         */
        virtual NetworkEvent* getNextEvent() = 0;

        /**
         * \brief Return the time of the next event in the event queue of the network
         * simulator
         */
        virtual int getTimeOfNextEvent() = 0;

        /**
         * \brief Return the time at which the execution of the tres::Network instance
         * will be triggered again
         */
        virtual int getNextWakeUpTime() = 0;

        /**
         * \brief Add a message to the trigger queue
         */
        void addMessageToTriggerQueue(SimMessage *);

        /**
         * \brief Return the a vector of block's ports to trigger in S/R implementation
         *
         * \note This function has side-effects, since the list of messages scheduled for
         * a new send/receive operation (\ref _msg_to_trigger, see below) gets cleared
         * once the port indices are computed
         */
        std::vector<int> getPortsToTrigger();

    protected:

        /** Map of the message-uid and (S/R)block-port correspondence */
        std::map<std::string, int> _msg_port_map;

        /** List of messages scheduled for a new send/receive operation */
        std::unordered_set<std::string> _msg_to_trigger;

    };
    /** @} */
}
#endif // TRES_NETWORK_HDR
