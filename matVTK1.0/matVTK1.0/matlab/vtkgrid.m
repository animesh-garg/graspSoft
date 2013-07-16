% vtkgrid([DATAHANDLE] [, config, ...])
%
% display axis grid to help with spatial orientation
% no optional or mandatory arguments, only configuration values.
% 
% Configuration:
% The grid consists of polygonial data, therefore the respective section of 
% Configuration parameters apply. Additionally these fields are valid:
%
% gridFly                       this parameter controls if and how the grid 
%                               axes are switched, when rotating the scene, 
%                               values: "outerEdges" (default), "closestTriad", 
%                               "furthestTriad", "staticTriad", "staticEdges".
% gridTickLocation              "inside", "outside" or "both" (default)
% grid[XYZ]AxisLabelVisibility	0/1 display "x"/"y"/"z" label for the respective axis, default: 1
% grid[XYZ]AxisTickVisibility	0/1 show major ticks, default: 1
% grid[XYZ]AxisMinorTickVisibility	0/1 show minor thick, default: 1
% gridDraw[XYZ]Gridlines        0/1 enable display of grid lines, default: 0
% grid[XYZ]AxisVisibility       0/1 hide/show complete coordinate axis, default: 1
% gridInertia                   parameter for volatility of switching axes
% gridCornerOffset              double value (default: 0.0) axis offset 
%                               from the origin - or respective corner, 
%                               where two or three axes are joined.

function [component] = vtkgrid(varargin)


    [win, comp, args, config] = extractHandles(varargin);

    if(length(args) > 0)
       error('no arguments allowed.');
    end
    
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.special, dp.minor.grid, win, comp);

    
    dp.sendConfig(config);
    [success window component] = dp.readHandles();
    
    dp.close();

end