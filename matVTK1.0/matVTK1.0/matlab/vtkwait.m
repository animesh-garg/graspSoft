% vtkwait([WINHANDLE])
%
% halt matlab script execution and wait for gui (inter)action

function [] = vtkwait(varargin)
	[win, comp, args, config] = extractHandles(varargin);

    nargs = length(args);
    
    if(nargs > 0 || comp ~= 0)
        error('only window handle is allowed as vtkwait argument.');
    end
    
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.interactive, dp.minor.wait, win, comp);

    disp('waiting for matVTK gui.');
    dp.waitForServer();
    
    [component window succes] = dp.readHandles();
    
    % Dirty, Dirty, but matlab crashes, when it tries to write to a closed
    % network stream
    %dp.close();
    
end