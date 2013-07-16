% HANDLE = VTKINIT()
% Create new VTK handle.
% vtkinit creates a new handle and sets it as the current. It takes no 
% arguments and returns the newly created handle.
% 
% % create explicit VTK handle
% VTK = vtkinit;
% VTK2 = vtkinit;
% 
% % new the current handle is identical to VTK2
% if VTK2==gcvtk()
%         disp('VTK2 is new the current handle.')
% end

function [window] = vtkinit(varargin)

	[win, comp, args, config] = extractHandles(varargin);

    nargs = length(args);
    
    if(nargs > 0 || win ~= 0 || comp ~= 0)
        error('no arguments for vtkinit allowed');
    end
    
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.special, dp.minor.open, win, comp);

    dp.sendConfig(config);
    
    [component window succes] = dp.readHandles();
    
    dp.close();
    
end