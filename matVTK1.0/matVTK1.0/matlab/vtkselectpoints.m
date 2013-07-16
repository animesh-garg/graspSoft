% not implemented
function [] = vtkselectpoints(nargin)

warning(['at this stage "' mfilename() '" is just a placeholder']);

end

% select 3D points
% [selectedPoints] = vtkselectpoints([HANDLE], points, [lblVolume], [config, ...])
% Parameters:
% [in] 	HANDLE 	(optional) explicit VTK handle
% [in] 	points 	Nx3 matrix containing coordinates (see Points)
% [in] 	lblVolume 	(optional) labeled volume
% [in] 	config 	(optional) config structure (see Configuration parameters)
% [out] 	selectedPoints 	logical array of selected points
% Displays points similar to vtkplotpoints() but for use as point selection tool: moving the mouse over a sphere and pressing 'p' selects the point. selectedPoints returns a logical index vector for the selected points.
% When a lblVolume is given the labeled regions correspond to the point indexes in the points argument. Selecting a point then shows the surface of the corresponding labeled region.
% 
% The default color for unselected points is blue, for selected points red.
% 
% Configuration
% The point colors can be set in the config struct, using the fields color and selectedColor. The field radius can be used to the sphere radius for the point representations.
