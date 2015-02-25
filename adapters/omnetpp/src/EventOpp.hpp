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
 * \file EventOpp.hpp
 */

#ifndef TRES_EVENTOPP_HDR
#define TRES_EVENTOPP_HDR
#include <vector>
#include <string>
#include <cmessage.h>   // cMessage
#include <tres/NetworkEvent.hpp>

namespace tres
{
    /**
     * \addtogroup tres_omnetpp
     * @{
     */
    /**
     * \brief Object adapter for OMNeT++ events
     *
     * \note Inherits the default implementation of destructor from its base class.
     * That's fine because the adaptee object exists in OMNeT++ and its correct
     * destruction is performed by OMNeT++
     */
    class EventOpp : public NetworkEvent
    {

        friend class NetworkOpp;

    public:

        virtual std::string getName() const;

        virtual long int getTime() const;

        /**
         * \brief Get the names of all the modules that produce the event
         */
        virtual std::vector<std::string>& getEvtGeneratorModulesNames();

        /**
         * \brief Set the resolution of simulation time
         */
        virtual void SetTimeUnitExponent(int);

    protected:

        /**
         * \brief Construct from parameters
         */
        EventOpp(const int, const std::vector<std::string>&);

        /**
         * \brief Utility method to initialize an instance of this class with an
         * OMNeT++ event
         * \note It's used by tres::NetworkOpp, which is a friend of this class
         */
        virtual void setAdapteePtr(cMessage*);

    protected:

        /** The base network event representation in OMNeT++ (Adaptee) */
        cMessage *_opp_evt;

        /** Resolution of the simulation time */
        int time_unit_exponent;

        /** Names of all modules that generate the event */
        std::vector<std::string> _gen_modules;

    private:

        /**
         * \brief Prevent default construction
         */
        EventOpp();

    };
    /** @} */
}
#endif // TRES_EVENTOPP_HDR
