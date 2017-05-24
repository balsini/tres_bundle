%% task-set description
%
task_set_descr = {...
%   % type            %name             %iat      %rdl        %ph      % prio
    'PeriodicTask',   'Task_spr',       0.100,    0.100,      0.0,     0; ...     % set-points' reading
    'PeriodicTask',   'Task_pos',       0.020,    0.020,      0.0,     5; ...     % attitude control
    'PeriodicTask',   'Task_yaw',       0.050,    0.050,      0.0,     15; ...    % yaw control
    'PeriodicTask',   'Task_alt',       0.025,    0.025,      0.0,     10; ...    % altitude control
};

%% sequences of pseudo instructions (task codes)
spr_instrs = {'delay(unif(0.001,0.002))'};
att_instrs = {'delay(unif(0.005,0.008))'; 'delay(unif(0.003,0.007))'; 'delay(unif(0.002,0.004))'};  % pos loop +  att loop +  ctl mixer
yaw_instrs = {'delay(unif(0.004,0.006))'};
alt_instrs = {'delay(unif(0.008,0.009))'};

%% launch the simulink model
mdl_quadcopter
uiopen('sl_quadcopter_tres_poster_df.slx',1);
%sim('sl_quadcopter_tres_poster.slx');
