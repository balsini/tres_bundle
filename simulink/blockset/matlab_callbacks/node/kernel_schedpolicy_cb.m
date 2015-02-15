function kernel_schedpolicy_cb(blk)

    % Get the value selected in the pop-up menu
    scheduling_policy = get_param(blk, 'scheduling_policy');

    % if it's set to OTHER
    if strcmp(scheduling_policy, 'OTHER'),
        % Set the visibility of parameters 'on' when 
        %   OTHER is selected in the pop-up.
        set_param(gcb,'MaskVisibilities', {'on'; ... % taskset_descr_name
                                           'on'; ... % scheduling_policy
                                           'on'; ... % sched_pol_descr
                                           'on'; ... % dead_miss_rule
                                           'on'; ... % time_res
                                           'on'; ... % core_num
                                           'on'  ... % under_engine
                                           });
    else
        % otherwise
        %
        % Turn off the visibility of the Value field
        %   when OTHER is not selected.
        set_param(gcb,'MaskVisibilities', {'on'; ... % taskset_descr_name
                                           'on'; ... % scheduling_policy
                                          'off'; ... % sched_pol_descr
                                           'on'; ... % dead_miss_rule
                                           'on'; ... % time_res
                                           'on'; ... % core_num
                                           'on'  ... % under_engine
                                           });

        % Get the mask parameter values. This is a cell
        %   array of strings.
        maskStr = get_param(gcb,'MaskValues');

        % Set the string in the edit box equal to ''
        maskStr{3}='';
        set_param(gcb,'MaskValues',maskStr);

        % if it's set to FIXED_PRIORITY
        if strcmp(scheduling_policy, 'FIXED_PRIORITY'),
            % Get the task-set description
            taskset_descr_name = get_param(blk, 'taskset_descr_name');

            % Evaluate it in the caller worskpace
            % (i.e., check if the variable does exist)
            try
                ts_descr = evalin('caller', taskset_descr_name);
            catch %#ok<CTCH>
                if ~isempty(taskset_descr_name),
                    errordlg (...
                        sprintf('Task-set description variable (%s) not found',taskset_descr_name),...
                        'T-Res Kernel Error'...
                    );
                end
                return;
            end

            % Check its number of columns is at least 5
            % (FIXED_PRIORITY needs that the tasks have a priority value)
            if size(ts_descr, 2) < 5,
                errordlg('FIXED_PRIORITY requires the task-set description have at least five (5) columns (type, iat, rdl, ph, PRIO)', 'T-Res Kernel Error');
                return;
            end
        end
    end

end