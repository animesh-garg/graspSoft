% DATAHANDLE = vtkplotlines([WINHANDLE], points, [startIdx], [idxList], [config, ...])
% 
% plot a single or multiple lines
% 
% Parameters:
% [in] 	WINHANDLE 	(optional) explicit VTK handle
% [in] 	points 	Nx3 matrix containing coordinates (see Points)
% [in] 	idxStart 	vector of start indexes
% [in] 	idxList 	list of indeces
% [in] 	config 	(optional) config structure (see Configuration parameters)
% vtkplotlines is able to plot lines in three different ways: In the 
% simplest form, when only the points list is used as argument, a single 
% line following all points in the given order.
% 
% Indexing
% If additionally the idxStart parameter is listed, multiple lines can be 
% plotted. The idxStart values are used to directly index the point list.
% pointlist = [x1 y1 z1;	% this is p1 ...
%              x2 y2 z2;		
%              x3 y3 z3;		
%              x4 y4 z4;		
%              x5 y5 z6];
% idxStart = [1 4];
% vtkplotpoints(pointlist, idxStart);	
% This would plot 2 lines, one consisting of the points {p1, p2, p3} and 
% one with the points {p4, p5}. The idxStart list cannot be longer than 
% length(pointlist/2), as each line has to have at least 2 points.
% 
% When callint vtkplotlines with all three arguments, points, idxStart and 
% idxList, the indexing is done the following way: idxStart now references 
% the start index in the index list. The index list itself references the 
% point list. This allows to reuse point coordinates. Reusing the pointlist 
% variable defined above, it looks like this:
% 
% idxStart = [1 3 5];
% idxList = [1 4 3 2 5 1];
% vtkplotpoints(pointlist, idxStart, idxList);
% producing 3 lines with the points: {p1, p4}, {p3, p2} and {p5, p1}. In 
% this case the idxStart cannot be longer than the idxList.
% 
% Tubes
% In normal plotting mode, plain lines are drawn. When setting the config 
% struct field lineTubes to 1, polygonal tubes are drawn instead. The tubes 
% are shaded like any other polygonal surfaces.
% 
% Configuration
% Aside from the default configuration parameters for a scene component, 
% vtkplotlines supports these special parameters: 
% lineWidth     sets the line width, defaults to 1.0
% lineTubes     0=draw lines, 1=draw tubes. defaults to 0.
% tubeSides     number of sides for the tube, defaults to 6 (hexagonal surface) 
% tubeRadius    tube thickness, defaults to 1.
%
%

function [component] = vtkplotlines(varargin)
    
    [window component args, config] = extractHandles(varargin);

   
    p = inputParser;
       
    p.addRequired('points', @ispoints);
    
    p.addOptional('startIdx', [], @iscolumnvector);
    p.addOptional('idxList', [], @iscolumnvector);
    
    p.addOptional('pointLabels', [], @islabels);
    p.addOptional('colorLUT', [], @iscolors);
    
    
    p.parse(args{:});
    
    results = p.Results;
    

    pointNum = size(results.points, 1);
    
    if(numel(results.startIdx) == pointNum)
       results.pointLabels = results.startIdx;
       results.startIdx = [1];
       results.idxList = [1:pointNum];
    end
    
     if(numel(results.idxList) == pointNum && isempty(results.pointLabels))
       results.pointLabels = results.idxList;
       results.idxList = [];
    end
    
    dp = matvtkDataPort;
    dp.connect();
    dp.checkVersion();
       
    dp.sendCommand(dp.major.plot, dp.minor.line, window, component);
    dp.sendPoints(results.points);
    
    
    if ~isempty(results.startIdx)
       dp.sendCells(results.startIdx);
       if ~isempty(results.idxList)
          dp.sendCells(results.idxList);
       end
    end
    
    
    
    if ~isempty(results.pointLabels)
       dp.sendMatrix(results.pointLabels);
       if ~isempty(results.colorLUT)
           dp.sendMaitrx(results.colorLUT);
       end
    end
    
    dp.sendConfig(config)
    [success window component] = dp.readHandles();
    dp.close();
    
end