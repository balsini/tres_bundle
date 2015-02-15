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
 * \file Tresenv.hpp
 */

#ifndef TRES_TRESENV_HDR
#define TRES_TRESENV_HDR
#include <sstream>
#include <csimulation.h>
#include <envirbase.h>

class Speedometer;

/**
 * \addtogroup tres_omnetpp
 * @{
 */
/**
 * \brief An environment to embed the OMNeT++ simulation engine in T-Res
 */
class Tresenv : public EnvirBase
{

protected:
    // new simulation options:
    opp_string opt_configname;
    opp_string opt_runstoexec;
    size_t opt_extrastack;
    opp_string opt_outputfile;
    
    bool opt_expressmode;
    bool opt_interactive;
    bool opt_autoflush; // all modes
    bool opt_modulemsgs;  // if normal mode
    bool opt_eventbanners; // if normal mode
    bool opt_eventbanner_details; // if normal mode
    bool opt_messagetrace; // if normal mode
    long opt_status_frequency_ms; // if express mode
    bool opt_perfdisplay; // if express mode
    bool setupnetwork_done;
    bool startrun_done;
    
    // set to true on SIGINT/SIGTERM signals
    static bool sigint_received;
    
    // stream to write output to
    FILE *fout;
    
protected:
    virtual void sputn(const char *s, int n);
    virtual void putsmsg(const char *s);
    virtual bool askyesno(const char *question);
    virtual void printEventBanner(cSimpleModule *mod);
    virtual void doStatusUpdate(Speedometer& speedometer);
    
public:
    Tresenv();
    virtual ~Tresenv();
    
    // redefined virtual funcs:
    virtual void componentInitBegin(cComponent *component, int stage);
    virtual void moduleCreated(cModule *newmodule);
    virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate);
    virtual void simulationEvent(cMessage *msg);
    virtual bool isGUI() const {return false;}
    virtual cEnvir& flush();
    virtual std::string gets(const char *prompt, const char *defaultReply);
    virtual bool idle();
    virtual unsigned getExtraStackForEnvir() const;
    virtual int run(int argc, char *argv[], cConfiguration *configobject);
    
    void sim_step();
    void sim_stop();
    
protected:
    virtual void run();
    virtual void printUISpecificHelp();
    
    virtual void readOptions();
    virtual void readPerRunOptions();
    virtual void askParameter(cPar *par, bool unassigned);
    
    // new functions:
    void help();
    void simulate();
    const char *progressPercentage();
    
    void installSignalHandler();
    void deinstallSignalHandler();
    static void signalHandler(int signum);

};
/** @} */
#endif // TRES_TRESENV_HDR
