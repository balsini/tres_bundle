# tres_bundle - RT scheduling, network COM and control logics co-simulation in Simulink

T-Res is a co-simulation framework which integrates external simulation engines in Simulink to enable the simulation of timing delays dependent on code execution, scheduling of tasks, and network communication latencies (messages), and to verify their impact on the performance of controls. More info about the T-Res project can be found at the [T-Res website](http://retis.sssup.it/tres/).

This package is an 'all-in-one' bundle including the _T-Res base interfaces_, i.e., abstractions that are specific to the RTOS and network COM domains, _concrete implementors_ for the base interfaces (on top of the [RTsim](http://rtsim.sssup.it/) and [OMNeT++](http://www.omnetpp.org/) simulation engines), and a set of _S-Function implementations_ that bind base interfaces and implementors to the Simulink engine.

Refer to the INSTALL document included in the tres_bundle distribution for info about installing and start using T-Res with Simulink.

# Authors

* Matteo Morelli (maintainer), matteo.morelli@sssup.it
* Fabio Cremona, fabio.cremona@sssup.it
* Marco Di Natale, marco.dinatale@sssup.it

# Note on Licensing

tres_bundle is made up of a set of modules and each module comes with its own license. The source tree containing interfaces and abstract classes (tres_bundle/base) is released under the terms of the permissive, industry-friendly 3-Clause BSD License. S-Function implementations (tres_bundle/simulink) are also available under the same license. The RTSim-based base-class implementor (tres_bundle/adapters/rtsim) comes under the terms of the GNU General Public License version 2 (or later). The OMNeT++-based base-class implementor (tres_bundle/adapters/omnetpp) is released under a combination of the [OMNeT++ APL](http://www.omnetpp.org/intro/license) and the 3-Clause BSD license. Pleaes refer to the [OMNEST Licensing Options](http://www.omnest.com/licensingfaq.php) to see if you need to buy a commercial license for OMNeT++ for your simulations.
