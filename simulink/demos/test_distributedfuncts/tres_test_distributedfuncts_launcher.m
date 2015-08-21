% (simple) descriptions of task sets
%
%                 % type              %name    %iat  %rdl   %ph
task_set_descr1 = {'PeriodicTask', 'Task1_1',    10,   10,    0};
task_set_descr2 = {'PeriodicTask', 'Task1_2',    15,   15,    0};

% sequences of pseudo instructions (task codes)
t11_instrs = {'fixed(2)','fixed(4)'};
t12_instrs = {'fixed(3)'};

% launch the simulink model
uiopen('tres_test_distributedfuncts_df.slx',1);
%sim('tres_test_distributedfuncts_df.slx');
