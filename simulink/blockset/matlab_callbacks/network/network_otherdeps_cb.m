function network_otherdeps_cb(blk)

    % Get the list of additional libraries
    other_deps = get_param(blk, 'other_deps');

    % Check it's a string
    if ~ischar(other_deps),
        errordlg('Other dependencies (typically a path to additional libraries) must be specified as string', 'T-Res Network Error');
        return;
    end
end
