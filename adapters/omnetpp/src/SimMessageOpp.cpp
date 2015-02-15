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
 * \file SimMessageOpp.cpp
 */

#include <iomanip>
#include <iostream>
#include <sstream>
#include "SimMessageOpp.hpp"

namespace tres
{
    SimMessageOpp::SimMessageOpp()
    {
        isMsgFromAppLevel = false;
        _opp_msg = NULL;
    }

    SimMessageOpp::SimMessageOpp(cMessage *opp_msg)
    {
        isMsgFromAppLevel = false;
        _opp_msg = opp_msg;
    }

    void SimMessageOpp::setAdapteePtr(cMessage* opp_msg)
    {
        _opp_msg = opp_msg;
    }

    void SimMessageOpp::setMsgFromAppLevelFlag(bool flagValue)
    {
        isMsgFromAppLevel = flagValue;
    }

    std::string SimMessageOpp::getUID() const
    {       
        std::string str_res;
        if (isMsgFromAppLevel)
        {
            std::stringstream ss;
            int id = ((CanAppSrv*)_opp_msg->getArrivalModule())->guessMessageID(_opp_msg->getKind());
            ss << std::setfill ('0') << std::setw(3) << std::hex << id;
            str_res = ss.str();
        }
        else
        {
            std::string str = _opp_msg->getName();
            str_res = str.substr(str.length()-3, 3);
        }
        return (str_res);
    }
}
