% (simple) task set description
%
%                 % type          %iat  %rdl   %ph
task_set_descr = {'PeriodicTask',   20,   20,    0; ...
                  'AperiodicTask',   0,    5,    0; ...
                  'PeriodicTask',   30,   30,    0};

% sequences of pseudo instructions (task codes)
t1_instrs = {'fixed(2)', 'fixed(3)'};
t2_instrs = {'fixed(1)'; 'fixed(2)'};
t3_instrs = {'fixed(3)'; 'fixed(7)'; 'fixed(7)'};

% launch the simulink model
uiopen('tres_dummy_aperiodic.mdl',1);
