% DATAHANDLE = vtkplotpoints([WINHANDLE,] points, [labels | labels, colors], [config, ...])
% plot optionally labeled points
% 
% Parameters:
% WINHANDLE     (optional) explicit VTK handle
% points        a list of points (see Points)
% pointLabels 	(optional) list of labels (same length as points).
% pointColors 	(optional) a color LUT for the labels (see Color Lookup Tables)
% config        (optional) config structure (see Configuration parameters)
% 
% Plot 3D points at the positions specified by the points argument. The 
% points are represented by a sphere with the center at the point position. 
% If the pointLabels parameter is given the points are auto colored with a 
% jet colormap from the smallest label to the biggest.
% If pointColors is given, the spheres are colored corresponding to their 
% labels and the defined colors.
% 
% Configuration
% pointSize         the radius of the spheres can be set. (default=1.0)
% phiResolution     number of latitude lines
% thetaResolution   number of longitude lines

% vtkplotpoints([HANDLE], points, [labels | labels, colors], [config, ...])
function [component] = vtkplotpoints(varargin)
    [win, comp, args, config] = extractHandles(varargin);

    p = inputParser;
       
    p.addRequired('points', @ispoints);
    
    p.addOptional('labels', [], @islabels);
    p.addOptional('colors', [], @iscolors);
    
    p.parse(args{:});
    
    results = p.Results;
    
    if ~isempty(results.labels) && ~isequallength(results.labels, results.points)
        error('points and labels must be of equal length');
    end
    
    
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.plot, dp.minor.point, win, comp);

    dp.sendPoints(results.points);
    
    if ~isempty(results.labels)
       dp.sendMatrix(results.labels); 
    end
    
    if ~isempty(results.colors)
        dp.sendColor(results.colors);
    end
    
    
    dp.sendConfig(config);
    
    [success window component] = dp.readHandles();
    
    dp.close();
end