graspSoft
=========

This is a framework for:
1. Calculating grasp points set for a given object mesh. 
2. Using submodularity of the set in each contact point, calculate the 
optimal subset which will possibly include redundant grasp points 

Current Dependencies:
Wavefront OBJ toolbox (WOBJ_toolbox_Vesion-2b)
        These are matlab files to read and write  .mtl and .obj files
        The files are included in the src with a relavant file for license in 
        the licenses folder. 

matVTK1.0
        This in turn requires a working VTK installation. 

CVX
        This version of the code runs on an academic license. 
        However CVX is not free for commercial purposes.         
        http://cvxr.com/cvx/

Notes:
One has to fire the matvtk guiserver manually once before matlab can run matvtk.
Otherwise matlab throws java.lang.xx.xx errors for not able to fire the client up.  

