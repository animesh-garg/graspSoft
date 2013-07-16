% vtkannotate([VTKHANDLE,] position, text, [, config, ...])
%
% add text to a position
% Parameters:
% HANDLE 	(optional) explicit VTK handle
% position 	1x3 coordinates
% text 	descriptive text
% config 	(optional) config structure (see Configuration parameters)
% 
% Sets a 2D font near the specified position (a single point), additionally a guideline is drawn from the text to the exact position. The font does not scale, when the scene is rotated or scaled.
% Text configuration settings can be applied to the specified text. see Text Formatting.
function [] = vtkannotate(varargin)

    [win, comp, args, config] = extractHandles(varargin);

    p = inputParser;
    
    
    p.addRequired('position', @ispoints);
    p.addRequired('text', @ischar);
    p.parse(args{:});
    
    results = p.Results;
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.special, dp.minor.annotation, win, comp);

    dp.sendMatrix(results.position);
    dp.sendString(results.text);
    
    dp.sendConfig(config);
    dp.close();

end