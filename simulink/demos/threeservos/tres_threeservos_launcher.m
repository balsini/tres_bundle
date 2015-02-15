% (simple) task set description
%
%                 % type           %iat   %rdl   %ph
task_set_descr = {'PeriodicTask', 0.004, 0.004,    0; ...
                  'PeriodicTask', 0.005, 0.005,    0; ...
                  'PeriodicTask', 0.006, 0.006,    0};

% sequences of pseudo instructions (task codes)
t1_descr = {'fixed(0.002)'};
t2_descr = {'fixed(0.002)'};
t3_descr = {'fixed(0.002)'};

% launch the simulink model
uiopen('tres_threeservos.mdl',1);
%sim('tres_threeservos.mdl');
