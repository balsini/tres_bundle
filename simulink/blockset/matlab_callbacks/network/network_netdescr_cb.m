function network_netdescr_cb(blk)

    % Get the variable name
    net_descr_name = get_param(blk, 'net_descr_name');

    % Evaluate it in the caller worskpace
    % (i.e., check if the variable does exist)
    try
        net_descr = evalin('caller', net_descr_name);
    catch %#ok<CTCH>
        if ~isempty(net_descr_name),
            errordlg (...
                sprintf('Network description variable (%s) not found',net_descr_name),...
                'T-Res Network Error'...
            );
        end
        return;
    end

    % Check if the variable has the right format
    %
    % Check if it's a cell array
    if ~isa(net_descr, 'cell'),
        errordlg('The network description variable must be a cell array', 'T-Res Network Error');
        return;
    end

    % Check if it has at least one row
    if size(net_descr, 1) < 1,
        errordlg('The network description variable must have at least one row (describing at least the network topology)', 'T-Res Network Error');
        return;
    end

    % Check if it has at least 2 columns
    if size(net_descr, 2) < 2,
        errordlg('The network description variable must have at least two (2) columns (descr_type, descr_file_paths)', 'T-Res Network Error');
        return;
    end

    % Check all the elements of 1st (type) and
    % 2nd columns are strings
    if ~all(cellfun(@ischar, net_descr(:,1:2))),
        errordlg('The network description variable must have string elements in the first two columns', 'T-Res Network Error');
        return;
    end

end