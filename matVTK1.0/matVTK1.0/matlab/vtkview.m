% vtkview([WINHANDLE,] azimuth, elevation)
% set viewpoint
% 
% Parameters:
% HANDLE        (optional) explicit VTK handle
% azimuth       horizontal rotation (in degrees)
% elevation 	vertical elevation (in degrees)
%
% rotates the camera around the focal point (the point the camera looks at).
% The given parameters change the current position about the given angles. 
% i.e. vtkview(10,0) rotates 10 degree horizontally, and keeps current elevation.


function [] = vtkview(varargin)

    [win, comp, args, config] = extractHandles(varargin);

    p = inputParser;
    
    
    p.addRequired('azimut', @isscalar);
    p.addRequired('elevation', @isscalar);
    p.parse(args{:});
    
    results = p.Results;
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.special, dp.minor.view_direction, win, comp);

    dp.sendMatrix(results.azimut);
    dp.sendMatrix(results.elevation);
    
    dp.sendConfig(config);

    [success window component] = dp.readHandles();

    dp.close();

end