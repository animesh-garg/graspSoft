% VTKDESTROY([WINHANDLE])
% 
% free resources, close window
% calling vtkdestroy() releases the resources of the handle given as 
% argument. When called without arguments, the current handle is destroyed.
% 
% The handle stores various scene information and also copies of the data 
% to be displayed. The memory is not released after the vtkshow() command, 
% but only after calling vtkdestroy() or closing the plot window.
% 
% 
% Note:
% Clearing the variable of a handle in the Matlab workspace does not clear 
% the handle itself.
% 
% Examples:
% % create a few handles
% VTK1 = vtkinit;
% VTK2 = vtkinit;
% VTK3 = vtkinit;
% 
% % release current handle (VTK3)
% vtkdestroy()
% 
% % release handle VTK1
% vtkdestroy(VTK1)
% 

function [] = vtkdestroy(varargin)


	[win, comp, args, config] = extractHandles(varargin);

    nargs = numel(args)
    
    if(nargs > 0)
        error('no arguments for vtkdestroy allowed');
    end
    
    disp(['closing component ' num2str(comp) ' in window ' num2str(win)]);
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.special, dp.minor.close, win, comp);

    dp.sendConfig(config);
    
    [component window succes] = dp.readHandles();
    
    dp.close();

end