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
 * \file RandExecSegment.cpp
 */

#include <cctype>   // isdigit
#include <cmath>
#include <cstdlib>  // atof
#include <tres/Factory.hpp>
#include <tres/ParseUtils.hpp>
#include "FixedExecSegment.hpp"

namespace tres
{
    RandExecSegment::RandExecSegment(unique_ptr<RandomVar> &c) :
        cost(std::move(c))
    {
    }

    RandExecSegment::RandExecSegment(RandomVar *c) :
        cost(c)
    {
    }

    Segment *RandExecSegment::createInstance(std::vector<std::string>& par)
    {
        Segment *temp = NULL;
        if (std::isdigit((par[0].c_str())[0]))
        {
            temp = new FixedExecSegment(atof(par[0].c_str()));
        }
        else
        {
            using namespace tres_parse_utils;
            std::string token = get_token(par[0]);
            std::string p = get_param(par[0]);
            std::vector<std::string> parms = split_param(p);
            unique_ptr<RandomVar> var(Factory<RandomVar>::instance().create(token,parms));
            if (!var.get()) throw ParseExc("RandExecSegment", par[0]);
            temp = new RandExecSegment(var);
        }
        return temp;
    }

    double RandExecSegment::getWCET() const
    {
        return cost->getMaximum();
    }

    double RandExecSegment::getDuration() const
    {
        return cost->get();
    }
}
