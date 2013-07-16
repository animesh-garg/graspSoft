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

#include "SceneConfiguration.h"

#include <vtkTextProperty.h>
#include <vtkTextActor.h>
#include <vtkActor2D.h>
#include <vtkVolumeProperty.h>
#include <vtkSphereSource.h>
#include <vtkBoxWidget2.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkImplicitPlaneRepresentation.h>






int SceneConfiguration::GetStringValue(std::string name) {
       
    const ConfigurationValue *strField = this->GetField(name);
    int value =  StringTable::find(name, strField->value_string);

    return value;
}

void SceneConfiguration::Copy(SceneConfiguration *config) {

    // delete current values
/*
    for(ConfigMap::iterator iter=this->values.begin(); iter!=this->values.end(); iter++) {
        std::string name = iter->first;
        delete values[name];
        values[name] = NULL;
    }*/
    
    for(ConfigMap::iterator iter=config->values.begin(); iter!=config->values.end(); iter++) {
        
        std::string name = iter->first;
        const ConfigurationValue *data = iter->second;
        
        //qDebug() << "copy value:" << name.c_str();
        ConfigurationValue *myValue = const_cast<ConfigurationValue*>(this->values[name]);
        if(myValue == NULL) {
            this->values[name] = new ConfigurationValue();
            myValue = const_cast<ConfigurationValue*>(this->values[name]);
        }
        
        myValue->SetValue(data);
        
    }
}
    
// general configuration method is templated
/*
void SceneConfiguration::Configure(T *prop) {
    ConfigAdapter<T> ca(this, prop);
    ca.Read();
}*/

/*
template<>
void SceneConfiguration::Configure<vtkVolumeProperty>(vtkVolumeProperty *vp) {
    vp->SetInterpolationTypeToLinear();
    ConfigAdapter<vtkVolumeProperty> vsa(vp);
    vsa.Read(this);
}

template<>
void SceneConfiguration::Configure<vtkTextProperty>(vtkTextProperty *prop) {
  prop->ShadowOff();
  prop->BoldOn();
  prop->ItalicOff();
  prop->SetFontSize(12);
  prop->SetFontFamilyToArial();

  ConfigAdapter<vtkTextProperty> ca(prop);
  ca.Read(this);
}

template<>
void SceneConfiguration::Configure<vtkTextActor>(vtkTextActor *prop) {
    prop->SetTextScaleModeToNone();

    ConfigAdapter<vtkTextActor> ca(prop);
    ca.Read(this);

    this->Configure(prop->GetTextProperty());
}



// special deployment for actor
template<>
void SceneConfiguration::Configure<vtkActor>(vtkActor *actor) {
    this->Configure(actor->GetProperty());
    this->Configure(dynamic_cast<vtkProp3D*>(actor));
}



template<>
void SceneConfiguration::Configure<vtkCubeAxesActor>(vtkCubeAxesActor *axes) {
  // dispatch to normal actor config, then specials
  this->Configure(dynamic_cast<vtkActor*>(axes));

  ConfigAdapter<vtkCubeAxesActor> ca(axes);
    ca.Read(this);
}


// vtkCamera needs special wrapping for overloaded methods
template<>
void SceneConfiguration::Configure<vtkCamera>(vtkCamera *camera) {
    ConfigAdapter<vtkCamera> csa(camera);
    csa.Read(this);
    
    CameraWrapper cw(camera);
    ConfigAdapter<CameraWrapper> ccw(&cw);
    ccw.Read(this);
}


template<>
void SceneConfiguration::Configure<vtkSphereSource>(vtkSphereSource *source) {
  Debug("configuring sphere source");
  source->SetThetaResolution(8); // longitude
  source->SetPhiResolution(8); // latitude

  ConfigAdapter<vtkSphereSource> csa(source);
  csa.Read(this);
}

template<>
void SceneConfiguration::Configure<vtkImplicitPlaneWidget2>(vtkImplicitPlaneWidget2* plane) {
  Debug("configuring plane widget and representation\n");
  ConfigAdapter<vtkImplicitPlaneWidget2> planeConfig(plane);
  planeConfig.Read(this);

  ConfigAdapter<vtkImplicitPlaneRepresentation> planeRep(dynamic_cast<vtkImplicitPlaneRepresentation*>(plane->GetRepresentation()));
  planeRep.Read(this);

  //ConfigAdapter<vtkProp3D> cap(this, plane->GetProp3D());
  //cap.Read();

}

template<>
void SceneConfiguration::Configure<vtkBoxWidget2>(vtkBoxWidget2* box) {
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



std::string SceneConfiguration::Serialize() {
    //Debug("=== serializing configuration ===\n");

    json_object *root = json_object_new_object();

    //Debug("values: %i\n", values.size());
    for(ConfigMap::iterator iter=this->values.begin(); iter!=values.end(); iter++) {

        std::string name = iter->first;
        const ConfigurationValue *data = iter->second;

        json_object *entry = NULL;
        if(data->IsScalar()) {
            entry = json_object_new_double(data->value_scalar);
        }
        else if(data->IsMatrix()) {
            entry = json_object_new_array();
            double *ptr = data->value_matrix;

            for(DataSize i=0; i<data->len; i++) {
                json_object_array_add(entry, json_object_new_double(ptr[i]));
            }
        }
        else if(data->IsString()) {
            entry = json_object_new_string(data->value_string.c_str());

        }
        else {
            Debug("unknown value for: %s''\n", name.c_str());
            entry = json_object_new_string("unknown error");
        }


        json_object_object_add (root, name.c_str(), entry);
    }

	//Debug("config json:");
    //Debug(json_object_to_json_string(root));

    std::string serialized = json_object_to_json_string(root);

	// decrements referencecount, free if count==0
    json_object_put(root);

    //Debug("=================================\n");
    //Debug("=================================\n");

    return serialized;
}

/// TODO: fix leaking memory, when building object from json
void SceneConfiguration::Deserialize(std::string json_string) {
    json_object *root = json_tokener_parse(json_string.c_str());

    if(is_error(root)) {
        Warning("config json invalid.\n");
        if(root) json_object_put(root);
        return;
    }

    Debug("json string ok.\n");

    json_object_object_foreach(root, key, val) {
        Debug("key: %s ", key);
        ConfigurationValue* value = NULL;

        switch(json_object_get_type(val)) {

			case json_type_int:
            case json_type_double:
                value = new ConfigurationValue(json_object_get_double(val));
            break;

            case json_type_array:
			{
				DataSize arrLen = json_object_array_length(val);
                value = new ConfigurationValue(arrLen);
                for(DataSize i=0; i<arrLen; i++) {
                    value->value_matrix[i] = json_object_get_double(json_object_array_get_idx (val, i));
                }
			}
            break;

            case json_type_string:
                //Debug("(string)\n");
			{
                value = new ConfigurationValue(key, json_object_get_string(val));
				//value->value_scalar = table.find(key, value->value_string);
			}
            break;

			default:
				Warning("unable to parse value for '%s'.", key);
				value = NULL;
			break;
        }

        if(value) this->SetField(key, value);
    }

	// decrements referencecount, free if count==0
    json_object_put(root);
}

