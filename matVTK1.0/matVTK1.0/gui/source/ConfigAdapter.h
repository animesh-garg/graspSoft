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

#ifndef STRUCT_ADAPTER_H
#define STRUCT_ADAPTER_H

#include <typeinfo>

#include <Constants.h>
#include <SceneConfiguration.h>
#include <PropertyEntry.h>

// for the implementations

#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkVolumeProperty.h>
#include <vtkCamera.h>
#include <vtkCubeAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkBoxWidget2.h>
#include <vtkBoxRepresentation.h>
#include <vtkActor2D.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkFixedPointVolumeRayCastMapper.h>

#include <vtkVolumeProperty.h>
#include <vtkTextProperty.h>
#include <vtkTextActor.h>
#include <vtkCubeAxesActor.h>
#include <vtkCamera.h>
#include <vtkSphereSource.h>

#include <vtkRenderWindow.h>


// end implementation headers

//
// config adapter base class
//


class ConfigAdapterBase {
  
public:
    ConfigAdapterBase(string name) : _adapterName(name) { }
    virtual void Read(SceneConfiguration *config) { config = NULL; }
    virtual void Write(SceneConfiguration *config) { config = NULL; }
    
    virtual string adapterName() { return _adapterName; }
    virtual std::list<string> names() { return std::list<string>(); }
    
protected:
    string _adapterName;
    
};


template<typename T>
class ConfigAdapter : public ConfigAdapterBase {
  public: 
      
    
    ConfigAdapter(T* property, string name="generic config") : ConfigAdapterBase(name) {
        this->property = property;
        //qDebug() << "constructor for " << typeid(this).name();
        this->BuildList();
    }
    
    
    // set config values to vtk objects
    void Read(SceneConfiguration *config) {
        
        // apply defaults first, then set provided config vaules
        this->ApplyDefaults();
        
        typename std::list<PropertyEntryBase<T>*>::iterator it;
        
        for(it=entries.begin(); it!=entries.end(); ++it) {
            
            const ConfigurationValue *data = config->GetField((*it)->GetName());
            std::string name = (*it)->GetName();

            // set field value if available in array
            if(data != NULL) {
                // Debug("Setting value from config: %s", name.c_str());
                (* it)->SetValue(property, data);
            }
        }
    }

    
    // extract config values from vtk objects
    void Write(SceneConfiguration *config) {
        
        typename std::list<PropertyEntryBase<T>*>::iterator it;
        for(it=entries.begin(); it!=entries.end(); ++it) {
            PropertyEntryBase<T> *propertyEntry = (*it);

			// this way 'data' below should never be NULL
            if(!config->HasField(propertyEntry->GetName())) {
                //qDebug() << "creating value for " << propertyEntry->GetName().c_str();
                ConfigurationValue *configValue = new ConfigurationValue();
                config->SetField(propertyEntry->GetName(), configValue);
			}
			
            //qDebug() << "exporting property: " << propertyEntry->GetName().c_str();
			const ConfigurationValue *data = config->GetField(propertyEntry->GetName());
            propertyEntry->GetValue(property, const_cast<ConfigurationValue*>(data));
        }
    }

    inline void ApplyDefaults() {
        //Debug("default implementation of apply defaults is empty.");
    }
    
    virtual ~ConfigAdapter() {
        typename std::list<PropertyEntryBase<T> *>::iterator it;
        
        // delete entries
        for(it=entries.begin(); it!=entries.end(); ++it) {
            delete (*it);
            //Debug("memory management is BAD! must do something here!");
        }
    }

    std::list<string> names() {
        
        std::list<string> strList;
        
        typename std::list<PropertyEntryBase<T>*>::iterator it;
        for(it=entries.begin(); it!=entries.end(); ++it) {
            PropertyEntryBase<T> *propertyEntry = (*it);
            string name = propertyEntry->GetName();
            strList.push_back(name);
        }
        
        return strList;
    }

  protected:
      inline virtual void BuildList() {
          qDebug() << "WARNING: default BuildList for type " << typeid(property).name() << " not implemented.";
      }
    

      T* property;
      std::list<PropertyEntryBase<T> *> entries;
};


// call correct overloaded functions
class CameraWrapper {
    
public:
    CameraWrapper(vtkCamera *camera) : camera(camera) {
	}
    
    double* GetFocalPoint() { return camera->GetFocalPoint(); }
    void SetFocalPoint(double *fp) { camera->SetFocalPoint(fp); }
    
    void SetPosition(double *d) {  camera->SetPosition(d);     }
    double *GetPosition() { return camera->GetPosition(); }
    
    void SetViewUp(double *d) { camera->SetViewUp(d); }
    double* GetViewUp() { return camera->GetViewUp(); }
    
    void SetFocalDisk(double ds) { camera->SetFocalDisk(ds); }
    double GetFocalDisk() { return camera->GetFocalDisk(); }
    
private:
    vtkCamera *camera;
};



class BoxRepresentationWrapper {
    
public:
    BoxRepresentationWrapper(vtkBoxRepresentation *box) {
        this->box = box;
        
        planes = vtkPlanes::New();
        box->GetPlanes(planes);
    }
    
    ~BoxRepresentationWrapper() {
        planes->Delete();
    }
    
    void SetNormals(double *d) {
        int len=6;
        vtkDoubleArray *n = vtkDoubleArray::New();
        n->SetNumberOfComponents(3);
        n->Allocate(len);
        
        for(int i=0; i<len; i++) {
            n->InsertTuple3(i, d[i], d[len+i], d[2*len+i]);
            Debug("setting normal: %f, %f, %f\n", d[i], d[len+i], d[2*len+i]);
            
        }
        
        planes->SetNormals(n);
        n->Delete();
        
        // this way planes are always normal to x/y/z axis :-(
        //box->SetPlaceFactor(1.0);
        //box->PlaceWidget(b);
        //box->BuildRepresentation();
    }
    
    void SetPoints(double *d) {
        int len=6;
        vtkDoubleArray *n = vtkDoubleArray::New();
        n->SetNumberOfComponents(3);
        n->Allocate(len);
        
        for(int i=0; i<len; i++) {
            n->InsertTuple3(i, d[i], d[len+i], d[2*len+i]);
        }
        
        planes->GetPoints()->SetData(n);
        n->Delete();
        
        // this way planes are always normal to x/y/z axis :-(
        box->SetPlaceFactor(1.0);
        box->PlaceWidget(planes->GetPoints()->GetBounds());
        //box->BuildRepresentation();
    }
    
    double* GetNormals() {
        //return planes->GetNormals()->GetPointer(0);
        
        int c=6;
        for(int i=0; i<c; i++) {
            double *xyz = planes->GetNormals()->GetTuple3(i);
            Debug("reading normal: %f, %f, %f\n", xyz[0], xyz[1], xyz[2]);
            normals[i] = xyz[0];
            normals[c+i] = xyz[1];
            normals[2*c+i] = xyz[2];
        }
        
        return normals;
        
        
    }
    
    double* GetPoints() {
        
        int c=6;
        for(int i=0; i<c; i++) {
            double *xyz = planes->GetPoints()->GetPoint(i);
            points[i] = xyz[0];
            points[c+i] = xyz[1];
            points[2*c+i] = xyz[2];
        }
        
        return points;
        //planes->GetPoints()->GetData()->GetPointer(0);
    }
    
    
private:
    double points[18];
    double normals[18];
    vtkPlanes* planes;
    vtkBoxRepresentation *box;
    
};





#pragma mark template implementations

template<>
inline void ConfigAdapter<vtkVolumeProperty>::ApplyDefaults() {
    property->SetInterpolationTypeToLinear();
}

template<>
inline void ConfigAdapter<vtkTextProperty>::ApplyDefaults() {
    property->ShadowOff();
    property->BoldOn();
    property->ItalicOff();
    property->SetFontSize(12);
    property->SetFontFamilyToArial();
}

template<>
inline void ConfigAdapter<vtkTextActor>::ApplyDefaults() {
    property->SetTextScaleModeToNone();
}





/*
 template<>
 void ConfigAdapter::ApplyDefaults<vtkBoxWidget2>() {
 Debug("configuring box widget");
 
 ConfigAdapter<vtkBoxWidget2> cab(box);
 cab.Read(this);
 
 
 //const char* typeName = typeid(* box->GetRepresentation()).name();
 //Debug("box representation: %s", typeName);
 
 vtkBoxRepresentation* rep = reinterpret_cast<vtkBoxRepresentation*>(box->GetRepresentation());
 
 //vtkBoxRepresentation* rep = (vtkBoxRepresentation*) (box->GetRepresentation());
 BoxRepresentationWrapper boxWrap(rep);
 ConfigAdapter<BoxRepresentationWrapper> boxRep(&boxWrap);
 boxRep.Read(this);
 }
 */

#define VALUE(prop, type , name, func) \
entries.push_back(new PropertyEntry<prop, type>(name, SCALAR, &prop::Set##func, &prop::Get##func))

#define VALUE_INT(prop, type , name, func) \
entries.push_back(new PropertyEntry<prop, type>(name, INTEGER, &prop::Set##func, &prop::Get##func))

#define VALUE_RANGE(prop, type , name, func) \
entries.push_back(new PropertyEntry<prop, type>(name, RANGE, &prop::Set##func, &prop::Get##func))


#define FLAG(prop, type , name, func) \
entries.push_back(new PropertyEntry<prop, type>(name, FLAG, &prop::Set##func, &prop::Get##func))


#define VECTOR3(prop, type , name, func) \
entries.push_back(new PropertyEntry<prop, type>(name, TRIPLET, &prop::Set##func , &prop::Get##func))

#define COLOR(prop, type , name, func) \
entries.push_back(new PropertyEntry<prop, type>(name, COLOR, &prop::Set##func , &prop::Get##func))


#define STRING(prop, type , name, func) \
entries.push_back(new PropertyEntry<prop, type>(name, STRING, &prop::Set##func, &prop::Get##func))

/*
 #define VECTOR(prop, type , name, func, len) \
 entries.push_back(new PropertyEntry<prop, type>(name, &prop::Set##func, &prop::Get##func, 1, len))
 
 #define MATRIX(prop, type , name, func, rows, cols) \
 entries.push_back(new PropertyEntry<prop, type>(name, &prop::Set##func, &prop::Get##func, rows, cols))
 */




template<>
inline void ConfigAdapter<vtkProperty>::BuildList() {
    
    VALUE_RANGE(vtkProperty, double, "opacity", Opacity);
    COLOR(vtkProperty, double*, "color", Color);
    
    FLAG(vtkProperty, int, "edgeVisibility", EdgeVisibility);
    COLOR(vtkProperty, double*, "edgeColor", EdgeColor);
    
    VALUE(vtkProperty, float, "lineWidth", LineWidth);
    
    STRING(vtkProperty, int, "interpolation", Interpolation);
    STRING(vtkProperty, int, "representation", Representation);
    
    FLAG(vtkProperty, int, "shading", Shading);
    
    VALUE_RANGE(vtkProperty, double, "ambient", Ambient);
    VALUE_RANGE(vtkProperty, double, "diffuse", Diffuse);
    VALUE_RANGE(vtkProperty, double, "specular", Specular);
    VALUE(vtkProperty, double, "specularPower", SpecularPower);
    
    COLOR(vtkProperty, double*, "ambientColor", AmbientColor);
    COLOR(vtkProperty, double*, "diffuseColor", DiffuseColor);
    COLOR(vtkProperty, double*, "specularColor", SpecularColor);
}

template<>
inline void ConfigAdapter<vtkVolumeProperty>::BuildList() {
    // this is in vtkImageData ...
    
    STRING(vtkVolumeProperty, int, "volumeInterpolation", InterpolationType);
    FLAG(vtkVolumeProperty, int, "shading", Shade);
    
    VALUE_RANGE(vtkVolumeProperty, double, "ambient", Ambient);
    VALUE_RANGE(vtkVolumeProperty, double, "diffuse", Diffuse);
    VALUE_RANGE(vtkVolumeProperty, double, "specular", Specular);
    VALUE(vtkVolumeProperty, double, "specularPower", SpecularPower);
}

template<>
inline void ConfigAdapter<vtkProp3D>::BuildList() {
    VECTOR3(vtkProp3D, double*, "position", Position);
    VECTOR3(vtkProp3D, double*, "scale", Scale);
    VECTOR3(vtkProp3D, double*, "orientation", Orientation);
    VECTOR3(vtkProp3D, double*, "origin", Origin);
}

template<>
inline void ConfigAdapter<vtkFixedPointVolumeRayCastMapper>::BuildList() {
    VALUE(vtkFixedPointVolumeRayCastMapper, float, "sampleDistance", SampleDistance);
}

template<>
inline void ConfigAdapter<vtkGPUVolumeRayCastMapper>::BuildList() {
    VALUE(vtkGPUVolumeRayCastMapper, float, "sampleDistance", SampleDistance);
}


template<>
inline void ConfigAdapter<CameraWrapper>::BuildList() {
    
    VECTOR3(CameraWrapper, double*, "cameraFocalPoint", FocalPoint);
    VECTOR3(CameraWrapper, double*, "cameraPosition", Position);
    VECTOR3(CameraWrapper, double*, "cameraViewUp", ViewUp);
    VALUE(CameraWrapper, double, "cameraFocalDisk", FocalDisk);
}

template<>
inline void ConfigAdapter<vtkCamera>::BuildList() {
    
    VALUE(vtkCamera, double, "cameraRoll", Roll);
    VALUE(vtkCamera, double, "cameraFocalDisk", FocalDisk);
    VALUE(vtkCamera, double, "cameraViewAngle", ViewAngle);
    STRING(vtkCamera, int, "cameraProjection", ParallelProjection);
}

template<>
inline void ConfigAdapter<vtkCubeAxesActor>::BuildList() {
    //Debug("configuring CubeAxes.\n");
    
    STRING(vtkCubeAxesActor, int, "gridFly", FlyMode);
    STRING(vtkCubeAxesActor, int, "gridTickLocation", TickLocation);
    
    FLAG(vtkCubeAxesActor, int, "gridXAxisLabelVisibility",  XAxisLabelVisibility);
    FLAG(vtkCubeAxesActor, int, "gridYAxisLabelVisibility",  YAxisLabelVisibility);
    FLAG(vtkCubeAxesActor, int, "gridZAxisLabelVisibility",  ZAxisLabelVisibility);
    
    FLAG(vtkCubeAxesActor, int, "gridXAxisTickVisibility",  XAxisTickVisibility);
    FLAG(vtkCubeAxesActor, int, "gridYAxisTickVisibility",  YAxisTickVisibility);
    FLAG(vtkCubeAxesActor, int, "gridZAxisTickVisibility",  ZAxisTickVisibility);
    
    FLAG(vtkCubeAxesActor, int, "gridXAxisMinorTickVisibility",  XAxisMinorTickVisibility);
    FLAG(vtkCubeAxesActor, int, "gridYAxisMinorTickVisibility",  YAxisMinorTickVisibility);
    FLAG(vtkCubeAxesActor, int, "gridZAxisMinorTickVisibility",  ZAxisMinorTickVisibility);
    
    FLAG(vtkCubeAxesActor, int, "gridDrawXGridlines",  DrawXGridlines);
    FLAG(vtkCubeAxesActor, int, "gridDrawYGridlines",  DrawYGridlines);
    FLAG(vtkCubeAxesActor, int, "gridDrawZGridlines",  DrawZGridlines);
    
    FLAG(vtkCubeAxesActor, int, "gridXAxisVisibility", XAxisVisibility);
    FLAG(vtkCubeAxesActor, int, "gridYAxisVisibility", YAxisVisibility);
    FLAG(vtkCubeAxesActor, int, "gridZAxisVisibility", ZAxisVisibility);
    
    FLAG(vtkCubeAxesActor, int, "gridInertia", Inertia);
    VALUE(vtkCubeAxesActor, double, "gridCornerOffset", CornerOffset);
}

template<>
inline void ConfigAdapter<vtkTextProperty>::BuildList() {
    VALUE(vtkTextProperty, double, "textOpacity", Opacity);
    VECTOR3(vtkTextProperty, double*, "textColor", Color);
    
    VALUE(vtkTextProperty, int, "textFontSize", FontSize);
    FLAG(vtkTextProperty, int, "textBold", Bold);
    FLAG(vtkTextProperty, int, "textItalic", Italic);
    VALUE(vtkTextProperty, int, "textShadow", Shadow);
    
    VALUE(vtkTextProperty, double, "textOrientation", Orientation);
    VALUE(vtkTextProperty, double, "textLineSpacing", LineSpacing);
    VALUE(vtkTextProperty, double, "textLineOffset", LineOffset);
    
    STRING(vtkTextProperty, int, "textJustification", Justification);
    STRING(vtkTextProperty, int, "textVerticalJustification", VerticalJustification);
}

template<>
inline void ConfigAdapter<vtkTextActor>::BuildList() {
    
}

/*
 template<>
 void ConfigAdapter<vtkActor2D>::BuildList() {
 
 }
 */

template<>
inline void ConfigAdapter<vtkRenderer>::BuildList() {
    FLAG(vtkRenderer, int, "twoSidedLighting", TwoSidedLighting);
    FLAG(vtkRenderer, int, "lightFollowCamera", LightFollowCamera);
    COLOR(vtkRenderer, double*, "backgroundColor", Background);
}

template<>
inline void ConfigAdapter<vtkRenderWindow>::BuildList() {
    //FLAG(vtkRenderWindow, int, "pointSmoothing", PointSmoothing);
    //FLAG(vtkRenderWindow, int, "lineSmoothing", LineSmoothing);
    //FLAG(vtkRenderWindow, int, "polygonSmoothing", PolygonSmoothing);
}


template<>
inline void ConfigAdapter<vtkSphereSource>::BuildList() {
    VALUE_INT(vtkSphereSource, int, "phiResolution", PhiResolution);
    VALUE_INT(vtkSphereSource, int, "thetaResolution", ThetaResolution);
    VALUE(vtkSphereSource, double, "pointSize", Radius);
}

template<>
inline void ConfigAdapter<vtkImplicitPlaneWidget2>::BuildList() {
    
}

template<>
inline void ConfigAdapter<vtkImplicitPlaneRepresentation>::BuildList() {
    VECTOR3(vtkImplicitPlaneRepresentation, double*, "cropPlaneOrigin", Origin);
    VECTOR3(vtkImplicitPlaneRepresentation, double*, "cropPlaneNormal", Normal);
}



template<>
inline void ConfigAdapter<vtkBoxWidget2>::BuildList() {
    FLAG(vtkBoxWidget2, int, "cropBoxTranslation", TranslationEnabled);
    FLAG(vtkBoxWidget2, int, "cropBoxScaling", ScalingEnabled);
    FLAG(vtkBoxWidget2, int, "cropBoxRotation", RotationEnabled);
}

///TODO: fix Matrix config adapter / PropertyEntry
template<>
inline void ConfigAdapter<BoxRepresentationWrapper>::BuildList() {
    //MATRIX(BoxRepresentationWrapper, double*, "cropBoxNormals", Normals, 6, 3);
    //MATRIX(BoxRepresentationWrapper, double*, "cropBoxPoints", Points, 6, 3);
}

#endif
