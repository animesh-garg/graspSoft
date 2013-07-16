
disp('matVTK demo script')
clear


addpath(pwd)
addpath([pwd filesep 'data'])
%load testState

% OBSOLETE: done in cmake
% compile mex files
%makeMex

%% %% start demo: loading data
% LUTlist = 
% 
%        AirwaysII: [1x1 struct]
%          Airways: [1x1 struct]
%       BoneSkinII: [1x1 struct]
%         BoneSkin: [1x1 struct]
%         DarkBone: [1x1 struct]
%         GoldBone: [1x1 struct]
%     SkinOnBlueII: [1x1 struct]
%       SkinOnBlue: [1x1 struct]
%       SoftSkinII: [1x1 struct]
%      SoftSkinIII: [1x1 struct]
%         SoftSkin: [1x1 struct]
%       YellowBone: [1x1 struct]

load LUTlist
usedLUT = 'DarkBone';

%opacityLUT = LUTlist.(usedLUT).opacityLUT;
%opacityLUT(:,2) = opacityLUT(:,2).*0.2;

%% plot volume data
load volume
volume = single(volume);
aspect = [1 1 1];


%% plot vertex data
load faceVertex
faces = double(T);

vz = max(P(:,3));
vertices = double([P(:,2) P(:,1) vz-P(:,3)]);
%vertices = vertices(unique(faces(:)),:);


numVerts = size(vertices,1 );
labels = 1:numVerts;
labels(1:500) = 1;
labels(501:1000) = 2;
labels(1001:1500) = 3;
labels(1501:2000) = 4;

pointColors = [1, 1, 0, 0;
               10, 0, 1, 0;
               50, 0, 0, 1;
               100, 1, 1, 1];
           
meshOpacity = 1;


%% plot points data
nPoints = 100;
[xmax ymax zmax] = size(volume);

points = [randint(1,nPoints,xmax); randint(1,nPoints,ymax); randint(1,nPoints,zmax)]';

points = [points(:,2)'; points(:,1)'; points(:,3)']';
points = points .* repmat(aspect, [length(points), 1, 1]);

pointLabels2 = linspace(1, 100, size(points, 1));
%pointLabels(:) = 1;

pointSize = 1.25;

pointColors2 = [1, 1, 1, 1;
                length(pointLabels2)/2, 1, 1, 0.1;
                length(pointLabels2), 1, 0, 0];

%% iso contour value:

% can be a single value, or multiples:
isoValue = [400 1200 0];
isoColor = [0.5 0.9 0.5];

%% lines plotting
load LinesDemo
MyLineSet(:,3) = MyLineSet(:,3)-8;
MyLineSet = MyLineSet.*5;
linePos = mean(MyLineSet);

%% load tensor data
load tensors

% load vector data
load vectors

%% =========================
%clear mex
%vtkdestroy
% create VTK manager handle
%VTK = vtkinit();

vtkinit

pointPlot = vtkplotpoints(rand(10,3).*10);
%meshPlot = vtkplotmesh(vertices, faces, (labels.*25)', pointColors)
% volume, colors, opacities, ...

volumePlot = vtkplotvolume(volume, LUTlist.(usedLUT).colorLUT, LUTlist.(usedLUT).opacityLUT)
vtkcrop(volumePlot)

vtkgrid
isosurfPlot = vtkisosurface(volume, isoValue');
%meshPlot = vtkplotmesh(vertices, faces, labels', pointColors)
%quiverPlot = vtkquiver(vectorPoints, Vectors, 'phiResolution', 10, 'thetaResolution', 10)

vtkwait
vtkdestroy()



%%


% create VTK manager handle
VTK = vtkinit();

% show isosurface in volume
vtkisosurface(VTK, volume, isoValue');

% mesh: vertices, faces, labels, color LUT, total opacity
vtkplotmesh(gcvtk, vertices, faces, labels', pointColors)

% plot point cloud
vtkplotpoints(gcvtk, points, pointLabels2', pointColors2)

% plot lines
vtkplotlines(MyLineSet, MyLineStarts)

%plot tensors
vtkplottensors(tensorPoints, Tensors)

% quiver plot vectors
vtkquiver(vectorPoints, Vectors)

% place text at 3d position (Origin)
%vtkannotate(linePos, 'Lines');

vtklegend(LUTlist.(usedLUT).colorLUT)

% set azimuth, elevation of view
vtkview(-60, 30)

% display orientation widget in interactive mode
vtkorientation();

% write title on top
vtktitle('matVTK: interactive mode');

% display interactive window
vtkwait();

% % % set new title
% % vtktitle('matVTK: scene saved to file');
% % 
% % % save scene to png image
% % disp('writing scene to file.')
% % vtksave('vtk_save_test.png');
% % 
% % % delete title
% % vtktitle('');

% shutdown vtk, free memory
vtkdestroy();
