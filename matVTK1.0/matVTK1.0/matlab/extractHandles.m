% helper function for matVTK
%
% parse window and component handles
% detect data arguments and additional config parameters
function [window, component, dataArgs, config] = extractHandles(argin)

nargin = length(argin);


startIdx = 1;
endIdx = nargin;

window = 0;
component = 0;
dataArgs = {};
config = struct();

if(nargin == 0)
    return;
end

% read handles (if any)
if(nargin > 0 && ismatvtkhandle(argin{1}))
    component = argin{1};
    startIdx = startIdx +1;
end

if(nargin > 1 && ismatvtkhandle(argin{2}))
    window = argin{2};
    startIdx = startIdx +1;
end



% config struct or key/value list?

% read key value pairs from argument list as config values
[offset config] = parseConfig(argin(startIdx:end));
config
endIdx = startIdx+offset-1;


% the rest is the real data
%dataArgs = cell(1, (endIdx-startIdx));
dataArgs = argin(startIdx:endIdx);
    
end


function [idx config] = parseConfig(args)

    len = length(args);
    structIdx = len;
    
    frontIdx = 1;
    idx = len;
    config = struct();

    structPos = cellfun(@(x) isa(x, 'struct'), args);
    if(any(structPos))
       structIdx = find(structPos);
       frontIdx = structIdx(end);
       config = args{frontIdx};
       structIdx = structIdx-1;
    else
        
    end
    
    for i=len:-2:frontIdx+1
        key = args{i-1};
        value = args{i};
        if isa(key, 'char')
           config.(key) = value;
           idx = idx - 2;
        else
            disp('no more config to read');
            break;
        end
    end
    
    
    
    idx = min(idx, structIdx);
    
end