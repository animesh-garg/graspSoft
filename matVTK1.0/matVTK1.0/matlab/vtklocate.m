% helper script for matVTK
% returns the absolute path to the matvtk install directory (for locating
% LUTlist.mat)
function [LUTpath] = vtklocate()

    fullPath = mfilename('fullpath');
    filename = mfilename();
    len = length(filename);

    LUTpath = fullPath(1:end-len);
end