% [DATAHANDLE, triangulation] = vtkplotmesh([WINHANDLE], vertices, [faces], [vertexLabels], [colors], [config, ...])
% plot polygon mesh
%
% Parameters:
% input:
% WINHANDLE 	(optional) explicit VTK handle
% vertices      a list of points (see Points)
% faces         (optional) triangulation matrix Nx3, containing the vertex indices, (1 based)
% vertexLabels 	(optional) list of labels (same length as vertices).
% colors    	(optional) a color LUT for the labels (see Color Lookup Tables)
% config    	(optional) config structure (see Configuration parameters)
% 
% output:
% triangulation triangle vertex indeces, same format as faces
%
% In the simplest call form, vtkplotmesh takes a vertex list and 
% triangulates it to a meshgrid. The result of the triangulation is 
% returned as output value.
%
% Triangulation Configuration:
% The Delaunay triangulation can be steered by to parameters in the config
% struct:
% triangulationTolerance:   control discarding of closely spaced points. This 
%       tolerance is specified as a fraction of the diagonal length of the 
%       bounding box of the points.
% triangulationAlpha:   For a non-zero alpha value, only edges, faces, or 
%       tetra contained within the circumsphere (of radius alpha) will be 
%       output.
%
% If the function is called with a faces argument, the triangulation is 
% taken from that list.
% The labels vector can be used, to assign labels to vertices. The vertices 
% are colored accordingly and color is interpolated over the polygons. When 
% no color is specified, a default jet colormap is used. Otherwise, the 
% colors for the labels are taken from the color LUT.

function [component, faceList] = vtkplotmesh(varargin)

    [window component args, config] = extractHandles(varargin);

   
    p = inputParser;
       
    p.addRequired('vertices', @ispoints);
    
    if(length(args) > 1 && size(args{2},2) == 3)
        p.addOptional('faces', [], @ispoints);
    end
    
    
    p.addOptional('vertexLabels', [], @islabels);
    
    p.addOptional('colors', [], @iscolors);
    
    p.parse(args{:});
    
       
    results = p.Results;
    
    if(~isempty(results.vertexLabels))
       
        if( ~isequallength(results.vertices, results.vertexLabels))
           error('vertices and labels must be of same length');
        end
    end
    
    

    dp = matvtkDataPort;
    dp.connect();
    dp.checkVersion();
       
    dp.sendCommand(dp.major.plot, dp.minor.mesh, window, component);
    dp.sendPoints(results.vertices);
    
    
    
    if(isfield(results, 'faces') && ~isempty(results.faces))
        dp.sendCells(results.faces);
    end
    
    if(isfield(results, 'vertexLabels') && ~isempty(results.vertexLabels))
        dp.sendMatrix(results.vertexLabels)
    end
    
    if(isfield(results, 'colors')  && ~isempty(results.colors) && iscolors(results.colors))
       dp.sendColor(results.colors); 
    end
    
    dp.sendConfig(config);
    [success window component] = dp.readHandles();
    
    [returnData] = dp.readReturnValues();
    if ~isempty(returnData)
        faceList = returnData{1}';
        faceList = uint64(double(faceList)+1);
    else
        faceList = results.faces;
    end
    
    dp.close();

end

