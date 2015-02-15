function network_msgdescr_cb(blk)

    % Get the variable name
    msgset_descr_name = get_param(blk, 'msgset_descr_name');

    % Evaluate it in the caller worskpace
    % (i.e., check if the variable does exist)
    try
        ms_descr = evalin('caller', msgset_descr_name);
    catch %#ok<CTCH>
        if ~isempty(msgset_descr_name),
            errordlg (...
                sprintf('Message-set description variable (%s) not found',msgset_descr_name),...
                'T-Res Network Error'...
            );
        end
        return;
    end

    % Check if the variable has the right format
    %
    % Check if it's a cell array
    if ~isa(ms_descr, 'cell'),
        errordlg('The message-set description variable must be a cell array', 'T-Res Network Error');
        return;
    end

    % Check if it has at least one row
    if size(ms_descr, 1) < 1,
        errordlg('The message-set description variable must have at least one row (empty message-sets are not allowed)', 'T-Res Network Error');
        return;
    end

    % Check if it has at least 2 columns
    if size(ms_descr, 2) < 2,
        errordlg('The message-set description variable must have at least two (2) columns (type, msg_uid)', 'T-Res Network Error');
        return;
    end

    % Check all the elements of 1st (type) and
    % 2nd columns are strings
    if ~all(cellfun(@ischar, ms_descr(:,1:2))),
        errordlg('The message-set description variable must have string elements in the first two columns', 'T-Res Network Error');
        return;
    end

end
