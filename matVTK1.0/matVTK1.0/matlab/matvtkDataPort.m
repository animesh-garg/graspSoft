

classdef matvtkDataPort < handle

    
    properties
        
        start = 0;
        stop = 0;
        version=1;
        
        javaBufferSize = (10 * 1024 * 1024); % defaults to 50MB
        defaultTimeout = 500000;
        
        major = struct();
        minor = struct();
        data = struct();
        value = struct();
        
        port = 14159;
        host = 'localhost';
        
    end
    properties(SetAccess=private, GetAccess=private)
        net_socket;
        input;
        output;
    end
    
    methods
        % constructor, setup command, data, value structs
        function obj = matvtkDataPort() 
            disp('new matvtkDataPort')
  
            obj.major.plot = 1;
            obj.major.get = 2;
            obj.major.set = 3;
            obj.major.special= 4;
            obj.major.interactive = 5;


            obj.minor.point = 1;
            obj.minor.line = 2;
            obj.minor.volume = 3;
            obj.minor.mesh = 4;
            obj.minor.tensor = 5;
            obj.minor.cutplane = 6;
            obj.minor.quiver = 7;
            obj.minor.isosurface = 8;
            obj.minor.close = 9;
            obj.minor.open = 10;
            obj.minor.remove = 11;
            obj.minor.grid = 12;
            obj.minor.title = 13;
            obj.minor.orientation = 14;
            obj.minor.color_legend = 15;
            obj.minor.view_direction = 16;
            obj.minor.annotation = 17;
            obj.minor.labels = 18;
            obj.minor.get = 19;
            obj.minor.selectpoints = 20;
            obj.minor.wait = 21;
            obj.minor.crop = 22;
            obj.minor.save = 23;
            obj.minor.config = 24;


            obj.data.error = 0;
            obj.data.matrix = 1;
            obj.data.volume = 2;
            obj.data.points = 3;
            obj.data.cell = 4;
            obj.data.color_lut = 5;
            obj.data.string = 6;
            obj.data.config = 7;

            obj.value.void = 0;
            obj.value.bit = 1;
            obj.value.char = 2;
            obj.value.signed_char = 3;
            obj.value.short = 4;
            obj.value.unsigned_short = 5;
            obj.value.integer = 6;
            obj.value.unsigned_integer = 7;
            obj.value.long = 8;
            obj.value.unsigned_long = 9;
            obj.value.float = 10;
            obj.value.double = 11;
            obj.value.idtype = 12;
            obj.value.string = 13;
            
            
        end
    
%        function delete(obj)
%           disp('destructing matvtkDataPort') 
%        end
        
        function [success] = connect(obj)
             %% prepare network
             success = 0;
            try 
                obj.net_socket = java.net.Socket(obj.host, obj.port);

                % get a buffered data input stream from the socket
                obj.input = java.io.DataInputStream(java.io.BufferedInputStream(obj.net_socket.getInputStream));
                obj.output = java.io.DataOutputStream(obj.net_socket.getOutputStream);
                success = 1;
            catch err
               disp('ERROR: matvtkDataPort.connect');
               disp(err.message);
               error('cannot connect to matvtk gui')
            end
            
        end
        
        function [] = close(obj)
            try
                if(obj.net_socket.isConnected() && ~obj.net_socket.isOutputShutdown())
                    obj.output.writeShort(obj.stop);
                    obj.output.flush();
                    % close network streams and sockets
                    obj.output.close();
                    obj.input.close();
                    obj.net_socket.close();
                else
                    disp('network connection already terminated.')
                end
            catch error
                disp('ERROR: matvtkDataPort.close');
                disp(error.message)
                disp(error)
            end
        end
        
        function [] = checkVersion(obj)
            obj.output.writeShort(obj.start);
            obj.output.writeShort(obj.version);
            obj.output.flush; 
            
            serverStart = obj.input.readShort();
            if (serverStart ~= 0)
                error('server started invalid conversation.');
            end

            serverVersion = obj.input.readShort();
            
            if (obj.version ~= serverVersion)
                error(['version mismatch, server: ' num2str(serverVersion) ' client: ' num2str(obj.version)]);
            end

            disp(['server version: ' num2str(serverVersion)]);

         
        end
        
        function [] = sendCommand(obj, MAJOR, MINOR, WINDOW, COMPONENT)
        %% send commands
            disp('sending commands')
            obj.output.writeShort(MAJOR);
            obj.output.writeShort(MINOR);
            
        %% send handle coordinates: window, plot component

            obj.output.writeShort(uint16(WINDOW));
            obj.output.writeShort(uint16(COMPONENT));
            obj.output.flush; 
        end
        
        function [] = sendData(obj, type, data, valueType)
 
            obj.output.writeShort(type)
            
            if (nargin < 4)
                valueType = obj.valueType(data);
            end
            
            disp(['sending data of type ' obj.valueName(valueType)]);
            obj.output.writeShort(valueType)

            dataSize = size(data);
            dimLen = length(dataSize);

            % send number of data dimensions
            obj.output.writeLong(dimLen);

            % write data dimension to the line
            for i=1:dimLen
                obj.output.writeLong(dataSize(i));
            end

            if(dimLen == 2)
               data = data'; 
            end
            
            % byte size of datatype by using the first element
            sizeof = length(typecast(data(1), 'uint8'));
            
            % number of indeces in real data, weighted by size of datatype
            snippetLen = (obj.javaBufferSize / sizeof);
            disp(['max snipped len: ' num2str(snippetLen/1024/1024*sizeof) 'MB'])
            if length(data(:)) < snippetLen
                dataBytes = typecast((data(:)), 'uint8');
                obj.output.write(dataBytes, 0, length(dataBytes));
            else
                disp(['splitting data send of ' num2str(length(data(:))*sizeof/1024/1024) ' MB']);
                for i=1:snippetLen:length(data(:))
                    
                    
                    endLen = i+snippetLen-1;
                    if(endLen > length(data(:)))
                        endLen = length(data(:));
                    end
                    disp([num2str(i) '-' num2str(endLen) ' / ' num2str(length(data(:)))]);
                    dataBytes = typecast(data(i:endLen), 'uint8');
                    obj.output.write(dataBytes(:), 0, length(dataBytes));
                    obj.output.flush;
                end
            end
            obj.output.flush;
            disp('data away');
        end
        
        function [data] = receiveData(obj, type)
            
            
            valueName = obj.valueName(type);
            dimLen = obj.input.readLong();
            dims = zeros(dimLen,1);
            
            for i=1:dimLen
                dims(i) = obj.input.readLong();
            end
            
            dataCount = prod(dims);
            
            castData = 0;
            if(valueName == 'char')
                castData = uint8('a');
            else
                castData = zeros(1, valueName);
            end
            
            sizeof = length(typecast(castData, 'uint8'));
            
            dataLen = sizeof*dataCount;
            disp(['have to read bytes: ' num2str(dataLen)])
            
            buffer = zeros(dataLen,1, 'uint8');
            %obj.input.readFully(buffer, 0, numel(buffer)-1);
            
            for b=1:dataLen
               buffer(b) = obj.input.read(); 
            end
            
            % set correct datatype and dimensions
            data = cast(buffer, valueName);
            if(length(dims)>1)
                data = reshape(castData, dims');
            end
            
        end
        
        function [] = sendMatrix(obj, matrix)
            obj.sendData(obj.data.matrix, matrix);
        end
        
        function [] = sendPoints(obj, points)
            obj.sendData(obj.data.points, points);
        end
        
        function [] = sendVolume(obj, vol)
            obj.sendData(obj.data.volume, vol);
        end
        
        function [] = sendCells(obj, data)
            obj.sendData(obj.data.cell, uint64(double(data)-1), obj.value.idtype);
        end
        
        function []  = sendColor(obj, colors)
            obj.sendData(obj.data.color_lut, colors);
        end
        
        function [] = sendOpacity(obj, opacity)
           obj.sendMatrix(opacity); 
        end
        
        function [] = sendString(obj, stringValue)
            disp('sending string')
            obj.output.writeShort(obj.data.string)
            obj.output.writeShort(obj.value.char)
            obj.output.flush;

            stringData = uint8(stringValue(:));
            stringLen = length(stringData);
            obj.output.writeLong(stringLen);
            obj.output.write(stringData, 0, stringLen);
            obj.output.flush;
            disp('string done.');
        end
        
        function [] = sendConfig(obj, config)
            if(isempty(fieldnames(config)))
                return;
            end
            
            configString = mat2json(config);
            
            obj.output.writeShort(obj.data.config);
            obj.output.writeShort(obj.value.char);
            obj.output.flush;
   
            obj.output.writeLong(length(configString));
            obj.output.write(uint8(configString(:)), 0, length(configString));
            obj.output.flush; 
        end
        
        % handles auslesen
        function [success window component] = readHandles(obj)
            
            % send data stop and flush
            obj.output.writeShort(0);
            obj.output.flush;
            
            % read return and handle values
            success = obj.input.readShort();
            window = uint16(obj.input.readUnsignedShort());
            component = uint16(obj.input.readUnsignedShort());
        end
        
        function [returnData] = readReturnValues(obj)
           
            returnData = {};
            count = 1;
            
            type = obj.input.readUnsignedShort();
            
            while ( type ~= 0)
                returnData{count} = obj.receiveData(type);
                count = count + 1;
                type = obj.input.readUnsignedShort();
            end
            
            returnData
            
        end
        
        function [] = waitForServer(obj)
            waitValue = 1;
            
            while (waitValue == 1)
                
                if ~(obj.net_socket.isConnected())
                   disp('socket not connected, breaking!');
                end
                       
                %if(~obj.input.available)
                %   disp('data input is empty')
                %   return; 
                %end
      
                disp('waiting for server to continue... (use "Continue" in Window menu)')
         
                
                try
                    obj.output.writeShort(1);
                    waitValue = obj.input.readShort();
                    pause(0.5)
                catch error
                    disp('ERROR: matvtkDataPort.waitForServer')
                    disp(error.message)
                   disp(['error waiting: ' error]);
                   break;
                end
            end
            disp('waiting is over');
            
        end
        
        function [t] = valueType(obj, data)
            
           switch class(data) 
               case 'double'
                   t = obj.value.double;
               case 'single'
                   t = obj.value.float;
               case 'int8'
                   t = obj.value.signed_char;
               case 'uint8'
                   t = obj.value.char;
               case 'int16'
                   t = obj.value.short;
               case 'uint16'
                   t = obj.value.unsigned_short;
               case 'int32'
                   t = obj.value.integer;
               case 'uint32'
                   t = obj.value.unsigned_integer;
               case 'int64'
                   t = obj.value.long;
               case 'uint64'
                   t = obj.value.unsigned_long;
               case 'char'
                   t = obj.value.string;
                   
           end
               
        end
        
        function [name] = valueName(obj, value)
               
            disp(['value: ' num2str(value)]);
            name = [];
            
            switch value
               case obj.value.double
                   name = 'double';
                   
               case obj.value.float
                   name = 'single';
                   
               case obj.value.signed_char
                   name = 'int8';
                   
               case obj.value.char
                   name = 'uint8';
                   
               case obj.value.short
                   name = 'int16';
                   
               case obj.value.unsigned_short
                   name = 'uint16';
                
               case obj.value.integer
                    name = 'int32';
               
               case obj.value.unsigned_integer
                   name = 'uint32';
               
               case obj.value.long
                    name = 'int64';
               
               case obj.value.unsigned_long
                    name = 'uint64';
               
               case obj.value.idtype
                    name = 'uint64';
                   
               case obj.value.string
                    name = 'char';
                   
            end 
            
            assert(~isempty(name));
            
        end
        
        function [] = delete(obj)
           disp('destroying DataPort')
        end
    end
end
