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

#include "CropCallback.h"
#include "SceneConfiguration.h"
#include "Constants.h"

#include <vtkImplicitPlaneWidget.h>
#include <vtkMapperCollection.h>
#include <vtkVolumeCollection.h>
#include <vtkVolumeMapper.h>
#include <vtkBoxWidget.h>
#include <vtk3DWidget.h>
#include <vtkPlanes.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>
#include <vtkMapper.h>

#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget2.h>

#include <vtkImplicitPlaneWidget2.h>
#include <vtkImplicitPlaneRepresentation.h>

#include <PlotComponent.h>



void BoxCropCallback::Execute(vtkObject *caller, unsigned long, void* callData) {
    callData = NULL;
    vtkBoxWidget2 *widget = reinterpret_cast<vtkBoxWidget2*>(caller);
    vtkPlanes *planes = vtkPlanes::New();

    Debug("clipping on box planes...");
    ((vtkBoxRepresentation*) (widget->GetRepresentation()) )->GetPlanes(planes);
    
    std::vector<PlotComponent*>::iterator comp;
    for(comp = components->begin(); comp != components->end(); ++comp) {
      
        (*comp)->SetCropPlanes(planes);
    }

    planes->Delete();
    return;
}

void PlaneCropCallback::Execute(vtkObject *caller, unsigned long, void* callData) {
    callData = NULL;
    vtkImplicitPlaneWidget2 *widget = reinterpret_cast<vtkImplicitPlaneWidget2*>( caller);


    vtkPlane *plane = vtkPlane::New();
    ((vtkImplicitPlaneRepresentation*) (widget->GetRepresentation()) )->GetPlane(plane);
    
    vtkPlaneCollection *planes = vtkPlaneCollection::New();
    planes->AddItem(plane);
    
    std::vector<PlotComponent*>::iterator comp;
    for(comp = components->begin(); comp != components->end(); ++comp) {
      (*comp)->SetCropPlanes(planes);
    }

    planes->Delete();
    plane->Delete();
    return;
  }




void Cropper::Place(double *bounds) {
    representation->PlaceWidget(bounds);

    //Debug("PLACING cropper.\n");

    if(cropMode == 1 && representation) {
      //Debug("CROPPER: bounds: %f, %f, %f - %f, %f, %f\n", bounds[0], bounds[2], bounds[4], bounds[1], bounds[3], bounds[5]);
        // orient plane to look in [1 1 0] direction
        
        double x = (bounds[1]-bounds[0])/2;
        double y = (bounds[3]-bounds[2])/2;
        double z = (bounds[5]-bounds[4])/2;

        vtkImplicitPlaneRepresentation *plane = dynamic_cast<vtkImplicitPlaneRepresentation*>(representation);

        // place plane normal to (1, 1, 0) direction
        plane->SetNormal(1, 1, 0);

        //mexDebug("setting plane origin: %f, %f, %f\n", x, y, z);
        plane->SetOrigin(x, y, z);
        

        cropCallback->Execute(widget, 0, NULL);
    }    
}

void Cropper::SetCropMode(int mode) {
    if(widget != NULL /*&& cropMode == mode*/) return;

    //unregister callback, delete widtet, delete callback
    /*
    if(theCropper != NULL) { 
        theCropper->RemoveObserver(cropCallback);
        cropCallback->Delete();
        theCropper->Delete();
    }
     **/

    // initialize and set mode
    cropMode = mode;
    if(mode == 0) {
        this->InitBox();
    }
    else if(mode == 1) {
        this->InitPlane();
    }
    
    representation->SetPlaceFactor(1.1);

    widget->AddObserver(vtkCommand::InteractionEvent, cropCallback);
    widget->SetPriority(1);
}
 

void Cropper::InitBox() {
  //Debug("BOX init.\n");
  vtkBoxRepresentation *boxRep = vtkBoxRepresentation::New();
  boxRep->InsideOutOn();

  box = vtkBoxWidget2::New();

  box->SetRepresentation( boxRep );
  representation = boxRep;

  widget = box;
  cropCallback = new BoxCropCallback;
}
        
void Cropper::InitPlane() {
    //Debug("PLANE init.\n");
    plane = vtkImplicitPlaneWidget2::New();
    vtkImplicitPlaneRepresentation *planeRepresentation = vtkImplicitPlaneRepresentation::New();

    plane->SetRepresentation(planeRepresentation);
    planeRepresentation->DrawPlaneOff();

    representation = planeRepresentation;
    widget = plane;
    cropCallback = new PlaneCropCallback;
}

Cropper::~Cropper() {
	/// TODO re-enable destructor functionality,
	//  disabled for debugging purposes only
	
	if(representation) representation->Delete();
	if(widget) widget->Delete();
			
    if(cropCallback != NULL) cropCallback->Delete();
	
    // they must not be freed, because mem is released with widget->Delete()
    //if(box) box->Delete();
	//if(plane) plane->Delete();
}

