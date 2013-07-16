% vtktitle([WINHANDLE, ] 'title of scene')
% add a title to the scene
% 
% Parameters:
% WINHANDLE (optional) explicit VTK handle
% title 	Title to set for the scene
%
% Set the scene tile to the given string. It is displayed at the top of the 
% render window and aligned at the vertical center. The text size scales 
% with the window size.


function [component] = vtktitle(varargin)

    [win, comp, args, config] = extractHandles(varargin);

    p = inputParser;
    
    
    p.addRequired('titleText', @ischar);
    p.parse(args{:});
    
    results = p.Results;
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.special, dp.minor.title, win, comp);

    dp.sendString(results.titleText);
    
    dp.sendConfig(config);
    [success window component] = dp.readHandles();
    dp.close();

end