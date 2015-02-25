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
 * \file NetworkOpp.cpp
 */

#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <opp_ctype.h>
#include <args.h>
#include <distrib.h>
#include <cconfigoption.h>
#include <inifilereader.h>
#include <sectionbasedconfig.h>
#include <appreg.h>
#include <cmodule.h>
#include <fsutils.h>
#include <fnamelisttokenizer.h>
#include <stringutil.h>
#include <fileutil.h>
#include <intxtypes.h>
#include <speedometer.h>
#include <fileoutvectormgr.h>
#include <fileoutscalarmgr.h>
#include <filesnapshotmgr.h>
#include <indexedfileoutvectormgr.h>
#include <akaroarng.h>
#include <akoutvectormgr.h>
#include <matchableobject.h>
#include <csimulation.h>
#include <cscheduler.h>
#include <ccomponenttype.h>
#include <simtime.h>

#include <tres/Factory.hpp>
#include <tres/ParseUtils.hpp>
#include "NetworkOpp.hpp"

Register_GlobalConfigOption(CFGID_LOAD_LIBS, "load-libs", CFG_FILENAMES, "", "A space-separated list of dynamic libraries to be loaded on startup. The libraries should be given without the `.dll' or `.so' suffix -- that will be automatically appended.");
Register_GlobalConfigOption(CFGID_CONFIGURATION_CLASS, "configuration-class", CFG_STRING, "", "Part of the Envir plugin mechanism: selects the class from which all configuration information will be obtained. This option lets you replace omnetpp.ini with some other implementation, e.g. database input. The simulation program still has to bootstrap from an omnetpp.ini (which contains the configuration-class setting). The class should implement the cConfigurationEx interface.");
Register_GlobalConfigOption(CFGID_USER_INTERFACE, "user-interface", CFG_STRING, "", "Selects the user interface to be started. Possible values are Cmdenv and Tkenv. This option is normally left empty, as it is more convenient to specify the user interface via a command-line option or the IDE's Run and Debug dialogs. New user interfaces can be defined by subclassing cRunnableEnvir.");

namespace tres
{
    static const char *convert(const std::string & s)
    {
        return s.c_str();
    }

    //
    // _OppEventHandler helper class
    // =============================
    //
    NetworkOpp::_OppEventHandler::~_OppEventHandler()
    {
        for (std::vector<EventOpp *>::iterator it = _evt_space.begin();
                it != _evt_space.end();
                    ++it)
            delete *it;
    }

    void NetworkOpp::_OppEventHandler::initialize(const std::vector<std::string>& evt_types, const int time_unit_exponent)
    {
        std::vector<std::string> evth_params;

        // Insert the time resolution in the params list
        std::stringstream ss;       // A convenience stringstream
        ss << time_unit_exponent;
        evth_params.push_back(ss.str());
        ss.str(std::string());      // Flush the ss

        for (std::vector<std::string>::const_iterator it = evt_types.begin();
                it != evt_types.end();
                    ++it)
        {
            // Create the concrete instance of EventOpp (according to the given event type)
            std::unique_ptr<EventOpp> _tmp_evt = Factory<EventOpp>::instance()
                                                        .create(*it, evth_params);

            // Register all the modules that produce such an event in the _evt_space_map
            std::vector<std::string> _gen_modules = _tmp_evt->getEvtGeneratorModulesNames();
            for (std::vector<std::string>::iterator jt = _gen_modules.begin();
                    jt != _gen_modules.end();
                        ++jt)
                _evt_space_map[*jt] = it - evt_types.begin(); // Yes, the right-hand side is CORRECT!

            // Push actually back the created event in the _evt_space vector
            _evt_space.push_back(_tmp_evt.release());
        }

        // Initialize the pointer to the next event in the Opp engine (NULL)
        _next_event = NULL;
    }

    void NetworkOpp::_OppEventHandler::readNextEventFromOppEngine(cMessage *msg)
    {
        // Use msg and the _evt_space_map to access the space dedicated
        // to the type of the next Opp event (from the Opp engine)
        std::string _msg_info = msg->getArrivalModule()->getComponentType()->getName();
        std::map<std::string, int>::iterator it = _evt_space_map.find(_msg_info);
        if (it == _evt_space_map.end()) throw cRuntimeError(" Could not find the event generator module named '%s'\n", _msg_info.c_str());
        _next_event = _evt_space[it->second];

        // Then forward the message from Opp to the right (concrete) instance of EventOpp
        _next_event->setAdapteePtr(msg);
    }

    EventOpp* NetworkOpp::_OppEventHandler::getEventOppInstancePtr()
    {
        return _next_event;
    }

    //
    // NetworkOpp class
    // =================
    //
    Network* NetworkOpp::createInstance(std::vector<std::string>& par)
    {
        // The input information for this function is _guaranteed_ to have the following form:
        //  - the number of messages in the message-set (#msgs)     - std::string (1)      <-- vector.begin()
        //  - the message-set description                           - std::string (#msgs)
        //      - "message_type;message_UID;"
        //  - the number of items describing the network (#ndescr)  - std::string (1)
        //  - the network description                               - std::string (#ndescr)
        //      - "description_type;description_file_paths"
        //  - the time resolution                                   - std::string (1)
        //                                                                             <-- vector.end()
        std::vector<std::string>::iterator it = par.begin();  // Convenience iterator

        // Get the number of messages
        int num_msgs = atoi((*(it++)).c_str());

        // Extract all the message types involved in the current simulation
        // For each message get its unique identifier
        std::set<std::string> _tmpset;
        std::vector<std::string> msg_types;
        std::vector<std::string> msg_uids;
        for (int i = 0; i < num_msgs; i++)
        {
            // Tokenize the message description
            using namespace tres_parse_utils;
            std::vector<std::string> tokens = split_instr(*(it++));
            std::vector<std::string>::iterator jt = tokens.begin();

            // Assign message types (msg_types must contain unique values)
            if(_tmpset.insert(*jt).second)
                msg_types.push_back(*jt);
            // Assign unique identifier
            msg_uids.push_back(*(++jt));
        }

        // Get the number of network description items
        int num_ndescr = atoi((*(it++)).c_str());

        // Extract all the description types
        // For each description get the file path
        std::vector<std::string> opp_params = {"tres_run"};
        for (int i = 0; i < num_ndescr; i++)
        {
            // Tokenize the message description
            using namespace tres_parse_utils;
            std::vector<std::string> tokens = split_instr(*(it++));
            std::vector<std::string>::iterator jt = tokens.begin();

            // Get the description information
            if (*jt == "nedpath")
                opp_params.push_back("-n");
            else if (*jt == "inifile")
                opp_params.push_back("-f");
            else if (*jt == "configname")
                opp_params.push_back("-c");
            opp_params.push_back(*(++jt));
        }

        // Additional libraries
        opp_params.push_back("-l");
        opp_params.push_back(*(it++));

        // Finally, get the time resolution
        double time_resolution = atof((*(it++)).c_str());
        int time_resolution_exponent = log10 (time_resolution);

        return new NetworkOpp(opp_params, msg_types, msg_uids, time_resolution_exponent);
    }

    NetworkOpp::NetworkOpp(const std::vector<std::string> &params, const std::vector<std::string> &msg_types, const std::vector<std::string> &msg_uids, const int time_resolution_exponent)
    {
        // Initialize the time resolution
        time_unit_exponent = -time_resolution_exponent;

        // Initialize the event handler with the types of network messages
        // and the time resolution info
        _evt_handler.initialize(msg_types, time_unit_exponent);

        // Read the UID of messages for the current simulation
        for (std::vector<std::string>::const_iterator it = msg_uids.begin();
                it != msg_uids.end();
                    ++it)
            _msg_port_map[*it] = it - msg_uids.begin();

        // initialize the gateway to NULL
        gateway = NULL;

        // Convert the arguments to be given to the Opp engine from std::string to char **
        std::vector<const char *> argv(params.size());
        std::transform(params.begin(), params.end(), argv.begin(), convert);

        // Start the Setup of the Opp engine/app/... for this simulation
        //
        int exitcode = 0;
        try
        {
            // Construct global lists
            CodeFragments::executeAll(CodeFragments::STARTUP);

            // ArgList
            ArgList args;
            args.parse(argv.size(), const_cast<char**>(argv.data()), "h?f:u:l:c:r:p:n:x:X:agGv");

            // First, load the ini file. It might contain the name of the user interface
            // to instantiate.
            const char *fname = args.optionValue('f',0);  // 1st '-f filename' option
            if (!fname) fname = args.argument(0);   // first argument
            if (!fname) fname = "omnetpp.ini";   // or default filename
            
            // When -h or -v is specified, be forgiving about nonexistent omnetpp.ini
            InifileReader *inifile = new InifileReader();
            if ((!args.optionGiven('v') && !args.optionGiven('h')) || fileExists(fname))
                inifile->readFile(fname);
            
            // Process additional '-f filename' options or arguments if there are any
            for (int k=1; (fname=args.optionValue('f',k))!=NULL; k++)
                inifile->readFile(fname);
            for (int k=(args.optionValue('f',0) ? 0 : 1); (fname=args.argument(k))!=NULL; k++)
                inifile->readFile(fname);
            
            // Activate [General] section so that we can read global settings from it
            bootconfig = new SectionBasedConfiguration();
            bootconfig->setConfigurationReader(inifile);
            bootconfig->setCommandLineConfigOptions(args.getLongOptions(), getWorkingDir().c_str());
            bootconfig->activateConfig("General", 0);
            
            // Load all libraries specified on the command line ('-l' options),
            // and in the configuration [General]/load-libs=.
            // (The user interface library also might be among them.)
            const char *libname;
            for (int k=0; (libname=args.optionValue('l',k))!=NULL; k++)
                loadExtensionLibrary(libname);
            std::vector<std::string> libs = bootconfig->getAsFilenames(CFGID_LOAD_LIBS);
            for (int k=0; k<(int)libs.size(); k++)
                loadExtensionLibrary(libs[k].c_str());
            
            // Create custom configuration object, if needed.
            std::string configclass = bootconfig->getAsString(CFGID_CONFIGURATION_CLASS);
            if (configclass.empty())
            {
                configobject = bootconfig;
            }
            
            // Validate the configuration, but make sure we don't report cmdenv-* keys
            // as errors if Cmdenv is absent; same for Tkenv.
            std::string ignorablekeys;
            if (omnetapps.getInstance()->lookup("Cmdenv")==NULL)
                ignorablekeys += " cmdenv-*";
            if (omnetapps.getInstance()->lookup("Tkenv")==NULL)
                ignorablekeys += " tkenv-*";
            configobject->validate(ignorablekeys.c_str());
            
            // Choose and set up user interface (EnvirBase subclass). Everything else
            // will be done by the user interface class.
            
            // Was it specified explicitly which one to use?
            std::string appname = opp_nulltoempty(args.optionValue('u',0));  // 1st '-u name' option
            if (appname.empty())
                appname = configobject->getAsString(CFGID_USER_INTERFACE);
            cOmnetAppRegistration *appreg = NULL;
            if (!appname.empty())
            {
                // Look up specified user interface
                appreg = static_cast<cOmnetAppRegistration *>(omnetapps.getInstance()->lookup(appname.c_str()));
                if (!appreg)
                {
                    ::printf("\n"
                             "User interface '%s' not found (not linked in or loaded dynamically).\n"
                             "Available ones are:\n", appname.c_str());
                    cRegistrationList *a = omnetapps.getInstance();
                    for (int i=0; i<a->size(); i++)
                        ::printf("  %s : %s\n", a->get(i)->getName(), a->get(i)->info().c_str());
                    
                    throw cRuntimeError("Could not start user interface '%s'", appname.c_str());
                }
            }
            else
            {
                // User interface not explicitly selected: pick one from what we have
                appreg = cOmnetAppRegistration::chooseBest();
                if (!appreg)
                    throw cRuntimeError("No user interface (Cmdenv, Tkenv, etc.) found");
            }
            
            // Create interface object.
            ::printf("Setting up %s...\n", appreg->getName());
            app = new Tresenv();
        }
        catch (std::exception& e)
        {
            ::fprintf(stderr, "\n<!> Error during startup: %s.\n", e.what());
            if (app)
            {
                delete app;
                app = NULL;
            }
            else
            {
                // normally, this is deleted by app
                delete bootconfig;
            }
        }

        // Simulate the execution of Opp's "Run" (actually a modified version of it)
        try
        {
            if (app)
            {
                simulationobject = new cSimulation("simulation", app);
                cSimulation::setActiveSimulation(simulationobject);
                app->run(argv.size(), const_cast<char**>(argv.data()), configobject);
            }
            else
            {
                exitcode = 1;
            }
        }
        catch (std::exception& e)
        {
            ::fprintf(stderr, "\n<!> %s.\n", e.what());
            exitcode = 1;
        }

    }

    NetworkOpp::~NetworkOpp() noexcept(true)
    {
        // shutdown
        app->sim_stop();
        cSimulation::setActiveSimulation(NULL);
        delete simulationobject;  // will delete app as well
        //CodeFragments::executeAll(CodeFragments::SHUTDOWN);
        delete gateway;
    }

    void NetworkOpp::processNextEvent()
    {
        app->sim_step();
    }

    int NetworkOpp::getTimeOfNextEvent()
    {
        return (simulationobject->guessNextSimtime().inUnit(time_unit_exponent));
    }

    int NetworkOpp::getNextWakeUpTime()
    {
        return (getTimeOfNextEvent());
    }

    NetworkEvent* NetworkOpp::getNextEvent()
    {
        _evt_handler.readNextEventFromOppEngine(simulationobject->getScheduler()->getNextEvent());
        return _evt_handler.getEventOppInstancePtr();
    }

    extern "C"
    {
        Network* create_object(std::vector<std::string>& params)
        {
            return NetworkOpp::createInstance(params);
        }

        void destroy_object(Network* p)
        {
            delete p;
        }
    }   
}
