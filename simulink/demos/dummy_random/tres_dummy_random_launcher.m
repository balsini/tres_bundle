% (simple) task set description
%
%                 % type          %iat  %rdl   %ph
task_set_descr = {'PeriodicTask',   20,   20,    0; ...
                  'PeriodicTask',   30,   30,    0};

% sequences of pseudo instructions (task codes)
t1_instrs = {'delay(unif(2,3))', 'delay(unif(3,4))'};
t2_instrs = {'delay(unif(3,4))'; 'delay(unif(7,8))'; 'delay(unif(7,8))'};

% launch the simulink model
uiopen('tres_dummy_random.mdl',1);
