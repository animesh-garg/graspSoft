% vtkplottensors([WINHANDLE], points, tensors, [labels], [colors], [config, ...])
% plot 3x3x3 tensors as ellipsoids
%
% Parameters:
% WINHANDLE 	(optional) explicit VTK handle
% points    	Nx3 matrix containing coordinates (see Points)
% tensors   	Nx27 matrix containing the tensors at the points locations
% labels        (optional) labels for the tensors
% colors    	(optional) color lookup table (see Color Lookup Tables)
% config        (optional) config structure (see Configuration parameters)
%
% Plots ellipsoids for the tensors at the points coordinates. The 
% ellipsoids are scaled and aligned to the orientation of the eigenvectors 
% and eigenvalues. Additionally they are colored from blue to red (jet 
% colormap) according to the biggest eigenvalue.
% If a labels vector is given, the coloring is done according to the labels.
% 
% If a colormap is specified, coloring is applied with the labels and the 
% given colors.
% 
% Configuration
% scaleFactor      (double scalar) factor to be multiplied with the scaling 
%                   factor generated from the data (to reduce huge outliers)
% maxScaleFactor   (double scalar) upper scaling boundary at which scaling is clamped.
% phiResolution     number of latitude lines (edges)
% thetaResolution   number of longitude lines (edges)

% usage: vtkplottensors([VTKHANDLE, ] points, tensors [, labels, colors, config])

function [component] = vtkplottensors(varargin)

 [win, comp, args, config] = extractHandles(varargin);

    p = inputParser;
       
    p.addRequired('points', @ispoints);
    p.addRequired('tensors', @istensors);
    
    p.addOptional('labels', [], @islabels);
    p.addOptional('colors', [], @iscolors);
    
    p.parse(args{:});
    results = p.Results;
    
    if( ~isempty(results.labels) && ~isequallength(results.labels, results.points))
       error('points and labels must be of equal length');
    end

    if( ~isempty(results.colors) && isempty(results.labels) )
        error('labels must be given when using colors.')
    end
    
    dp = matvtkDataPort;
    dp.connect();
        
    dp.checkVersion();
    dp.sendCommand(dp.major.plot, dp.minor.tensor, win, comp);

    dp.sendPoints(results.points);
    dp.sendMatrix(results.tensors);
    
    if(results.labels)
        dp.sendMatrix(results.labels);
    end
    
    if(results.colors)
        dp.sendColors(results.colors);
    end
    
    dp.sendConfig(config);
    [success window component] = dp.readHandles();
    dp.close();
    
end