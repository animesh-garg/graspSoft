% vtkquiver([WINHANDLE,] points, vectors, [config, ...])
% 
% Parameters:
% WINHANDLE 	(optional) explicit VTK handle
% points    	Nx3 matrix containing coordinates (see Points)
% vectors   	Nx3 vector list
% config    	(optional) config structure (see Configuration parameters)
%
% vtkquiver() plots 3 dimensional vectors as arrows at the locations 
% specified in points. The format of the vector list is the same as for the 
% points.
% The vectors are scaled with their length and also colored with a jet 
% colormap, i.e. small vectors blue, big ones red.
% 
% Configuration:
% scaling       0/1 scale arrows depending on vector size
% scaleFactor   double, factor to control how "strong" scaling is applied
%               when a 'color' value is applied, all arrows are of that color.


function [component] = vtkquiver(varargin)

    [win, comp, args, config] = extractHandles(varargin);

    p = inputParser;
    
    p.addRequired('points', @ispoints);
    p.addRequired('vectors', @ispoints);
    
    p.parse(args{:});
    
    results = p.Results;
    
    if ~isequallength(results.points, results.vectors)
        error('points and vectors must be of equal length');
    end
    
    
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.plot, dp.minor.quiver, win, comp);

    dp.sendPoints(results.points);
    dp.sendMatrix(results.vectors);
        
    dp.sendConfig(config);
    [success window component] = dp.readHandles();
    dp.close();
    
end
