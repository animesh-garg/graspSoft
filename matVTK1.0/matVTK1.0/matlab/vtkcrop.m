% vtkcrop([DATAHANDLE,] [config, ...])
%
% add data to crop widget
% 
% Parameters:
% HANDLE 	(optional) explicit VTK plot handle (default is curren handle)
% config 	(optional) config structure (see Configuration parameters)
% 
% Displays a crop widget in the scene. When using the handles to intersect 
% the widget's planes with the data, the parts outside the crop widget are 
% cut away.
% It can be called multiple times for different data in the same scene.
% 
% dataHandle
% When plotting data, a mapping is done between Matlab variables and the 
% components displayed in a scene. I.e. when plotting points, the handle 
% returned by the vtkplotpoints function has to be used by 
% vtkcrop: pointsHandle = vtkplotpoints(pointList); 
% vtkcrop(pointsHandle) 
%
% or in short: 
% vtkcrop(vtkplotpoints(pointList))
%
% Tools:
% The tool's name hast to be given as string.
% box Displays a box enclosing the whole scene. the box can be rotated independently of the rest of the scene When moving the box or single planes, the specified data is cut off accordingly
% plane Shows a single plane. Intersecting the plane with the data cuts it away. For easier handling it is initialized with the origin at the center of the scene and the normal vector in [1 1 0] direction.
% The default method is box.
%
% Config params:
% cropTool 	(optional) string specifying the crop tool. 
%           values: 'box' (default) , 'plane'
%
function [] = vtkcrop(varargin)

    [win, comp, args, config] = extractHandles(varargin);
    
    if(win && ~comp) 
       comp = win; win = 0; 
    end
    
    nargs = length(args);
    
    if(nargs > 0)
        error('only handles allowed for vtkcrop.');
    end
    
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.special, dp.minor.crop, win, comp);

    dp.sendConfig(config);
    
    [component window succes] = dp.readHandles();
    
    dp.close();
end