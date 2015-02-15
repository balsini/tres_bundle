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
 * \file EventOpp.cpp
 */

#include <cstring>
#include <cmodule.h>
#include <ccomponenttype.h>
#include "EventOpp.hpp"

namespace tres
{
    EventOpp::EventOpp(const int tue, const std::vector<std::string>& genms) :
        _opp_evt(NULL), time_unit_exponent(tue), _gen_modules(genms)
    {
    }

    void EventOpp::setAdapteePtr(cMessage *opp_evt)
    {
        _opp_evt = opp_evt;
    }

    std::vector<std::string>& EventOpp::getEvtGeneratorModulesNames()
    {
        return _gen_modules;
    }

    void EventOpp::SetTimeUnitExponent(int exponent)
    {
        time_unit_exponent = exponent;
    }

    std::string EventOpp::getName() const
    {
        return (_opp_evt->getName());
    }

    long int EventOpp::getTime() const
    {
        return (_opp_evt->getArrivalTime().inUnit(time_unit_exponent));
    }
}
