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
#include "PlotComponent.h"

#include "vtkObjectFactory.h"
#include <SceneWindow.h>
#include <ConfigDialog.h>


void QtHandler::readConfiguration(SceneConfiguration *config) {
    
    
    qDebug() << "PLOT COMPONENT CONFIGURE";
    qDebug() <<"SceneConfig" << config;
//    qDebug() << "color value: " << config->GetField("color");
//    qDebug() << "color data: " << config->GetField("color")->GetMatrix()[0] << config->GetField("color")->GetMatrix()[1] << config->GetField("color")->GetMatrix()[2];

    this->component->Configure(config);
    
    emit(updateWindow());
}

void QtHandler::toggleVisibility(bool vis) {
	qDebug() << " visibility for " << this->component->_name.c_str() << vis;
	
	this->component->prop->SetVisibility(vis);

	emit(updateWindow());
}

void QtHandler::removeComponent() {
    qDebug() << " removing component " << this->component->_name.c_str();
    
    //this->component->RemoveFromWindow(this->window);
    
    //connect(this, SIGNAL(deleteComponent(PlotComponent*)), this->window, SLOT(removeComponent(PlotComponent*)), Qt::QueuedConnection);
    //emit(deleteComponent(this->component));
    //disconnect(this, SIGNAL(deleteComponent(PlotComponent*)), this->window, SLOT(removeComponent(PlotComponent*)));

    emit(updateWindow());
    
    this->window->removeComponent(this->component);
    
}

void QtHandler::configure() {
    qDebug() << " config dialog for component " << this->component->_name.c_str();
    
    
    if(configDialog == NULL) {
        configDialog = new ConfigDialog(this->window->GetMainWindow());
        //dialog->SetModal(FALSE);
        
        QString configTitle = "Configure ";
        configTitle.append(this->component->name().c_str());
        configDialog->setWindowTitle(configTitle);
        
        QObject::connect(configDialog, SIGNAL(configChanged(SceneConfiguration*)), this, SLOT(readConfiguration(SceneConfiguration*)));
    }
    else {
        configDialog->clear();
    }
    
    // update all config data
    std::list<ConfigAdapterBase*>::iterator adapterIt;

    for(adapterIt = component->configAdapters.begin();
        adapterIt != component->configAdapters.end();
        adapterIt++) {
        
        ConfigAdapterBase *adapter = (*adapterIt);
        configDialog->buildConfig(adapter);
    }

    
    configDialog->show();
    configDialog->raise();
    configDialog->activateWindow();


}


PlotComponent* PlotComponent::NewComponent(vtkActor* actor) {
	ActorPlotComponent *comp = ActorPlotComponent::New();
	comp->SetProp(actor);
    comp->AddConfigAdapter(new ConfigAdapter<vtkProp3D>(actor, "3D object"));
    comp->AddConfigAdapter(new ConfigAdapter<vtkProperty>(actor->GetProperty(), "property"));
	return comp;
}

PlotComponent* PlotComponent::NewComponent(vtkVolume* volume) {
	VolumePlotComponent *comp = VolumePlotComponent::New();
	comp->SetProp(volume);
    comp->AddConfigAdapter(new ConfigAdapter<vtkProp3D>(volume, "3D object"));
    //comp->AddConfigAdapter(new ConfigAdapter<vtkVolume>(volume, "volume"));
    comp->AddConfigAdapter(new ConfigAdapter<vtkVolumeProperty>(volume->GetProperty(), "volume details"));
	return comp;
}

PlotComponent* PlotComponent::NewComponent(vtkActor2D* actor2d) {
	Actor2DPlotComponent *comp = Actor2DPlotComponent::New();
	comp->SetProp(actor2d);
    comp->AddConfigAdapter(new ConfigAdapter<vtkActor2D>(actor2d, "actor 2d"));

	return comp;
}

PlotComponent* PlotComponent::NewComponent(vtkInteractorObserver *interactorObserver) {
    InteractorObserverPlotComponent *comp = InteractorObserverPlotComponent::New();
    
    comp->SetObserver(interactorObserver);
    
    return comp;
}

PlotComponent* PlotComponent::NewComponent(vtkOrientationMarkerWidget *orientationMarker) {
    //OrientationMarkerPlotComponent *comp = OrientationMarkerPlotComponent::New();
    InteractorObserverPlotComponent *comp = InteractorObserverPlotComponent::New();
    
    comp->SetObserver(orientationMarker);
    //comp->SetProp(orientationMarker->GetOrientationMarker());

    return comp;
}


vtkCxxRevisionMacro(PlotComponent, "revision: 1.0");
vtkStandardNewMacro(PlotComponent);

vtkCxxRevisionMacro(ActorPlotComponent, "revision: 1.0");
vtkStandardNewMacro(ActorPlotComponent);

vtkCxxRevisionMacro(VolumePlotComponent, "revision: 1.0");
vtkStandardNewMacro(VolumePlotComponent);

vtkCxxRevisionMacro(Actor2DPlotComponent, "revision: 1.0");
vtkStandardNewMacro(Actor2DPlotComponent);

vtkCxxRevisionMacro(InteractorObserverPlotComponent, "revision: 1.0");
vtkStandardNewMacro(InteractorObserverPlotComponent);

vtkCxxRevisionMacro(OrientationMarkerPlotComponent, "revision: 1.0");
vtkStandardNewMacro(OrientationMarkerPlotComponent);





void PlotComponent::AddToWindow(SceneWindow* window) {
    window->GetRenderer()->AddViewProp(this->GetProp());
    this->qtHandler->window = window;
}

void PlotComponent::RemoveFromWindow(SceneWindow *window) {
    window->GetRenderer()->RemoveViewProp(this->GetProp());
    this->qtHandler->window = NULL;
}



void InteractorObserverPlotComponent::AddToWindow(SceneWindow* window) {
    this->_observer->SetInteractor(window->GetRenderer()->GetRenderWindow()->GetInteractor());
    this->_observer->SetEnabled(1);
    //PlotComponent::AddToWindow(window);
    
    this->qtHandler->SetSceneWindow(window);
}

void InteractorObserverPlotComponent::RemoveFromWindow(SceneWindow *window) {
    this->_observer->SetEnabled(0);
    this->_observer->SetInteractor(NULL);
        
    this->qtHandler->SetSceneWindow(NULL);
}

