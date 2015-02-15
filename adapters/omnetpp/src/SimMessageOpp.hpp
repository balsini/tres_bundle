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
 * \file SimMessageOpp.hpp
 * \todo Implement specialized versions of this class for specific event types
 * e.g, CanSimMessageOpp, TCPSocketSimMessageOpp, etc.
 */

#ifndef TRES_SIMMESSAGEOPP_HDR
#define TRES_SIMMESSAGEOPP_HDR
#include <string>
#include <cmessage.h>
#include <can/CanAppSrv.h>
#include <tres/SimMessage.hpp>

namespace tres
{
    /**
     * \addtogroup tres_omnetpp
     * @{
     */
    /**
     * \brief Object adapter for OMNeT++-based network messages
     */
    class SimMessageOpp : public SimMessage
    {

        friend class CanEventOpp;

    public:

        /**
         * \brief Default constructor
         */
        SimMessageOpp();

        /**
         * \brief Construct from external parameters
         */
        SimMessageOpp(cMessage*);

        /**
         * \brief The virtual destructor
         */
        virtual ~SimMessageOpp() = default;

        /**
         * \brief Set the value of an helper flag to help detecting whether a message
         * flows from the Application layer down through the ISO/OSI network stack
         * (see \ref isMsgFromAppLevel)
         */
        virtual void setMsgFromAppLevelFlag(bool);

        /**
         * \brief Get the unique ID of this task
         */
        virtual std::string getUID() const;

    protected:

        /**
         * \brief Utility method to initialize an instance of this class with an
         * OMNeT++ message
         * \note It's used by tres::CanEventOpp, which is a friend of this class
         */
        virtual void setAdapteePtr(cMessage*);

    protected:

        /** Helper flag to detect if a message is flowing from the Application layer down
         * through the ISO/OSI network stack
         * \note This is useful to implement the getUID() method because, in the CAN
         * bus module of OMNeT++, the string representing the message ID (e.g., 'XYZ')
         * is different depending whether the message is flowing down from the App layer
         * ('0xXYZ') or up to the App layer ('XYZ', i.e, without the '0x' prefix)*/
        bool isMsgFromAppLevel;

        /** The base network message representation in OMNeT++ */
        cMessage *_opp_msg;

    };
    /** @} */    
}
#endif // TRES_SIMMESSAGEOPP_HDR
