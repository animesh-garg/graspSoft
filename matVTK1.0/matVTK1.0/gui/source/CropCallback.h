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

#ifndef CROPCALLBACK_H
#define CROPCALLBACK_H

//#include <mex.h>
#include <vector>

#include <vtkCommand.h>
#include <vtkMapperCollection.h>
#include <vtkVolumeCollection.h>
#include <vtkAbstractWidget.h>
#include <vtkWidgetRepresentation.h>

#include <SceneConfiguration.h>

class PlotComponent;




enum CropMode {CropBox, CropPlane};

class vtkBoxWidget2;
class vtkImplicitPlaneWidget2;


class CropCallback : public vtkCommand {
protected:
    
  std::vector<PlotComponent*> *components;
    
public: 
    CropCallback() {
        this->components = new std::vector<PlotComponent*>();
    }
  
    virtual ~CropCallback() {
        delete components;
    }
    
  void AddComponent(PlotComponent* component) {
    this->components->push_back(component);
  }
  

};




class BoxCropCallback : public CropCallback {
    
    virtual void Execute(vtkObject *caller, unsigned long, void* callData);
     
};

class PlaneCropCallback : public CropCallback {
    
    virtual void Execute(vtkObject *caller, unsigned long, void* callData); 
};


// cropping adapter

class Cropper {
    public:
        Cropper() {
            widget = NULL;
            representation = NULL;
            cropCallback = NULL;
            box = NULL;
            plane = NULL;


        }
        
	virtual ~Cropper();
	    
    void InitBox();
    void InitPlane();
        
        void SetCropMode(int mode);
        void Place(double *bounds);
        
        int GetMode() {
            return cropMode;
        }
        
        vtkAbstractWidget* GetWidget() {
            return widget;
        }

        vtkWidgetRepresentation* GetRepresentation() {
          return representation;
        }
        
        bool IsActive() {
            return widget != NULL;
        }
                
        CropCallback* GetCallback() {
            return cropCallback;
        }

        vtkBoxWidget2 *GetBox() { return box; }
        vtkImplicitPlaneWidget2 *GetPlane() { return plane; }
        //void SetConfiguration(SceneConfiguration *config) { this->config = config; }

        void AddComponent(PlotComponent* component) {
          this->cropCallback->AddComponent(component);
        }

    private:
        vtkWidgetRepresentation *representation;
        vtkAbstractWidget *widget;

        vtkBoxWidget2 *box;
        vtkImplicitPlaneWidget2 *plane;

        CropCallback *cropCallback;
        int cropMode;
        //double* bounds;
        //SceneConfiguration *config;
};



#endif

