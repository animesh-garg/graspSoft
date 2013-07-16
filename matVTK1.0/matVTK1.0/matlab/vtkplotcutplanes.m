% DATAHANDLE = vtkplotcutplanes([WINHANDLE,] volume, points, vectors, [LUTname | colors,] [,config])
% 
% cut planes through volume
% 
% Parameters:
% HANDLE 	(optional) explicit VTK handle
% volume 	3 dimensional volume matrix
% points 	Nx3 matrix containing coordinates (see Points), used as plane origins
% normals 	Nx3 matrix, plane normal vectors
% LUTname 	(optional) name of the color LUT as string
% colors 	(optional) explicit color LUT (see Color Lookup Tables)
% config 	(optional) config structure (see Configuration parameters)
%
% vtkplotcutplanes intersects a volume with the plane or planes specified 
% by the values in points and vectors. Optionaly a color LUT can be 
% specified by name, or as matrix. Points and vectors must be of equal length.
% A color LUT is used, to map the volume data on the intersecting plane. 
% If no name or explicit LUT is given, the default LUT is used.

function [component] = vtkplotcutplanes(varargin)
    [win, comp, args, config] = extractHandles(varargin);

    p = inputParser;
    
    p.addRequired('volume', @isvolume);
    p.addRequired('points', @ispoints);
    p.addRequired('vectors', @ispoints);
    
    
    p.addOptional('lutName', [], @ischar);
    p.addOptional('colors', [], @iscolors);
    
    p.parse(args{:});
    
    results = p.Results;
    
    if ~isequallength(results.points, results.vectors)
        error('points and vectors must be of equal length');
    end
    
    if(~isempty(results.lutName) && ~isempty(results.colors))
       warning('Got both LUT name and color matrix, ignoring LUT name');
       results.lutName = [];
    end
    
    
    if(results.lutName)
        [results.colors opacity] = loadLUT(results.lutName);
    end
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.plot, dp.minor.cutplane, win, comp);

    dp.sendVolume(results.volume);
    dp.sendPoints(results.points);
    dp.sendMatrix(results.vectors);
    
    if ~isempty(results.colors)
        dp.sendColor(results.colors);
    end
    
    
    dp.sendConfig(config);
    
    [success window component] = dp.readHandles();
    
    dp.close();
end
