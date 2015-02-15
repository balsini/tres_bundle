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
 * \file regvar.cpp
 *
 * Register random variables into the tres::Factory registry. Random variables
 * can also be registered with two (or more) different names.
 *
 * \warning Users should never access objects of this file; it is used just
 * for initialization of the objects needed for the abstract factory that
 * creates RandomVars.
 */

#include <cstdlib>
#include <tres/Factory.hpp>
#include <tres/ParseUtils.hpp>
#include "GenericVar.hpp"

using namespace std;

namespace tres
{
    static registerInFactory<RandomVar,
                             DeltaVar,
                             RandomVar::BASE_KEY_TYPE>
    registerDelta("delta");

    static registerInFactory<RandomVar,
                             UniformVar,
                             RandomVar::BASE_KEY_TYPE>
    registerUnif1("unif");

    static registerInFactory<RandomVar,
                             UniformVar,
                             RandomVar::BASE_KEY_TYPE>
    registerUnif2("uniform");

    static registerInFactory<RandomVar,
                             NormalVar,
                             RandomVar::BASE_KEY_TYPE>
    registerNormal1("normal");

    static registerInFactory<RandomVar,
                             NormalVar,
                             RandomVar::BASE_KEY_TYPE>
    registerNormal2("gauss");

    static registerInFactory<RandomVar,
                             ExponentialVar,
                             RandomVar::BASE_KEY_TYPE>
    registerExp1("exp");

    static registerInFactory<RandomVar,
                             ExponentialVar,
                             RandomVar::BASE_KEY_TYPE>
    registerExp2("exponential");

    static registerInFactory<RandomVar,
                             ParetoVar,
                             RandomVar::BASE_KEY_TYPE>
    registerPareto("pareto");

    static registerInFactory<RandomVar,
                             PoissonVar,
                             RandomVar::BASE_KEY_TYPE>
    registerPoisson("poisson");

    static registerInFactory<RandomVar,
                             DetVar,
                             RandomVar::BASE_KEY_TYPE>
    registerDet("trace");

    static registerInFactory<RandomVar,
                             GenericVar,
                             RandomVar::BASE_KEY_TYPE>
    registerGeneric("PDF");

    static registerInFactory<RandomVar,
                             DistVar,
                             RandomVar::BASE_KEY_TYPE>
    registerDist("dist");
}
