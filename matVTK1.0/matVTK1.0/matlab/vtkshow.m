% Deprecated.
% use vtkwait([WINHANDLE]) instead.

function [] = vtkshow(varargin)
    
    warning('vtkshow() is deprecated. use vtkwait() to pause Matlab execution during interaction with matVTK figures.');
    vtkwait(varargin{:})
    warning('vtkshow() is deprecated. use vtkwait() to pause Matlab execution during interaction with matVTK figures.');

end