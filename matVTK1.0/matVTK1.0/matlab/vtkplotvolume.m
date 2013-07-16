% vtkplotvolume([HANDLE], volume, [LUTname | colors, opacity], [config, ...])
% Plot volume.
%
% Volume rendering of the input matrix. Currently single (float) and double 
% format are supported. The VTK TextureRenderer3D is default for the 
% rendering. However it downsamples the input volume to a size not bigger 
% than 128x256x256 this can result in poor image quality. 
% 
% Parameters:
% HANDLE 	(optional) explicit VTK handle
% volume 	mandatory parameter, a matrix containing the volume (see Volumes)
% lutName 	(optional) name (string) of the color/opacity LUT pair to be used
% color 	(optional) color table (see Color Lookup Tables)
% opacity 	(optional) opacity lookup table (see Opacity Lookup Tables)
% config 	(optional) config structure (see Configuration parameters)
% 
% If no color LUT is given, a jet colormap is used, with a constant opacity 
% value of 0.05 throughout the volume. see also for the stored color/opacity 
% LUT pairs in LUTlist.mat in the base directory. The table pairs in there 
% can be referenced by the name (LUTname parameter).
% 
% Configuration
% volumeInterpolation   defines interpolation between volume values, can be
%                       "nearest" or "linear" (default).
% volumeSpacing         vector of sampling density in x,y,z directions, 
%                       default is isotropic [1 1 1]
% rayCast               0/1 (default: 0). if set to on (1), a fixed point 
%                       raycaster is used, i.e. the floating point data is 
%                       resampled to 15bit values.

function [component] = vtkplotvolume(varargin)

% vtkplotvolume([HANDLE], volume, [LUTname | colors, opacity], [config, ...])
        
    [win, comp, args, config] = extractHandles(varargin);
    
    nargs = length(args);
    if(nargs < 1 || nargs > 3)
       error('invalid number of arguments');
    end
    
    volume = args{1};
    if(length(size(volume)) ~= 3)
        error('volume must have 3 dimensions');
    end
    
    LUTname = '';
    colors = [];
    opacity = [];

    load LUTlist
    
    p = inputParser;
       
    p.addRequired('volume', @(v)length(size(v)));
    
    hasname = 0;
    if(nargs == 2)
        p.addOptional('LUTname', LUTname, @islutname);
        hasname = 1;
    elseif(nargs==3)
        p.addOptional('colors', colors, @iscolors);
        p.addOptional('opacity', opacity, @isopacity);
    end
    
    p.parse(args{:});
    
    
    results = p.Results;
        
    luts = 1;
      % load and send correct LUT data, if any
    if(hasname == 1 && ~isempty(p.Results.LUTname))
        [colors opacity] = loadLUT(p.Results.LUTname);
    elseif(hasname == 0 && isfield(results, 'colors') && ~isempty(results.colors) && ~isempty(results.opacity))
       colors =  results.colors;
       opacity = results.opacity;
    else
        if(~all([isempty(LUTname) isempty(colors) isempty(opacity)]))
            error('invalid color/opacity/lookuptable data');
        end
        luts = 0;
    end
    
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.plot, dp.minor.volume, win, comp);
    dp.sendVolume(results.volume);
    
    
    if(luts)
        dp.sendColor(colors);
        dp.sendOpacity(opacity);
    end
    
    % send config
    dp.sendConfig(config)
    
    component = -1; win = -1; success = -1;
    [success window component] = dp.readHandles()
    
    dp.close();
    
end
