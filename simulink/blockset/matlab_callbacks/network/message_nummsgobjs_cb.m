function message_nummsgobjs_cb(blk)

    % Get the number of message objects
    num_msg_objs_ = get_param(blk, 'num_msg_objects');

    % Evaluate it
    try
        num_msg_objs = eval(num_msg_objs_);
    catch ME
        errordlg (...
            sprintf('The value specified as number of message objects has generated a %s.\n%s',ME.identifier,ME.message),...
            'T-Res Message Error'...
        );
        return;
    end

    % Check it's a finite number (inf/nan raise an error)
    if ~isfinite(num_msg_objs),
        errordlg('The number of message objects must be finite', 'T-Res Message Error');
        return;
    end

    % Check it has no imaginary part
    if imag(num_msg_objs) ~= 0,
        errordlg('(?!) The number of message objects cannot have an imaginary part', 'T-Res Message Error');
        return;
    end

    % Check it's a scalar
    if numel(num_msg_objs) ~= 1,
        errordlg('The number of message objects must be a scalar (not empty)', 'T-Res Message Error');
        return;
    end

    % Check it's strictly greater than 0
    if ~(num_msg_objs > 0),
        errordlg('The number of message objects must be > 0', 'T-Res Message Error');
        return;
    end

    % Check it contains an integer value
    % (It's different from checking it's integer-object (via isinteger()))
    if floor(num_msg_objs) ~= num_msg_objs,
        errordlg('The number of message objects must be an integer value', 'T-Res Message Error');
        return;
    end
end
