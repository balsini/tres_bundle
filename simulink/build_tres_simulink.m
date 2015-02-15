% DO NOT MODIFY BELOW!
% ====================
TRES_BASE_INC    = [pwd,'/../base/include/'];
TRES_RTSIM_INC   = [pwd,'/../adapters/rtsim/include/'];
RTSIM_INC        = [pwd,'/../3rdparty/rtsim/metasim/src/ -I', ...
                    pwd,'/../3rdparty/rtsim/rtlib/src/'];
TRES_OMNETPP_INC = [pwd,'/../adapters/omnetpp/include/'];
TRES_BASE_LIB    = [pwd,'/../build/base/src/'];
TRES_RTSIM_LIB   = [pwd,'/../build/adapters/rtsim/src/'];
TRES_OMNETPP_LIB = [pwd,'/../build/adapters/omnetpp/src/'];
MEX_CFLAGS       = 'CXXFLAGS=''$CXXFLAGS -Wall -O0 --std=c++0x''';
MEX_INC          = sprintf('-I%s -I%s -I%s -I%s', RTSIM_INC, TRES_BASE_INC, TRES_RTSIM_INC, TRES_OMNETPP_INC);
MEX_LIB          = sprintf('-L%s -L%s -L%s -ltres_base -ltres_rtsim -ltres_omnetpp_gw', ...
                            TRES_BASE_LIB, TRES_RTSIM_LIB, TRES_OMNETPP_LIB);
MEX_OUT      = '-outdir libs';
MEX_IN_CLL   = {MEX_OUT, MEX_CFLAGS, ' -g ', MEX_INC, MEX_LIB};
MEX_IN_CMD   = [sprintf('%s ',MEX_IN_CLL{1:end-1}), MEX_IN_CLL{end}];
MDL_SRC      = {'src/node/tres_task.cpp', ...
                'src/node/tres_kernel.cpp', ...
                'src/network/tres_message_df.cpp', ...
                'src/network/tres_network_df.cpp'};
if ~exist('libs','dir'),
    mkdir('libs');
end

disp ('Building T-Res mex files...');
cellfun(@(sfun) eval(sprintf('mex %s %s', MEX_IN_CMD, sfun)), MDL_SRC, 'UniformOutput', true);
disp ('Building T-Res mex files... DONE!');
clear LD_PATH RTSIM_INC TRES_BASE_INC TRES_RTSIM_INC TRES_OMNETPP_INC TRES_BASE_LIB TRES_RTSIM_LIB TRES_OMNETPP_LIB MEX_CFLAGS MEX_INC MEX_LIB MDL_SRC MEX_IN_CLL MEX_IN_CMD MEX_OUT
