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
 * \file NetworkOpp.hpp
 */

#ifndef TRES_NETWORKOPP_HDR
#define TRES_NETWORKOPP_HDR
#include <vector>
#include <string>
#include <map>
#include <envirdefs.h>
#include <csimulation.h>
#include <sectionbasedconfig.h>
#include <cenvir.h>
#include <simtime_t.h>
#include <tres/Network.hpp>
#include <tres_omnetpp/NetworkOppGateway.hpp>
#include "Tresenv.hpp"
#include "EventOpp.hpp"   // cMessage

namespace tres
{
    /**
     * \addtogroup tres_omnetpp
     * @{
     */
    /**
     * \brief Object adapter for OMNeT++ networks
     */
    class NetworkOpp : public Network
    {

        friend class NetworkOppGateway;

        /**
         * \brief Helper class to manage the execution of next event coming from OMNeT++
         *
         * \note The helper is intended for internal use only. Its methods are executed
         * by NetworkOpp in the following order
         *  - initialize()
         *  - readNextEventFromOppEngine()
         *  - getEventOppInstancePtr()
         *
         * The first one is executed inside NetworkOpp constructor; the other two
         * methods are executed inside NetworkOpp::getNextEvent()
         */
        class _OppEventHandler
        {

        public:

            /**
             * \brief The destructor
             */
            ~_OppEventHandler();

            /**
             * \brief Initialize the helper class
             *
             * Create a space for each event type and initialize the current event to NULL;
             * for each event, register in the \ref _evt_space_map all the modules that
             * produce it
             */
            void initialize(const std::vector<std::string>&, const int);

            /**
             * \brief Use the helper class
             *
             * Detect the type of the next event coming from OMNeT++ and fill the
             * dedicated space with all the info of that event
             */
            void readNextEventFromOppEngine(cMessage *);

            /**
             * \brief Convenience methods to get a pointer to the space dedicated to
             * the next event in the OMNeT++ engine
             */
            EventOpp* getEventOppInstancePtr();

        private:
            /** A space where the events for the current simulation are
             * (pre-)allocated at time of initialization */
            std::vector<EventOpp *> _evt_space;

            /** A map between the type of an event (CAN, TCPSocket, ...)
             * and its location index inside the \ref _evt_space */
            std::map<std::string, int> _evt_space_map;

            /** A pointer to the incoming event from the OMNeT++ engine */
            EventOpp *_next_event;

        };

    public:

        /**
         * \brief Creator function used for object construction
         * according to the Factory Method pattern
         */
        static Network* createInstance(std::vector<std::string>&);

        /**
         * \brief The destructor
         */
        virtual ~NetworkOpp();

        virtual void processNextEvent();

        /**
         * \brief Return the next event from the OMNeT++ event queue
         */
        virtual NetworkEvent* getNextEvent();

        /**
         * \brief Return the time of the next event in the OMNeT++ event queue
         */
        virtual int getTimeOfNextEvent();

        /**
         * \brief Return the time at which OMNeT++ will drive the next Network execution
         */
        virtual int getNextWakeUpTime();

    protected:

        /**
         * \name Bunch of entities related to OMNeT++ needed to embed the OMNeT++
         * simulation engine within a NetworkOpp instance
         * @{
         */
        cSimulation *simulationobject;
        SectionBasedConfiguration *bootconfig;
        cConfigurationEx *configobject;
        cStaticFlag dummy;
        /**
         * @}
         */

        /** OMNeT++-specific command interface for use with T-Res */
        Tresenv *app;

        /** OMNeT++-specific gateway to ease the use of NetworkOpp instances
         * in S/R simulation systems (e.g., Simulink) */
        NetworkOppGateway *gateway;

        /** Handler to the next OMNeT++ event */
        _OppEventHandler _evt_handler;

        /** Resolution of the simulation time */
        int time_unit_exponent;

    private:

        /**
         * \brief Prevent default construction
         */
        NetworkOpp();

        /**
         * \brief Construct from external parameters
         */
        NetworkOpp(const std::vector<std::string>&, const std::vector<std::string>&, const std::vector<std::string>&, const int);

    };
    /** @} */

    typedef Network* create_t;
    typedef void destroy_t(Network*);
}
#endif // TRES_NETWORKOPP_HDR
