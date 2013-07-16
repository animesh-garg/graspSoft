% [CURRENTHANDLE] = gcvtk([HANDLE])
% get / set current VTK handle
% 
% Parameters:
% [in] 	HANDLE 	(optional) explicit VTK handle
% [out] 	CURRENTHANDLE 	the current VTK handle
% gcvtk returns the currently used implicit handle. When given a handle as optional argument, it becomes the new current handle.
% % get the current handle
% VTK1 = vtkinit()
% VTK2 = vtkinit()
% 
% if VTK2==gcvtk()
%   disp('VTK2 is current.');
% end
% 
% % set a new current handle
% CURRENT = gcvtk(VTK1)
% if VTK1 == CURRENT
%   disp('VTK1 is current.')
% end
% Note:
% When not specifying a handle explicitly with other functions, gcvtk() is called internally to get the current handle.

function [window component] = gcvtk(varargin)
	
    [win, comp, args, ~] = extractHandles(varargin);
    

    nargs = length(args);
    
    if(nargs > 0)
        error('max 1 argument allowed for gcvtk');
    end
    
    
    
    dp = matvtkDataPort;
    
    disp('connecting...');
    dp.connect();
        
    disp('version check...');
    dp.checkVersion();
    
    disp('sending command...');
    dp.sendCommand(dp.major.special, dp.minor.get, win, comp);

    disp('reading handles...');
    [success window component] = dp.readHandles();
    
    disp('shutting down...');
    dp.close();


end