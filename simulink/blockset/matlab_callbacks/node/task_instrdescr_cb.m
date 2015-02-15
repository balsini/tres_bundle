function task_instrdescr_cb(blk)

    % Get the variable name
    instr_descr_name = get_param(blk, 'conf_name');

    % Evaluate it in the caller worskpace
    % (i.e., check if the variable does exist)
    try
        instr_descr = evalin('caller', instr_descr_name);
    catch %#ok<CTCH>
        if ~isempty(instr_descr_name),
            errordlg (...
                sprintf('Instruction-set description variable (%s) not found',instr_descr_name),...
                'T-Res Task Error'...
            );
        end
        return;
    end

    % Check if the variable has the right format
    %
    % Check if it's a cell array
    if ~isa(instr_descr, 'cell'),
        errordlg('The instruction-set description variable must be a cell array', 'T-Res Task Error');
        return;
    end

    % Check if it has at least one row
    [id_c, id_r] = size(instr_descr);
    if id_r < 1,
        errordlg('The instruction-set description variable must have at least one row (empty task-code is not allowed)', 'T-Res Task Error');
        return;
    end

    % Check if it's not a matrix
    if (id_r ~= 1) && (id_c ~= 1),
        errordlg('The instruction-set description variable must have at least one element and cannot be a matrix', 'T-Res Task Error');
        return;
    end

    % Check all the elements are strings (type)
    if ~all(cellfun(@ischar, instr_descr(:))),
        errordlg('The instruction-set description variable must contain only string elements', 'T-Res Task Error');
        return;
    end

end