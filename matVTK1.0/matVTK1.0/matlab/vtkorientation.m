% vtkorientation([HANDLE,] [type])
% Add orientation widget to scene.
% 
% Parameters:
% HANDLE 	(optional) explicit VTK handle
% type 	(optional) what kind of widget to display, either 'cube' (default) or 'axes'
% Adds a widget to the scene, that stays aligned to the world coordinate 
% system and therefore allows for spacial orientation. There exist two 
% variations of the widget.
% Setting the type to 'axes' shows three arrows along the main axes x,y,z. 
% Using 'cube' as parameter displays a cube with different labels on each face.
%
function [component window] = vtkorientation(varargin)


    [win, comp, args, config] = extractHandles(varargin);

    p = inputParser;
    
    
    p.addOptional('orientationWidget', 'axis', @ischar);
    p.parse(args{:});
    
    name = p.Results.orientationWidget;
    
    if( ~(strcmp(name, 'axis') || strcmp(name, 'cube')) )
        disp('using default "cube" widget');
        name = 'cube';
    else
        disp(['widget mode ' name]);
    end
    
    config.(name) = 1;
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.special, dp.minor.orientation, win, comp);

    
    dp.sendConfig(config);
    [success window component] = dp.readHandles();
    dp.close();

end