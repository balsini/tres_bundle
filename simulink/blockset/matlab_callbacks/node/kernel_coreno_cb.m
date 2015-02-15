function kernel_coreno_cb(blk)

    % Get the number of CPU cores
    core_num_ = get_param(blk, 'core_num');

    % Evaluate it
    try
        core_num = eval(core_num_);
    catch ME
        errordlg (...
            sprintf('The value specified as number of CPU cores has generated a %s.\n%s',ME.identifier,ME.message),...
            'T-Res Kernel Error'...
        );
        return;
    end

    % Check it's a finite number (inf/nan raise an error)
    if ~isfinite(core_num),
        errordlg('The number of CPU cores must be finite', 'T-Res Kernel Error');
        return;
    end

    % Check it has no imaginary part
    if imag(core_num) ~= 0,
        errordlg('(?!) The number of CPU cores cannot have an imaginary part', 'T-Res Kernel Error');
        return;
    end

    % Check it's a scalar
    if numel(core_num) ~= 1,
        errordlg('The number of CPU cores must be a scalar (not empty)', 'T-Res Kernel Error');
        return;
    end

    % Check it's strictly greater than 0
    if ~(core_num > 0),
        errordlg('The number of CPU cores must be > 0', 'T-Res Kernel Error');
        return;
    end

    % Check it contains an integer value
    % (It's different from checking it's integer-object (via isinteger()))
    if floor(core_num) ~= core_num,
        errordlg('The number of CPU cores must be an integer value', 'T-Res Kernel Error');
        return;
    end
end