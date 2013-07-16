% HANDLE = vtkisosurface([WINHANDLE], volume, [isoValues | colorLUT], [config, ...])
% 
% generate isosurfaces of volume
% 
% Parameters:
% HANDLE        (optional) explicit VTK handle
% volume        3 dimensional volume matrix
% isoValues 	(optional) vector of volume values used for isosurfaces
% colorLUT      (optional) color table (see Color Lookup Tables)
% config        (optional) config structure (see Configuration parameters)
%
% Only the argument volume is mandatory. If no other parameters are specified, 
% 10 iso values are created automatically from (including) [min ... max].
% When the iso values are given explicitly, the surfaces are created at the 
% respective values, using the same color.
% 
% If a color table is specified instead, the surfaces are generated on the 
% values in the first column in the table, using the corresponding colors.
% 
function [component] = vtkisosurface(varargin)
    [win, comp, args, config] = extractHandles(varargin);

    p = inputParser;
    
    p.addRequired('volume', @isvolume);
    
    
    p.addOptional('isoValues', [], @isvector);
    p.addOptional('colors', [], @iscolors);
    
    p.parse(args{:});
    
    results = p.Results;
    
    
    if(~isempty(results.isoValues) && ~isempty(results.colors))
       warning('Got both iso values and color matrix, ignoring iso values');
       results.isoValues = [];
    end
    
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.plot, dp.minor.isosurface, win, comp);

    dp.sendVolume(results.volume);
    
    if(~isempty(results.isoValues))
        dp.sendMatrix(results.isoValues);
    else
        if ~isempty(results.colors)
            dp.sendColor(results.colors);
        end
    end
    
    dp.sendConfig(config);
    
    [success window component] = dp.readHandles();
    
    dp.close();
end