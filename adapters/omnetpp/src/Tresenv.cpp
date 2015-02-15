/*-----------------------------------------------------------------------------------
 *  Copyright (c) 2014,2015, ReTiS Lab., Scuola Superiore Sant'Anna.
 *
 *  This file is part of tres_omnetpp.
 *
 *  You can redistribute it and/or modify tres_omnetpp under the terms
 *  of the Academic Public License as published at
 *  http://www.omnetpp.org/intro/license.
 *
 *  tres_omnetpp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY. See the file `AP_License' for details on this
 *  and other legal matters.
 *--------------------------------------------------------------------------------- */

/**
 * \file Tresenv.cpp
 */

#include "tresdefs.h"
#include "Tresenv.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <algorithm>

#include <opp_ctype.h>
#include <enumstr.h>
#include <appreg.h>
#include <csimplemodule.h>
#include <ccomponenttype.h>
#include <cmessage.h>
#include <args.h>
#include <speedometer.h>
#include <timeutil.h>
#include <stringutil.h>
#include <cconfigoption.h>
#include <cproperties.h>
#include <cproperty.h>

Register_GlobalConfigOption(CFGID_CONFIG_NAME, "tresenv-config-name", CFG_STRING, NULL, "Specifies the name of the configuration to be run (for a value `Foo', section [Config Foo] will be used from the ini file). See also tresenv-runs-to-execute=. The -c command line option overrides this setting.")
Register_GlobalConfigOption(CFGID_RUNS_TO_EXECUTE, "tresenv-runs-to-execute", CFG_STRING, NULL, "Specifies which runs to execute from the selected configuration (see tresenv-config-name=). It accepts a comma-separated list of run numbers or run number ranges, e.g. 1,3..4,7..9. If the value is missing, Tresenv executes all runs in the selected configuration. The -r command line option overrides this setting.")
Register_GlobalConfigOptionU(CFGID_TRESENV_EXTRA_STACK, "tresenv-extra-stack", "B",  "8KiB", "Specifies the extra amount of stack that is reserved for each activity() simple module when the simulation is run under Tresenv.")
Register_GlobalConfigOption(CFGID_TRESENV_INTERACTIVE, "tresenv-interactive", CFG_BOOL,  "false", "Defines what Tresenv should do when the model contains unassigned parameters. In interactive mode, it asks the user. In non-interactive mode (which is more suitable for batch execution), Tresenv stops with an error.")
Register_GlobalConfigOption(CFGID_OUTPUT_FILE, "tresenv-output-file", CFG_FILENAME, NULL, "When a filename is specified, Tresenv redirects standard output into the given file. This is especially useful with parallel simulation. See the `fname-append-host' option as well.")
Register_PerRunConfigOption(CFGID_EXPRESS_MODE, "tresenv-express-mode", CFG_BOOL, "true", "Selects ``normal'' (debug/trace) or ``express'' mode.")
Register_PerRunConfigOption(CFGID_AUTOFLUSH, "tresenv-autoflush", CFG_BOOL, "false", "Call fflush(stdout) after each event banner or status update; affects both express and normal mode. Turning on autoflush may have a performance penalty, but it can be useful with printf-style debugging for tracking down program crashes.")
Register_PerRunConfigOption(CFGID_MODULE_MESSAGES, "tresenv-module-messages", CFG_BOOL, "true", "When tresenv-express-mode=false: turns printing module ev<< output on/off.")
Register_PerRunConfigOption(CFGID_EVENT_BANNERS, "tresenv-event-banners", CFG_BOOL, "true", "When tresenv-express-mode=false: turns printing event banners on/off.")
Register_PerRunConfigOption(CFGID_EVENT_BANNER_DETAILS, "tresenv-event-banner-details", CFG_BOOL, "false", "When tresenv-express-mode=false: print extra information after event banners.")
Register_PerRunConfigOption(CFGID_MESSAGE_TRACE, "tresenv-message-trace", CFG_BOOL, "false", "When tresenv-express-mode=false: print a line per message sending (by send(),scheduleAt(), etc) and delivery on the standard output.")
Register_PerRunConfigOptionU(CFGID_STATUS_FREQUENCY, "tresenv-status-frequency", "s", "2s", "When tresenv-express-mode=true: print status update every n seconds.")
Register_PerRunConfigOption(CFGID_PERFORMANCE_DISPLAY, "tresenv-performance-display", CFG_BOOL, "true", "When tresenv-express-mode=true: print detailed performance information. Turning it on results in a 3-line entry printed on each update, containing ev/sec, simsec/sec, ev/simsec, number of messages created/still present/currently scheduled in FES.")

Register_PerObjectConfigOption(CFGID_TRESENV_EV_OUTPUT, "tresenv-ev-output", KIND_MODULE, CFG_BOOL, "true", "When tresenv-express-mode=false: whether Tresenv should print debug messages (ev<<) from the selected modules.")


//
// Register the Tresenv user interface
//

Register_OmnetApp("Tresenv", Tresenv, 10, "command-line user interface");

//
// The following function can be used to force linking with Tresenv; specify
// -u _tresenv_lib (gcc) or /include:_tresenv_lib (vc++) in the link command.
//
extern "C" TRESENV_API void tresenv_lib() {}
// on some compilers (e.g. linux gcc 4.2) the functions are generated without _
extern "C" TRESENV_API void _tresenv_lib() {}


#define LL  INT64_PRINTF_FORMAT

static char buffer[1024];

bool Tresenv::sigint_received;


// utility function for printing elapsed time
char *timeToStr(timeval t, char *buf=NULL)
{
    static char buf2[64];
    char *b = buf ? buf : buf2;
    
    if (t.tv_sec<3600)
        sprintf(b,"%ld.%.3ds (%dm %02ds)", (long)t.tv_sec, (int)(t.tv_usec/1000), int(t.tv_sec/60L), int(t.tv_sec%60L));
    else if (t.tv_sec<86400)
        sprintf(b,"%ld.%.3ds (%dh %02dm)", (long)t.tv_sec, (int)(t.tv_usec/1000), int(t.tv_sec/3600L), int((t.tv_sec%3600L)/60L));
    else
        sprintf(b,"%ld.%.3ds (%dd %02dh)", (long)t.tv_sec, (int)(t.tv_usec/1000), int(t.tv_sec/86400L), int((t.tv_sec%86400L)/3600L));
    
    return b;
}


Tresenv::Tresenv()
{
    // Note: ctor should only contain trivial initializations, because
    // the class may be instantiated only for the purpose of calling
    // printUISpecificHelp() on it
    
    // initialize fout to stdout, then we'll replace it if redirection is
    // requested in the ini file
    fout = stdout;
    
    // init config variables that are used even before readOptions()
    opt_autoflush = true;
}

Tresenv::~Tresenv()
{
}

void Tresenv::readOptions()
{
    EnvirBase::readOptions();
    
    cConfiguration *cfg = getConfig();
    
    // note: configname and runstoexec will possibly be overwritten
    // with the -c, -r command-line options in our setup() method
    opt_configname = cfg->getAsString(CFGID_CONFIG_NAME);
    opt_runstoexec = cfg->getAsString(CFGID_RUNS_TO_EXECUTE);
    
    opt_extrastack = (size_t) cfg->getAsDouble(CFGID_TRESENV_EXTRA_STACK);
    opt_outputfile = cfg->getAsFilename(CFGID_OUTPUT_FILE).c_str();
    
    if (!opt_outputfile.empty())
    {
        processFileName(opt_outputfile);
        ::printf("Tresenv: redirecting output to file `%s'...\n",opt_outputfile.c_str());
        FILE *out = fopen(opt_outputfile.c_str(), "w");
        if (!out)
            throw cRuntimeError("Cannot open output redirection file `%s'",opt_outputfile.c_str());
        fout = out;
    }
}

void Tresenv::readPerRunOptions()
{
    EnvirBase::readPerRunOptions();
    
    cConfiguration *cfg = getConfig();
    opt_expressmode = cfg->getAsBool(CFGID_EXPRESS_MODE);
    opt_interactive = cfg->getAsBool(CFGID_TRESENV_INTERACTIVE);
    opt_autoflush = cfg->getAsBool(CFGID_AUTOFLUSH);
    opt_modulemsgs = cfg->getAsBool(CFGID_MODULE_MESSAGES);
    opt_eventbanners = cfg->getAsBool(CFGID_EVENT_BANNERS);
    opt_eventbanner_details = cfg->getAsBool(CFGID_EVENT_BANNER_DETAILS);
    opt_messagetrace = cfg->getAsBool(CFGID_MESSAGE_TRACE);
    opt_status_frequency_ms = 1000*cfg->getAsDouble(CFGID_STATUS_FREQUENCY);
    opt_perfdisplay = cfg->getAsBool(CFGID_PERFORMANCE_DISPLAY);
}

void Tresenv::askParameter(cPar *par, bool unassigned)
{
    bool success = false;
    while (!success)
    {
        cProperties *props = par->getProperties();
        cProperty *prop = props->get("prompt");
        std::string prompt = prop ? prop->getValue(cProperty::DEFAULTKEY) : "";
        std::string reply;
        
        // ask the user. note: gets() will signal "cancel" by throwing an exception
        if (!prompt.empty())
            reply = this->gets(prompt.c_str(), par->str().c_str());
        else
            // DO NOT change the "Enter parameter" string. The IDE launcher plugin matches
            // against this string for detecting user input
            reply = this->gets((std::string("Enter parameter `")+par->getFullPath()+"' ("+(unassigned?"unassigned":"ask")+"):").c_str(), par->str().c_str());
        
        try
        {
            par->parse(reply.c_str());
            success = true;
        }
        catch (std::exception& e)
        {
            ev.printfmsg("%s -- please try again.", e.what());
        }
    }
}

int Tresenv::run(int argc, char *argv[], cConfiguration *configobject)
{
    args = new ArgList();
    args->parse(argc, argv, "h?f:u:l:c:r:p:n:x:X:agGv");
    cfg = dynamic_cast<cConfigurationEx *>(configobject);
    if (!cfg)
        throw cRuntimeError("Cannot cast configuration object %s to cConfigurationEx", configobject->getClassName());
    if (simulationRequired())
    {
        if (setup())
            run();
    }
    return exitcode;
}

void Tresenv::run()
{
    // '-c' and '-r' option: configuration to activate, and run numbers to run.
    // Both command-line options take precedence over inifile settings.
    // (NOTE: inifile settings *already* got read at this point! as EnvirBase::setup()
    // invokes readOptions()).
    
    const char *configname = args->optionValue('c');
    if (configname)
        opt_configname = configname;
    if (opt_configname.empty())
        opt_configname = "General";
    
    const char *runstoexec = args->optionValue('r');
    if (runstoexec)
        opt_runstoexec = runstoexec;
    
    // if the list of runs is not given explicitly, must execute all runs
    if (opt_runstoexec.empty())
    {
        int n = cfg->getNumRunsInConfig(opt_configname.c_str());  //note: may throw exception
        if (n==0)
        {
            ev.printfmsg("Error: Configuration `%s' generates 0 runs", opt_configname.c_str());
            exitcode = 1;
            return;
        }
        else
        {
            char buf[32];
            sprintf(buf, (n==1 ? "%d" : "0..%d"), n-1);
            opt_runstoexec = buf;
        }
    }
    
    ::fflush(fout);
    
    setupnetwork_done = false;
    startrun_done = false;
    
    ::fflush(fout);
    
    cfg->activateConfig(opt_configname.c_str(), 0);
    
    const char *itervars = cfg->getVariable(CFGVAR_ITERATIONVARS2);
    if (itervars && strlen(itervars)>0)
        ::fprintf(fout, "Scenario: %s\n", itervars);
    ::fprintf(fout, "Assigned runID=%s\n", cfg->getVariable(CFGVAR_RUNID));
    
    readPerRunOptions();
    
    // find network
    cModuleType *network = resolveNetwork(opt_network_name.c_str());
    ASSERT(network);
    
    // set up network
    ::fprintf(fout, "Setting up network `%s'...\n", opt_network_name.c_str());
    ::fflush(fout);
    
    setupNetwork(network);
    setupnetwork_done = true;
    
    // prepare for simulation run
    ::fprintf(fout, "Initializing...\n");
    ::fflush(fout);
    
    disable_tracing = opt_expressmode;
    startRun();
    startrun_done = true;
    
    // run the simulation
    ::fprintf(fout, "\nRunning simulation...\n");
    ::fflush(fout);
    
}

void Tresenv::simulate()
{
    
}
void Tresenv::sim_step()
{
    // implement graceful exit when Ctrl-C is hit during simulation. We want
    // to finish the current event, then normally exit via callFinish() etc
    // so that simulation results are not lost.
    installSignalHandler();
    
    startClock();
    sigint_received = false;
    disable_tracing = true;
    
    cSimpleModule *mod;
    try
    {

        mod = simulation.selectNextModule();
        if (!mod)
            throw cTerminationException("scheduler interrupted while waiting");
        
        // execute event
        simulation.doOneEvent(mod);
        printEventBanner(mod);
        checkTimeLimits();
        if (sigint_received)
            throw cTerminationException("SIGINT or SIGTERM received, exiting");

    }
    catch (std::exception& e)
    {
        disable_tracing = false;
        stoppedWithException(e);
        displayException(e);
    }
    
    
    disable_tracing = false;
    stopClock();
    deinstallSignalHandler();
}

void Tresenv::sim_stop()
{
    disable_tracing = false;
    
    ::fflush(fout);
    simulation.callFinish();
    flushLastLine();
    
    checkFingerprint();
    
    // call endRun()
    if (startrun_done)
    {
        try
        {
            endRun();
        }
        catch (std::exception& e)
        {
            displayException(e);
        }
    }
    
    // delete network
    if (setupnetwork_done)
    {
        try
        {
            simulation.deleteNetwork();
        }
        catch (std::exception& e)
        {
            displayException(e);
        }
    }
    shutdown();
}

// note: also updates "since" (sets it to the current time) if answer is "true"
inline bool elapsed(long millis, struct timeval& since)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    bool ret = timeval_diff_usec(now, since) > 1000*millis;
    if (ret)
        since = now;
    return ret;
}

void Tresenv::printEventBanner(cSimpleModule *mod)
{
    ::fprintf(fout, "** Event #%" LL "d  T=%s%s   %s (%s, id=%d)\n",
              simulation.getEventNumber(),
              SIMTIME_STR(simulation.getSimTime()),
              progressPercentage(), // note: IDE launcher uses this to track progress
              mod->getFullPath().c_str(),
              mod->getComponentType()->getName(),
              mod->getId()
              );
    if (opt_eventbanner_details)
    {
        ::fprintf(fout, "   Elapsed: %s   Messages: created: %ld  present: %ld  in FES: %d\n",
                  timeToStr(totalElapsed()),
                  cMessage::getTotalMessageCount(),
                  cMessage::getLiveMessageCount(),
                  simulation.msgQueue.getLength());
    }
}

void Tresenv::doStatusUpdate(Speedometer& speedometer)
{
    speedometer.beginNewInterval();
    
    if (opt_perfdisplay)
    {
        ::fprintf(fout, "** Event #%" LL "d   T=%s   Elapsed: %s%s\n",
                  simulation.getEventNumber(),
                  SIMTIME_STR(simulation.getSimTime()),
                  timeToStr(totalElapsed()),
                  progressPercentage()); // note: IDE launcher uses this to track progress
        ::fprintf(fout, "     Speed:     ev/sec=%g   simsec/sec=%g   ev/simsec=%g\n",
                  speedometer.getEventsPerSec(),
                  speedometer.getSimSecPerSec(),
                  speedometer.getEventsPerSimSec());
        
        ::fprintf(fout, "     Messages:  created: %ld   present: %ld   in FES: %d\n",
                  cMessage::getTotalMessageCount(),
                  cMessage::getLiveMessageCount(),
                  simulation.msgQueue.getLength());
    }
    else
    {
        ::fprintf(fout, "** Event #%" LL "d   T=%s   Elapsed: %s%s   ev/sec=%g\n",
                  simulation.getEventNumber(),
                  SIMTIME_STR(simulation.getSimTime()),
                  timeToStr(totalElapsed()),
                  progressPercentage(), // note: IDE launcher uses this to track progress
                  speedometer.getEventsPerSec());
    }
    
    // status update is always autoflushed (not only if opt_autoflush is on)
    ::fflush(fout);
}

const char *Tresenv::progressPercentage()
{
    double simtimeRatio = -1;
    if (opt_simtimelimit!=0)
        simtimeRatio = simulation.getSimTime() / opt_simtimelimit;
    
    double cputimeRatio = -1;
    if (opt_cputimelimit!=0) {
        timeval now;
        gettimeofday(&now, NULL);
        long elapsedsecs = now.tv_sec - laststarted.tv_sec + elapsedtime.tv_sec;
        cputimeRatio = elapsedsecs / (double)opt_cputimelimit;
    }
    
    double ratio = std::max(simtimeRatio, cputimeRatio);
    if (ratio == -1)
        return "";
    else {
        static char buf[32];
        // DO NOT change the "% completed" string. The IDE launcher plugin matches
        // against this string for detecting user input
        sprintf(buf, "  %d%% completed", (int)(100*ratio));
        return buf;
    }
}

void Tresenv::componentInitBegin(cComponent *component, int stage)
{
    if (!opt_expressmode && opt_eventbanners && component->isEvEnabled())
        ::fprintf(fout, "Initializing %s %s, stage %d\n",
                  component->isModule() ? "module" : "channel", component->getFullPath().c_str(), stage);
}

void Tresenv::signalHandler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM)
        sigint_received = true;
}

void Tresenv::installSignalHandler()
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

void Tresenv::deinstallSignalHandler()
{
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
}

//-----------------------------------------------------

void Tresenv::putsmsg(const char *s)
{
    ::fprintf(fout, "\n<!> %s\n\n", s);
    ::fflush(fout);
}

void Tresenv::sputn(const char *s, int n)
{
    EnvirBase::sputn(s, n);
    
    if (disable_tracing)
        return;
    
    cComponent *ctx = simulation.getContext();
    if (!ctx || (opt_modulemsgs && ctx->isEvEnabled()) || simulation.getContextType()==CTX_FINISH)
    {
        ::fwrite(s,1,n,fout);
        if (opt_autoflush)
            ::fflush(fout);
    }
}

cEnvir& Tresenv::flush()
{
    ::fflush(fout);
    return *this;
}

std::string Tresenv::gets(const char *prompt, const char *defaultReply)
{
    if (!opt_interactive)
    {
        throw cRuntimeError("The simulation wanted to ask a question, set tresenv-interactive=true to allow it: \"%s\"", prompt);
    }
    else
    {
        ::fprintf(fout, "%s", prompt);
        if (!opp_isempty(defaultReply))
            ::fprintf(fout, "(default: %s) ", defaultReply);
        ::fflush(fout);
        
        ::fgets(buffer, 512, stdin);
        buffer[strlen(buffer)-1] = '\0'; // chop LF
        
        if (buffer[0]=='\x1b') // ESC?
            throw cRuntimeError(eCANCEL);
        
        return std::string(buffer);
    }
}

bool Tresenv::askyesno(const char *question)
{
    if (!opt_interactive)
    {
        throw cRuntimeError("Simulation needs user input in non-interactive mode (prompt text: \"%s (y/n)\")", question);
    }
    else
    {
        // should also return -1 (==CANCEL)
        for(;;)
        {
            ::fprintf(fout, "%s (y/n) ", question);
            ::fflush(fout);
            ::fgets(buffer, 512, stdin);
            buffer[strlen(buffer)-1] = '\0'; // chop LF
            if (opp_toupper(buffer[0])=='Y' && !buffer[1])
                return true;
            else if (opp_toupper(buffer[0])=='N' && !buffer[1])
                return false;
            else
                putsmsg("Please type 'y' or 'n'!\n");
        }
    }
}

bool Tresenv::idle()
{
    return sigint_received;
}

void Tresenv::moduleCreated(cModule *mod)
{
    EnvirBase::moduleCreated(mod);
    
    bool ev_enabled = getConfig()->getAsBool(mod->getFullPath().c_str(), CFGID_TRESENV_EV_OUTPUT);
    mod->setEvEnabled(ev_enabled);
}

void Tresenv::messageSent_OBSOLETE(cMessage *msg, cGate *)
{
    if (!opt_expressmode && opt_messagetrace)
    {
        ::fprintf(fout, "SENT:   %s\n", msg->info().c_str());
        if (opt_autoflush)
            ::fflush(fout);
    }
}

void Tresenv::simulationEvent(cMessage *msg)
{
    EnvirBase::simulationEvent(msg);
    
    if (!opt_expressmode && opt_messagetrace)
    {
        ::fprintf(fout, "DELIVD: %s\n", msg->info().c_str());  //TODO can go out!
        if (opt_autoflush)
            ::fflush(fout);
    }
}

void Tresenv::printUISpecificHelp()
{
    ev << "Tresenv-specific options:\n";
    ev << "  -c <configname>\n";
    ev << "                Select a given configuration for execution. With inifile-based\n";
    ev << "                configuration database, this selects the [Config <configname>]\n";
    ev << "                section; the default is the [General] section.\n";
    ev << "                See also: -r.\n";
    ev << "  -r <runs>     Execute the specified runs in the configuration selected with the\n";
    ev << "                -c option. <runs> is a comma-separated list of run numbers or\n";
    ev << "                run number ranges, for example 1,2,5-10. When not present, all\n" ;
    ev << "                runs of that configuration will be executed.\n" ;
    ev << "  -a            Print all config names and number of runs it them, and exit.\n";
    ev << "  -x <configname>\n";
    ev << "                Print the number of runs in the given configuration, and exit.\n";
    ev << "  -g, -G        Make -x verbose: print the unrolled configuration, iteration\n";
    ev << "                variables, etc. -G provides more details than -g.\n";
    ev << "  -X <configname>\n";
    ev << "                Print the fallback chain of the given configuration, and exit.\n";
}

unsigned Tresenv::getExtraStackForEnvir() const
{
    return opt_extrastack;
}

