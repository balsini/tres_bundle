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

    % Check if it has at least 4 columns
    if size(ts_descr, 2) < 4,
        errordlg('The task-set description variable must have at least four (4) columns (type, iat, rdl, ph)', 'T-Res Kernel Error');
        return;
    end

    % Check all the elements of 1st column are strings (type)
    if ~all(cellfun(@ischar, ts_descr(:,1))),
        errordlg('The task-set description variable must have only string elements in the 1st column', 'T-Res Kernel Error');
        return;
    end

    % Check all the elements of 2nd (iat = interarrival time),
    % 3rd (rdl = relative deadline) and
    % 4th (ph = phase) columns are numbers
    % iat and rdl must also be strictly > 0
    iat_rdl_isnum_matrix = cellfun(@(x) tsdescr_elem_is_number(x), ts_descr(:,2:3));
    if ~all(iat_rdl_isnum_matrix(:)) || ~all(cellfun(@(x) tsdescr_elem_is_number(x,true), ts_descr(:,4))),
        errordlg('The task-set description variable must have numeric elements in the column range 2:4', 'T-Res Kernel Error');
        return;
    end

    % Check all the elements in the (optional)
    % 5th column are integer numbers
    if size(ts_descr, 2) > 4,
        if ~all(cellfun(@(x) tsdescr_elem_is_number(x, true), ts_descr(:,5))),
            errordlg('The task-set description variable must have numeric elements in the 5th column', 'T-Res Kernel Error');
            return;
        end
        if ~all(cellfun(@(x) floor(x) == x, ts_descr(:,5))),
            errordlg('All the elements in the 5th column of the task-set description variable must have an integer value', 'T-Res Kernel Error');
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