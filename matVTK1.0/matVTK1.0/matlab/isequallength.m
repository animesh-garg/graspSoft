function [value] = isequallength(varargin)

    if(nargin<2)
        error(['iseqlallength must compare 2 arguments, only ' num2str(nargin) ' given']);
    end

    value = 0;
    
    lengthDim = 1;
    
    len = size(varargin{1}, lengthDim);
    
    for i=2:nargin
        if(len ~= size(varargin{i}, lengthDim))
           return; 
        end
    end

    value = 1;
end