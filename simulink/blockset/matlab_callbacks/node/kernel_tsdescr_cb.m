function kernel_tsdescr_cb(blk)

    % Get the variable name
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

    % Check if the variable has the right format
    %
    % Check if it's a cell array
    if ~isa(ts_descr, 'cell'),
        errordlg('The task-set description variable must be a cell array', 'T-Res Kernel Error');
        return;
    end

    % Check if it has at least one row
    if size(ts_descr, 1) < 1,
        errordlg('The task-set description variable must have at least one row (empty task-sets are not allowed)', 'T-Res Kernel Error');
        return;
    end

    % Check if it has at least 5 columns
    if size(ts_descr, 2) < 5,
        errordlg('The task-set description variable must have at least four (5) columns (type, name, iat, rdl, ph)', 'T-Res Kernel Error');
        return;
    end

    % Check all the elements of 1st & 2nd column are strings (type, name)
    if ~all(cellfun(@ischar, ts_descr(:,1:2))),
        errordlg('The task-set description variable must have only string elements in the two first columns', 'T-Res Kernel Error');
        return;
    end

    % Check all the elements of 3rd (iat = interarrival time),
    % 4th (rdl = relative deadline) and
    % 5th (ph = phase) columns are numbers
    % rdl must also be strictly > 0
    rdl_isnum_vector = cellfun(@(x) tsdescr_elem_is_number(x), ts_descr(:,4));
    if ~all(rdl_isnum_vector(:)) || ~all(cellfun(@(x) tsdescr_elem_is_number(x,true), ts_descr(:,[3,5]))),
        errordlg('The task-set description variable must have numeric elements in the column range 3:5', 'T-Res Kernel Error');
        return;
    end

    % Check all the elements in the (optional)
    % 6th column are integer numbers
    if size(ts_descr, 2) > 5,
        if ~all(cellfun(@(x) tsdescr_elem_is_number(x, true), ts_descr(:,6))),
            errordlg('The task-set description variable must have numeric elements in the 6th column', 'T-Res Kernel Error');
            return;
        end
        if ~all(cellfun(@(x) floor(x) == x, ts_descr(:,6))),
            errordlg('All the elements in the 6th column of the task-set description variable must have an integer value', 'T-Res Kernel Error');
            return;
        end
    end
end

function res = tsdescr_elem_is_number(x, skip)
    if nargin == 1,
        skip = false;
    end
    res = 1;

    % Check it's a finite number (inf/nan raise an error)
    if ~isfinite(x),
        res = 0;
    end

    % Check it has no imaginary part
    if imag(x) ~= 0,
        res = 0;
    end

    % Check it's a scalar
    if numel(x) ~= 1,
        res = 0;
    end

    if ~skip,
        % Check it's strictly greater than 0
        if ~(x > 0),
            res = 0;
        end
    end
end
