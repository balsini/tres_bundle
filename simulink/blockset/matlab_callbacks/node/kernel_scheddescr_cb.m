function kernel_scheddescr_cb(blk)

    % Get the variable name
    sched_pol_descr = get_param(blk, 'sched_pol_descr');

    % Evaluate it in the caller worskpace
    % (i.e., check if the variable does exist)
    try
        sp_descr = evalin('caller', sched_pol_descr);
    catch %#ok<CTCH>
        if ~isempty(sched_pol_descr),
            errordlg (...
                sprintf('Sched. policy description variable (%s) not found', sched_pol_descr),...
                'T-Res Kernel Error'...
            );
        end
        return;
    end

    % Check if the variable has the right format
    %
    % Check if it's a cell array
    if ~isa(sp_descr, 'cell'),
        errordlg('The sched. policy description variable must be a cell array', 'T-Res Kernel Error');
        return;
    end

    % Check if it's exactly one row
    if size(sp_descr,1) ~= 1,
        errordlg('The sched. policy description variable must have exactly one row', 'T-Res Kernel Error');
        return;
    end

end