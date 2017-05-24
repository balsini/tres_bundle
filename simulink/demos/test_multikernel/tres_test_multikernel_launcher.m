% (simple) descriptions of task sets
%
%                 % type              %name    %iat  %rdl   %ph
task_set_descr1 = {'PeriodicTask', 'Task1_1',     0,   15,    0};
task_set_descr2 = {'PeriodicTask', 'Task1_2',     6,    6,    0;...
                   'PeriodicTask', 'Task2_2',     8,    8,    0;...
                   'PeriodicTask', 'Task3_2',    10,   10,    0};
task_set_descr3 = {'PeriodicTask', 'Task1_3',     8,    8,    0;...
                   'PeriodicTask', 'Task2_3',    12,   12,    0};

% sequences of pseudo instructions (task codes)
t11_instrs = {'fixed(2)','fixed(4)'};
t12_instrs = {'fixed(3)','fixed(1)'};
t22_instrs = {'fixed(5)'};
t32_instrs = {'fixed(6)'};
t13_instrs = {'fixed(4)'};
t23_instrs = {'fixed(5)','fixed(3)'};

% launch the simulink model
%uiopen('tres_test_multikernel.slx',1);
sim('tres_test_multikernel.slx');
%sim('tres_test_multikernel_periodic.slx');
