classdef matvtkClient < handle

    
    properties
        % communication port
        dataPort = matvtkDataPort;
        
        % reak data arguments
        window;
        component;
        arguments;
        config;
        returnValues = {};
        
    end
    properties(SetAccess=private, GetAccess=private)
        % detecting optional/mandatory arguments, argument types
        inputParser = inputParser;
    end
    
    methods
        function obj = matvtkClient()
        end
              
        function [] = parseArguments(obj, args)
           [obj.window, obj.component, obj.arguments, obj.config] = extractHandles(args);
           
           if(numel(obj.arguments) > 0)
                obj.inputParser.parse(obj.arguments{:});
                
           end
           
        end
        
        function [success] = process(obj)
            
            success = 0;
            obj.dataPort.connect();
            obj.dataPort.checkVersion();
            
            dp.sendCommand(dp.major.special, dp.minor.open, win, comp);
            
            
            dp.sendConfig(obj.config);
            
            [obj.component obj.window succes] = dp.readHandles();

            % read data return values here
            obj.returnValues = dp.readReturnValues();
            
            obj.dataPort.close();
        end
        
        
        
    end
end