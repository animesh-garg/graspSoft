% [[config component window] = vtkconfig([DATAHANDLE, ] [WINHANDLE, ] config_params)]
% 
% Get / set config parameters:
% Always allows to get / set global config parameters.
% When a HANDLE for a specific plot component is given, specific config
% parameters become available.
% 
% The format for the config parameters is either 'key', value pairs or a
% struct with parameter names as field names:
%
% vtkconfig(HANDLE, 'key', [1 2 3]  % sets parameter 'key' to vector [1 2 3]
% OR:
% config = struct();
% config.key = [1 2 3];
% vtkconfig(HANDLE, config)
%
% (default handles are current window and current plot component)
%
% return values: struct of all config parameters / values for the used
% handle as well as used plot component and window handles.
%
% All config parameters / values that can be set by this tool can also be
% changed in the GUI menu: Window -> Plot Component -> Settings...
%
% all matVTK plot scripts accept config parameters (as struct or key /
% value pairs) as their last arguments. i.e.
% vtkplotpoints(rand(10,3), 'color', [1 0 0]); % plot 10 points with color red
% 
function [config component window] = vtkconfig(varargin)

    [win, comp, args, config] = extractHandles(varargin);

    if(length(args) > 0)
       error('no arguments allowed.');
    end
    
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.special, dp.minor.config, win, comp);

    
    dp.sendConfig(config);
    
    [success window component] = dp.readHandles();
    
    [returnData] = dp.readReturnValues();
    
    
    dp.close();

    
    configJson = returnData{1}';
    config = json2mat(configJson);
end