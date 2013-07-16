% HANDLE = vtklegend([WINHANDLE], colorLUT, [title], [config, ...])
% 
% Parameters:
% HANDLE 	(optional) explicit VTK handle
% colorLUT 	color lookup table (see Color Lookup Tables)
% title 	(optional) alternative title string (default: "Values")
% config 	(optional) config structure (see Configuration parameters)
%
% Displays a color bar at the left border of the window, labeled with the 
% according values that are mapped to those colors. Text Formatting values 
% are respected for the title string.
%
function [component] = vtklegend(varargin)

    [win, comp, args, config] = extractHandles(varargin);

    p = inputParser;
    
    
    p.addRequired('colorLut', @iscolors);
    p.addOptional('title', [], @ischar);
    p.parse(args{:});
    
    
    
   
   
   
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.special, dp.minor.color_legend, win, comp);

    dp.sendColor(p.Results.colorLut);
    
    if(~isempty(p.Results.title))
        dp.sendString(p.Results.title);
    end
    
    
    dp.sendConfig(config);
    [success window component] = dp.readHandles();
    dp.close();

end