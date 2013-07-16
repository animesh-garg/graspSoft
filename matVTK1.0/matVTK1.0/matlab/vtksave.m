% vtksave([WINHANDLE,] filename)
% save scene to png file
% 
% Parameters:
% WINHANDLE (optional) explicit VTK handle
% filename 	string of the filename
% config 	(optional) config structure (see Configuration parameters)
%
% Saves the current rendering into a png file of the given size. Either as 
% static png image or as VRML scene (only polygon data can be exported)
% 
% Configuration:
% saveFormat        file format of saved scene: "png" (default), "pov" or "vrml" 
% imageDimension    base size of screenshot size: i.e. 640x480 (default: size of window)
% magnification multiplier for screenshot size, default: 1 (640x480 with multiplier of 2 -> 1280x960)

function [] = vtksave(varargin)

    [window component args, config] = extractHandles(varargin);
    p = inputParser;
       
    p.addRequired('savePath', @ischar);
    p.parse(args{:});
    
    results = p.Results;
    
    if ~isfield(config, 'saveFormat')
        ext = 'png';
        
        pidx = strfind(results.savePath, '.');
        if(~isempty(pidx))
            ext = results.savePath(pidx(end)+1:end);
        end
    
        config.saveFormat = ext; 
    end
    

    dp = matvtkDataPort;
    dp.connect();
    dp.checkVersion();
       
    dp.sendCommand(dp.major.special, dp.minor.save, window, component);
    dp.sendString(results.savePath);
    
        
    dp.sendConfig(config);
    [success window component] = dp.readHandles();
    [returnData] = dp.readReturnValues();
    
    dp.close();

end