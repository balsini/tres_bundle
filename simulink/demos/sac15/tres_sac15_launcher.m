% task set description
%                 % type          %iat   %rdl      %ph
task_set_descr = {'PeriodicTask', 0.004, 0.004,    0; ...
                  'PeriodicTask', 0.005, 0.005,    0; ...
                  'PeriodicTask', 0.006, 0.006,    0};

% sequences of pseudo instructions (task codes)
t1_descr = {'fixed(0.002)'};
t2_descr = {'fixed(0.002)'};
t3_descr = {'fixed(0.002)'};

% (network) message-set description
%                % type           %uid of msg on the network
msg_set_descr = {'CanMessageOpp', '11a'; ...
                 'CanMessageOpp', '11b'};

% (simulator-specific) network configuration (e.g., topology, initialization files, etc.)
opp_root_dir = getenv('omnetpp_root');
ss_net_config = {'nedpath', [opp_root_dir,'/inet/examples:',opp_root_dir,'/inet/src:.']; ...
                 'configname', 'CanBus'; ...
                 'inifile', 'omnetpp.ini'};

% launch the simulink model
uiopen('tres_sac15.slx',1);
%sim('tres_sac15.slx');
