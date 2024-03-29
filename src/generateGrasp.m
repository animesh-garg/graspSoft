% load vertex data

% calculate normals and get wrench matrix
% coordinate transformation
% subsample to get candidate grasp points
% use brute force search on severely subsampled points to get best solution
% in hull case
% use saturate

pathtoVTK = pwd;
addpath(genpath([pathtoVTK(1:end-3) filesep 'matVTK1.0']));
%%
% [faces_f3,verts_v3,labels_v] = load_cube;
%wobj = read_wobj('~/Data/grasping/obj/m288.obj')
wobj = read_wobj('/home/animesh/work/graspSoft/data/maleHead.obj')
[verts_v3,faces_f3,normals_f3] = wobj_verts_faces(wobj);

%%




%%

% vtkdestroy;

%%
[wrenches_f6,cents_f3,normals_f3] = get_contact_wrenches3d(verts_v3,faces_f3);
F = size(wrenches_f6,1);
cand_inds = 1:ceil(F/100):F; %only some candidate indices.
%TODO: Select candidates in a more sophisticated manner

wrenches_f6 = wrenches_f6(cand_inds,:); %delete all other indices other than the required ones
wrenches_6f = wrenches_f6'; %convert to column array
cents_3f = cents_f3'; %convert to column array

%Whys the bsxfun with 0's. It seems to do nothing.
%This is where LP is being solved. 
[A_66,c_6] = min_vol_ellips(bsxfun(@plus,wrenches_6f,[0 0 0 0 0 0]'));
G_6f = A_66 * wrenches_6f;

%% Visualization

VTK = vtkinit()

vtkplotmesh(gcvtk, verts_v3, faces_f3, ...
    'representation','surface','opacity',.5,'backgroundColor',[1 1 1])

numContacts = 15;

%NOISE wrenches
t_k6 = sphere_samples(10000,6);

% [r_hull_sat_approx4,inds_hull_sat4] = inrad_hull_k_sat(G_6f',numContacts,c_6',t_k6);
% r_hull_sat_exact4 = inrad_hull(G_6f(:,inds_hull_sat4)',c_6');

[r_mink_sat_approx4,inds_mink_sat4] = inrad_mink_k_sat(G_6f',numContacts,c_6',t_k6);

pointSize = .05;
inds = cand_inds(inds_mink_sat4);
vtkplotpoints(gcvtk, cents_f3(inds,:),'pointSize',pointSize,'color',[.3 .3 .3 1]);

K = numel(inds);
linepts = zeros(2*K,3);
for i=1:K
   linepts(2*i-1,:) = cents_f3(inds(i),:);
   linepts(2*i,:) = linepts(2*i-1,:) - normals_f3(inds(i),:)*pointSize*10;
end
vtkplotlines(gcvtk, linepts,(1:2:2*K)','lineWidth',6,'color',[.3 .3   .3 1])
% for i=1:numContacts
%     pt = cents_f3(inds(i),:);
%     vtkannotate(pt,num2str(i),'textFontSize',20)
% end
