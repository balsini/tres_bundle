/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2014, ReTiS Lab., Scuola Superiore Sant'Anna.
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
 * \file SimMessage.hpp
 */

#ifndef TRES_SIMMESSAGE_HDR
#define TRES_SIMMESSAGE_HDR
#include <string>

namespace tres
{
    /**
     * \addtogroup tres_base_network_abstractions
     * @{
     */
    /**
     * \brief The base class for simulated COM network messages
     *
     * Provide infrastructure code related to the simulation of send/receive operations
     * of network communication messages by \em concrete \em implementations
     * of tres::Network
     */
    class SimMessage
    {

    public:

        typedef std::string BASE_KEY_TYPE;

        /**
         * \brief The virtual destructor
         */
        virtual ~SimMessage() = default;

        /**
         * \brief Get the unique ID of this network message
         */
        virtual std::string getUID() const = 0;

    };
    /** @} */
}
#endif // TRES_SIMMESSAGE_HDR
