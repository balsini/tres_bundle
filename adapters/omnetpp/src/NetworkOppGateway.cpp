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
 * \file NetworkOppGateway.cpp
 */

#include <tres_omnetpp/NetworkOppGateway.hpp>
#include "NetworkOpp.hpp"

#include <dlfcn.h>
#include <cstdlib>

namespace tres
{
    Network* NetworkOppGateway::createInstance(std::vector<std::string>& par)
    {
        std::string lib_net_adapter_name = std::string( std::getenv("TRES_OMNETPP_LIB") )
                                                + "/libtres_omnetpp.so";
        void* handle = dlopen(lib_net_adapter_name.c_str(), RTLD_LAZY);

        // Declaration of Creator/Destructor functions
        Network* (*create)(std::vector<std::string>&);
        void (*destroy)(Network*);

        // Initialization
        create = (Network* (*)(std::vector<std::string>&))dlsym(handle, "create_object");
        destroy = (void (*)(Network*))dlsym(handle, "destroy_object");

        // Construct the object
        Network* ns = (Network*)create(par);
        ((NetworkOpp *) ns)->gateway = new NetworkOppGateway(handle);
        return ns;
    }

    NetworkOppGateway::NetworkOppGateway(void *lib_handle) : handle(lib_handle)
    {
    }

    NetworkOppGateway::~NetworkOppGateway()
    {
        dlclose(handle);
    }
}
