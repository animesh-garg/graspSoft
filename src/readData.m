function [ Mesh ] = readData( fileName)
%This file reads data using readMesh 
% from matlabmesh tools

%files are assumed to be in the pathToProject/data folder
%pwd is assumed to be pathToProject/src 
currDir = pwd;
pathToFile = [currDir(1:end-3) 'data' filesep fileName];
%srcDIR= '/src';
%pathToFile = [curr(1:regexp(curr, srcDIR)) 'data' filesep fileName];
Mesh = readMesh(pathToFile);

end

