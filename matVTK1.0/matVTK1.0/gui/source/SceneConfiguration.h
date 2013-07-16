/* ********************************************************************
 matVTK: 3D visualization for Matlab
 see http://www.cir.meduniwien.ac.at/matvtk/
 
 If you use this software for research purposes, please cite
 the paper mentioned on the homepage in any resulting publication.  
 
 Developed at the Medical University of Vienna, Austria
 Erich Birngruber <erich.birngruber@meduniwien.ac.at>, 2009
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 see also LICENSE file or http://www.gnu.org/
 ******************************************************************** */

/** 
\page page_config Configuration parameters
For function specific parameter documentation, please look at the function documentation.<br>
For documentation of standard data types see \ref page_types.

\section General
Config parameters can be set either as struct field, and the struct itself beeing the last argument of a function call:
<pre>
config = struct();
config.color = [0.5 0.1 0.3];
config.opacity = 0.3;
vtkmyfun(param1, param2, config);
</pre>

Another way of setting parameters is the classic Matlab style of ...,'name',value,... pairs:
<pre>
vtkmyfun(param1, param2, 'color', [0.5 0.1 0.3], 'opacity', 0.3);
</pre>

A combination of both ways is also possible. In that case, if a config value is specified twice, the latest value is used:
<pre>
config = struct();
config.color = [0.5 0.1 0.3];
config.opacity = 0.3;
vtkmyfun(param1, param2, config, 'opacity', 0.7) % uses an opacity of 0.7
</pre>

Names, or config parameters that are not used by the function are silently ignored.

\section page_config_scene Scene configuration
nothing yet, but will be: camera, light settings.

\section page_config_actor Actor configuration
valid for all plots:
<table border="0">
<tr><td><b>position</b>
</td><td> [x y z] position in the scene (default [0 0 0])
</td></tr>
<tr><td><b>scale</b> 
</td><td>scaling in x,y,z directions (default [1 1 1])
</td></tr>
<tr><td><b>orientation</b>
</td><td>rotation about x, y and z axis: [rx ry rz], attention: rotation is done in order z, y, x.
</td></tr>
<tr><td><b>shading</b>
</td><td>shading off/on (0/1), default: 0
</td></tr>
<tr><td><b>ambient</b>
</td><td>ambient lighting coefficient (0..1)
</td></tr>
<tr><td><b>diffuse</b>
</td><td>diffuse lighting coefficient (0..1)
</td></tr>
<tr><td><b>specular</b> 
</td><td>specular lighting coefficient (0..1)
</td></tr>
<tr><td><b>specularPower</b>
</td><td>specular power (positive double, try 20.0)
</td></tr>
</table>


\section page_config_polydata Polygonal representations
these include points, lines, meshes, tensors, isosurface, cutplanes (basically NOT volume plots)

<table border="0">
<tr><td><b>color</b>
</td><td>set color [r g b] in 0..1 range for channels, default: [1 1 1] (white)
</td></tr>
<tr><td><b>opacity</b>
</td><td>set opacity in 0..1 range (double)
</td></tr>
<tr><td><b>edgeVisibility</b>
</td><td>0/1 display edges of polygons
</td></tr>
<tr><td><b>edgeColor</b>
</td><td>set color of edges [r g b] (0..1)
</td></tr>
<tr><td><b>lineWidth</b>
</td><td>width of line for edge representation, default: 1.0
</td></tr>
<tr><td><b>interpolation</b>
</td><td>lighting interpolation, string: "flat", "gouroud" or "phong" (default)
</td></tr>
<tr><td><b>representation</b>
</td><td>representation of surfaces: "points", "wireframe", "surface" (default)
</td></tr>
</table>

\section page_config_text Text Formatting
<table border="0">
<tr><td><b>textOpacity</b>
</td><td>same as <b>opacity</b>, for text only, 0..1 range
</td></tr>
<tr><td><b>textColor</b>
</td><td>text color as [r g b] array
</td></tr>
<tr><td><b>textFontSize</b>
</td><td>font size in points, default: 12
</td></tr>
<tr><td><b>textBold</b>
</td><td>bold text (0/1), default: 1
</td></tr>
<tr><td><b>textItalic</b>
</td><td>italic text (0/1), default: 0
</td></tr>
<tr><td><b>textShadow</b>
</td><td>display text shadow (0/1), default: 0
</td></tr>
<tr><td><b>textOrientation</b>
</td><td>text rotation angle in degrees
</td></tr>
<tr><td><b>textLineSpacing</b>
</td><td>
</td></tr>
<tr><td><b>textLineOffset</b>
</td><td>
</td></tr>
<tr><td><b>textJustification</b>
</td><td>horizontal justification relative to specified text position: "left" (default) / "centered" / "right"
</td></tr>
<tr><td><b>textVerticalJustification</b>
</td><td>vertical justification relative to specified text position: "left" (default) / "centered" / "bottom"
</td></tr>
</table>


**/
// all the rest is internal
/// \internal

#ifndef SCENE_CONFIGURATION_H
#define SCENE_CONFIGURATION_H

#include "Constants.h"
#include "StringTable.h"

#include <vtkCubeAxesActor.h>
#include <vtkCamera.h>
#include <vtkBoxRepresentation.h>
#include <vtkDoubleArray.h>
#include <vtkPlanes.h>
#include <vtkPoints.h>
#include <vtkDataArray.h>
#include <json.h>

#include "matvtkProtocol.h"

#include "ConfigurationValue.h"

#ifdef WIN32
#define isnan _isnan
#endif

class vtkCamera;
class vtkVolumeProperty;
class vtkProperty;
class vtkProp3D;
class vtkActor;
class vtkActor2D;
class vtkVolume;
class vtkTextProperty;
class vtkTextActor;
class vtkRenderer;
class vtkSphereSource;
class vtkImplicitPlaneWidget2;
class vtkBoxWidget2;
class vtkFixedPointVolumeRayCastMapper;
class vtkGPUVolumeRayCastMapper;



typedef std::map<std::string, const ConfigurationValue*> ConfigMap;


class SceneConfiguration {

public:

    const ConfigurationValue* GetField(std::string name) {
        const ConfigurationValue *data = NULL;
        if(values.find(name) != values.end()) data = values[name];
        return data;
    }
    
    void SetField(std::string key, ConfigurationValue* value) {
        if(value != NULL)
            values[key] = value;
        
        return;
	}
    

    bool HasField(std::string name) {
        return (GetField(name) != NULL);
	}
    
    
    bool IsScalar(std::string name) {
        return (HasField(name) && GetField(name)->IsScalar());
    }
    
    bool IsPointer(std::string name, int len=3) {
        return (HasField(name) && GetField(name)->IsMatrix(len));
    }
    
    bool IsString(std::string name) {
        return (HasField(name) && GetField(name)->IsString());
    }
    
    bool IsOn(std::string name) {
        return (IsScalar(name) && GetScalar(name) == 1.0);
    }

    bool IsOff(std::string name) {
        return (IsScalar(name) && GetScalar(name) == 0.0);
    }

    
    double GetScalar(std::string name) {
        const ConfigurationValue *value = GetField(name);
        if(value) return value->value_scalar;

        return -1.0;
    }
    
    double* GetPointer(std::string name) {
        const ConfigurationValue *value = GetField(name);
        if(value) return value->value_matrix;
        
        return NULL;
    }
    
    void Copy(SceneConfiguration *config);
    
    int GetStringValue(std::string name);
    
  	
    std::string Serialize();
    void Deserialize(std::string);

    protected:
    ConfigMap values;   


};



#endif

