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
 * \file CanEventOpp.cpp
 */

#include <cstring>
#include <cmodule.h>
#include <ccomponenttype.h>
#include "CanEventOpp.hpp"

namespace tres
{
    EventOpp* CanEventOpp::createInstance(std::vector<std::string>& par)
    {
        // Get the time resolution
        double time_unit_exponent = atof((par[0]).c_str());
        return new CanEventOpp(time_unit_exponent);
    }

    CanEventOpp::CanEventOpp(const int time_unit_exponent) :
        EventOpp(time_unit_exponent, {"CanAppSrv", "CanController", "CanBus", "CanAppCli"})
    {
    }

    bool CanEventOpp::isGeneratedByAppLevelTraffic()
    {
        bool isMsgFromApp = (!strcmp(_opp_evt->getArrivalModule()->getComponentType()->getName(), "CanAppSrv")) && (_opp_evt->isSelfMessage());
        if (isMsgFromApp)
            _gen_msg.setMsgFromAppLevelFlag(true);
        bool isMsgToApp = !(strcmp(_opp_evt->getArrivalModule()->getComponentType()->getName(), "CanAppCli"));
        bool isMsgToApp1 = !(strcmp(_opp_evt->getArrivalModule()->getComponentType()->getName(), "CanAppSrv")) && !(_opp_evt->isSelfMessage());
        if (isMsgToApp)
            _gen_msg.setMsgFromAppLevelFlag(false);
        if (isMsgToApp1)
            _gen_msg.setMsgFromAppLevelFlag(false);
        return(isMsgFromApp || isMsgToApp || isMsgToApp1);
    }

    SimMessage* CanEventOpp::getGeneratorMessage()
    {
        _gen_msg.setAdapteePtr( _opp_evt );
        return &_gen_msg;
    }
}
