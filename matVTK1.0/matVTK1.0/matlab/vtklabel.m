% HANDLE = vtklabel([WINHANDLE], points, [labels | strings], [config, ...])
% 
% draw multiple labels at data points
% 
% Parameters:
% HANDLE 	(optional) explicit VTK handle
% points 	Nx3 matrix containing coordinates (see Points)
% labels 	(optional) vector/matrix of labels
% strings 	(optional) string cell of text labels
% config 	(optional) config structure (see Configuration parameters)
% 
% If points is the only data argument, the points are labeled "id: X" where X corresponds to the Matlab line index of the point in the points matrix.
% Both, labels and strings must have the same number of rows as points. When specifiying the labels vector/matrix, the rows of the matrix are used as labels and printed in vector notation: "(x1, x2, x3, ... xn)" i.e. vtklabel(points, points) plots the coordinate values of the points array (at the corresponding position).
% 
% string is a cell vector of strings, that are used to label the data points.
% 
% Configuration:
% labelClosestN (double) show labels only for N points closest to view plane. default: show all labels (value: 0)
% 

function [] = vtklabel(varargin)

    [win, comp, args, config] = extractHandles(varargin);

    p = inputParser;
    
    
    p.addRequired('positions', @ispoints);
    p.addOptional('label', []);
    %p.addOptional('numLabel', [], @iscell);
    
    
    
    
    
    p.parse(args{:});
    results = p.Results;
    
    
    if ~(isnumeric(results.label) || iscell(results.label))
        error('labels must be matrix or cell of strings');
    end
    
    if( ~isempty(results.label) && ~isequallength(results.label, results.positions))
        error('labels and positions must be of equal length');
    end
    
%    if( ~isempty(results.numLabel) && ~isequallength(results.numLabel, results.positions))
%        error('labels and positions must be of equal length');
%    end
    
    

    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.special, dp.minor.labels, win, comp);

    dp.sendPoints(results.positions);
    
    if ~isempty(results.label) && isnumeric(results.label)
        dp.sendMatrix(results.label)
    end
    
    if iscell(results.label)
        dp.sendString(mat2json(results.label))
    end
    
    dp.sendConfig(config);
    dp.close();

end