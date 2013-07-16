function y_mp = hullpts(x_np)
%% hullpts.m
% given an input x_np
% calculates convex hull using builtin "convhulln" function
% Variable 'facets' conatins the indices of points in x_np which are on the 
% convex hull
% y_mp is a list of unique points in x_np lying on convex hull


facets = convhulln(x_np);
inds = unique(facets);
y_mp = x_np(inds,:);
end