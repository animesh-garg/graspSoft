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

#ifndef PLOTCOMPONENT_H
#define PLOTCOMPONENT_H

#include <string>
#include <vector>

#include <vtkSetGet.h>
#include <vtkObjectBase.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkVolume.h>
#include <vtkProp.h>
#include <vtkPlanes.h>
#include <vtkMapper.h>
#include <vtkMapper2D.h>
#include <vtkVolumeMapper.h>
#include <vtkPlaneCollection.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkInteractorObserver.h>
#include <vtkOrientationMarkerWidget.h>


#include <qobject.h>
#include <qstring.h>
#include <qaction.h>
#include <qmenu.h>
#include <qmainwindow.h>
#include <qapplication.h>

#include <SceneConfiguration.h>

#include <SceneWindow.h>
#include <matvtkProtocol.h>
#include <ConfigAdapter.h>
#include <SceneRegistry.h>


/*
#define SUBMIT(pipe, prop)\
PlotComponent *comp = PlotComponent::NewComponent(prop);\
pipe->Submit(comp);\
ObjectHandle<PlotComponent> *compHandle = new ObjectHandle<PlotComponent>(comp);\
plhs[0] = compHandle->to_mex_handle()
*/

class PlotComponent;
class SceneWindow;
class ConfigDialog;

class QtHandler : public QObject {
	Q_OBJECT
    
public:
    QtHandler(PlotComponent *component) : component(component), configDialog(NULL) { 
        
	}
    
    void SetSceneWindow(SceneWindow *win) { this->window = win; }
    
public slots:
    void toggleVisibility(bool vis);
    void removeComponent();
    void configure();
    void readConfiguration(SceneConfiguration *config);
    
signals:
	void updateWindow();
    void deleteComponent(PlotComponent *c);
	
private:
    PlotComponent *component;

    // weak reference to scene window
    SceneWindow *window;
    
    ConfigDialog *configDialog;
    
    friend class PlotComponent;
};

class PlotComponent : public vtkObject
{
public:

    static PlotComponent* NewComponent(vtkActor* actor);
    static PlotComponent* NewComponent(vtkVolume* volume);
	static PlotComponent* NewComponent(vtkActor2D* actor2d);
    static PlotComponent* NewComponent(vtkInteractorObserver* observer);
    static PlotComponent* NewComponent(vtkOrientationMarkerWidget* orientationMarker);
    
	static PlotComponent *New();
	vtkTypeRevisionMacro(PlotComponent, vtkObject);
	
	
    virtual vtkProp* GetProp() { return this->prop; }

	virtual void SetProp(vtkProp* prop) {
		if(this->prop) this->prop->UnRegister(this);
		
		if(prop) prop->Register(this);
		this->prop = prop;
	}
	
	//virtual void addToRenderer(vtkRenderer *renderer) = 0;
	
    void SetCropPlanes(vtkPlanes *planes) {
      mapper->SetClippingPlanes(planes);
    }

    void SetCropPlanes(vtkPlaneCollection *planes) {
      mapper->SetClippingPlanes(planes);
    }

	void SetName(std::string name) {
		this->_name = name;
	}
	
	std::string name() {
		return this->_name;
	}
	
	virtual ~PlotComponent() { 
		//Debug("destructing plot component");
		if(this->prop) this->prop->UnRegister(this);

        // delete all config adapters for this component
        std::list<ConfigAdapterBase*>::iterator adapterIt;
        for(adapterIt = configAdapters.begin();
            adapterIt != configAdapters.end();
            adapterIt++) {
            
            delete (*adapterIt);
        }

        
        
        ///TODO: this may crash if used
        //this->qtHandler->deleteLater();
	}

    virtual void AddToWindow(SceneWindow* window);
    virtual void RemoveFromWindow(SceneWindow *window);
    
    virtual QMenu* componentMenu(QMainWindow *window) {
        RegistryHandle handle = SceneRegistry::instance()->handleFor(this);
        QString menuLabel = QString("Component %1 (%2)").arg(QString(this->_name.c_str())).arg(handle);
        qDebug() << "menuLabel: " << menuLabel;
        this->_menu = new QMenu(menuLabel, window);

		// move to application main thread and set parent to enable correct signal/slot handling
		this->_menu->moveToThread(QApplication::instance()->thread());
		this->qtHandler->setParent(this->_menu);

		
        QAction *configurationAction = new QAction("Configure...", this->_menu);
        QObject::connect(configurationAction, SIGNAL(triggered()), qtHandler, SLOT(configure()), Qt::QueuedConnection);
        this->_menu->addAction(configurationAction);
        
        QAction *visibilityAction = new QAction("Visible", this->_menu);
		visibilityAction->setCheckable(true);
		visibilityAction->setChecked(true);
		QObject::connect(visibilityAction, SIGNAL(triggered(bool)), qtHandler, SLOT(toggleVisibility(bool)), Qt::QueuedConnection);
        //QObject::connect(visibilityAction, SIGNAL(triggered()), qtHandler, SLOT(dummyToggle()), Qt::QueuedConnection);
        this->_menu->addAction(visibilityAction);		
		

        QAction *removeAction = new QAction("remove", this->_menu);
        QObject::connect(removeAction, SIGNAL(triggered()), qtHandler, SLOT(removeComponent()), Qt::QueuedConnection);
        
        this->_menu->addAction(removeAction);
        
        
   		QObject::connect(qtHandler, SIGNAL(updateWindow()), window, SLOT(update()));
        return this->_menu;
    }

	virtual void removeMenu() {
        //window->removeMenu(this->_menu);
        
		this->_menu->clear();
        delete this->_menu; 
        this->_menu = NULL;
    }
	
    
    virtual void Configure(SceneConfiguration *config) {

        std::list<ConfigAdapterBase*>::iterator adapterIt;
        
        for(adapterIt = configAdapters.begin();
            adapterIt != configAdapters.end();
            adapterIt++) {
            
            ConfigAdapterBase *adapter = (*adapterIt);
            adapter->Read(config);
            adapter->Write(config);
        }
    }
    
    /*
    virtual void ExportConfig(SceneConfiguration *config) {
    
        std::list<ConfigAdapterBase*>::iterator adapterIt;
        
        for(adapterIt = configAdapters.begin();
            adapterIt != configAdapters.end();
            adapterIt++) {
            
            ((ConfigAdapterBase*) (*adapterIt))->Write(config);
        }
    }
    */
    
    void AddConfigAdapter(ConfigAdapterBase *ca) {
        configAdapters.push_back(ca);
    }
    
    
  protected:
    QMenu* _menu;
	vtkAbstractMapper *mapper;
	vtkProp *prop;
	std::string _name;
    
    std::list<ConfigAdapterBase*> configAdapters;
    
    QtHandler *qtHandler;    
    
    
	// protected con-/destructor, because we should use the New()/Delete() vtk method;
	PlotComponent() : mapper(NULL), prop(NULL), _name("PlotComponent") {
		this->qtHandler = new QtHandler(this);
		this->qtHandler->moveToThread(QApplication::instance()->thread());
        this->_name = "Unnamed Component";
	}
    
	
	friend class QtHandler;
};



class ActorPlotComponent : public PlotComponent {

  public:
	static ActorPlotComponent* New();
	
	vtkTypeRevisionMacro(ActorPlotComponent, PlotComponent);
	
	void SetProp(vtkActor *actor) {
        this->actor = actor;
		PlotComponent::SetProp(actor);
		this->mapper = vtkAbstractMapper::SafeDownCast(actor->GetMapper());
    }
	

  protected:
    vtkActor *actor;
	ActorPlotComponent() {
	}
		
  private:
};

class VolumePlotComponent : public PlotComponent {

  public:
	static VolumePlotComponent* New();
	vtkTypeRevisionMacro(VolumePlotComponent, PlotComponent);

	

	void SetProp(vtkVolume *volume) {
        this->volume = volume;
		PlotComponent::SetProp(volume);
		this->mapper = vtkAbstractMapper::SafeDownCast(volume->GetMapper());
    }
    

	
protected:
    vtkVolume *volume;
	VolumePlotComponent() {
		this->_name = "Volume";
	}

};

class Actor2DPlotComponent : public PlotComponent {
	
  public:
	static Actor2DPlotComponent* New();
	vtkTypeRevisionMacro(Actor2DPlotComponent, PlotComponent);
	
	
	void SetProp(vtkActor2D *actor2d) {
        this->actor2d = actor2d;
		PlotComponent::SetProp(actor2d);
		this->mapper = vtkAbstractMapper::SafeDownCast(actor2d->GetMapper());
	}
    


	
protected:
    vtkActor2D *actor2d;
    Actor2DPlotComponent() {
		this->_name = "2D Component";
    }

	
private:

};


class InteractorObserverPlotComponent : public PlotComponent {
public:
    static InteractorObserverPlotComponent* New();
    vtkTypeRevisionMacro(InteractorObserverPlotComponent, PlotComponent);
    
    void AddToWindow(SceneWindow* window);
    void RemoveFromWindow(SceneWindow *window);
    
    void SetObserver(vtkInteractorObserver *observer) {
        if(this->_observer) this->_observer->UnRegister(this);
		
		if(observer) observer->Register(this);
		this->_observer = observer;

    }
    

    
    ~InteractorObserverPlotComponent() {
        
        if(_observer) _observer->UnRegister(this);
        qDebug() << "destroying InteractorObserverPlotComponent";
    }
    
protected:
    InteractorObserverPlotComponent() : _observer(NULL) {
        
        this->_name="InteractorObserver";
    }
    
    vtkInteractorObserver *_observer;
};


class OrientationMarkerPlotComponent : public InteractorObserverPlotComponent {

public:
    static OrientationMarkerPlotComponent* New();
    
    vtkTypeRevisionMacro(OrientationMarkerPlotComponent, InteractorObserverPlotComponent);

    
protected:
    OrientationMarkerPlotComponent() : _observer(NULL) {
        
        this->_name="OrientationMarker";
    }
    
    vtkInteractorObserver *_observer;
    
};

#endif // PLOTCOMPONENT_H
