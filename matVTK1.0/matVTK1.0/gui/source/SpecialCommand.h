/*
 *  SpecialCommand.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 11.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SPECIALCOMMAND_H
#define SPECIALCOMMAND_H

#include <qobject.h>
#include <MatvtkCommand.h>
#include <StateDataPort.h>
#include <PlotComponent.h>

#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include <vtkSmartPointer.h>

#include <CropCallback.h>

class SpecialCommand : public MatvtkCommand {
	public:
	void setupArgumentMachine(DataPort *port) { 
		port = NULL;
		this->argumentMachine->setInitialState(configDataType);
		this->argumentMachine->addTransition(configDataType, matvtkProtocol::DATA_STOP, nullState);
	}
	static MatvtkCommand* newCommand(matvtkProtocol::MinorCommand minor);
};

#pragma mark CloseCommand

class CloseCommand : public SpecialCommand {
	Q_OBJECT;
	
signals:
	void destroyComponent(PlotComponent*);
	
public:
	bool execute(SceneWindow *win, PlotComponent *component) {
        if(win == NULL) return false;
        
        // if no component is given, close the window
        if(component == NULL) {
            
            if(win != NULL) {
                std::clog << "closing window: " << win << std::endl;
                win->closeWindow();
                //win->deleteLater();
            }
        }
        // unregister and remove one specific component
        else {
            qDebug() << "removing component " << component->name().c_str();

            connect(this, SIGNAL(destroyComponent(PlotComponent*)), win, SLOT(removeComponent(PlotComponent*)), Qt::BlockingQueuedConnection);
            emit(destroyComponent(component));
			disconnect(this, SIGNAL(destroyComponent(PlotComponent*)), win, SLOT(removeComponent(PlotComponent*)));
        }
        
		
		return true;
	}	
};

#pragma mark OpenCommand

class OpenCommand : public SpecialCommand {
	Q_OBJECT;
	
public:
	OpenCommand() {
        
        // must be a blocking connection, because we need a valid handle to return to the client
		QObject::connect(this, SIGNAL(newWindow(RegistryHandle*)), SceneRegistry::instance(), SLOT(newWindow(RegistryHandle*)), Qt::BlockingQueuedConnection);
	}
	
    ~OpenCommand() {
        // disconnect all (one) connections made in the constructor
        QObject::disconnect(SceneRegistry::instance());
    }
    
	bool execute(SceneWindow *win, PlotComponent *component) {
		component = NULL;

		RegistryHandle handle;
		emit(newWindow(&handle));
		
        win = SceneRegistry::instance()->windowForHandle(handle);
        
        this->_sceneWindow = win;
        
		
		return true;
	}	
	
	
signals:
	void newWindow(RegistryHandle*);
	
	
};

#pragma mark GRID command

class SceneGridCommand : public SpecialCommand {
	Q_OBJECT;
	
public:
	SceneGridCommand() {
	}
	
	bool execute(SceneWindow *win, PlotComponent *component) {
		
				
		vtkSmartPointer<vtkCubeAxesActor> axes = vtkSmartPointer<vtkCubeAxesActor>::New();
		
		axes->SetXTitle("x");
		axes->SetYTitle("y");
		axes->SetZTitle("z");
		
		//config.Configure(axes.GetPointer());
		//pipe->SetAxes(axes);
		
		axes->SetBounds(win->GetRenderer()->ComputeVisiblePropBounds());
        axes->SetCamera(win->GetRenderer()->GetActiveCamera());


		component = PlotComponent::NewComponent(axes);
        component->AddConfigAdapter(new ConfigAdapter<vtkCubeAxesActor>(axes.GetPointer()));
        
		component->SetName("AxisGrid");
        this->submitComponent(component, win);

		

		
		/*
		 vtkCubeAxesActor2D *axes2 = vtkCubeAxesActor2D::New();
		 //axes2->SetViewProp(foheActor)
		 axes2->SetCamera(pipe->GetRenderer()->GetActiveCamera());
		 //axes2->SetLabelFormat("%6.4g");
		 
		 axes2->SetFlyModeToOuterEdges();
		 axes2->SetFontFactor(1.0);
		 
		 axes2->ScalingOff();
		 axes2->GetXAxisActor2D()->AdjustLabelsOn();
		 axes2->GetXAxisActor2D()->SetNumberOfLabels(10);
		 axes2->GetXAxisActor2D()->SetNumberOfMinorTicks(10);
		 axes2->SetCornerOffset(0.0);
		 
		 
		 //axes2->SetAxisTitleTextProperty(tprop)
		 //axes2.SetAxisLabelTextProp
		 axes2->SetBounds(pipe->GetRenderer()->ComputeVisiblePropBounds());
		 pipe->GetRenderer()->AddViewProp(axes2);
		 */
		
		
		//axes->SetFlyModeToStaticEdges();
		//axes->DrawXGridlinesOn();
		//axes->DrawYGridlinesOn();
		//axes->DrawZGridlinesOn();
		//renderer->AddActor(axes);
		/*
		 vtkAxisActor *axis = vtkAxisActor::New();
		 
		 //axis->SetTitle("axis title");
		 axis->DrawGridlinesOn();
		 axis->TickVisibilityOn();
		 axis->LabelVisibilityOn();
		 axis->MinorTicksVisibleOn();
		 
		 double bounds[] = {0, 10, 0, 10, 0, 10};
		 axis->SetBounds(bounds);
		 
		 axis->SetCamera(pipe->GetRenderer()->GetActiveCamera());
		 pipe->Submit(axis);*/
		
		

		
		return true;
	}	
};

#pragma mark TITLE

class SceneTitleCommand : public SpecialCommand {
	
	Q_OBJECT;
	
signals:
	void newTitleComponent(PlotComponent *comp);
	
public:
	SceneTitleCommand() {
	}
	
	void setupArgumentMachine(DataPort *port) { 
	
		StateDataType *titleType = new StateDataType(port);
		titleReader = new StateDataRead<StringSerializer>(port);
		
		this->argumentMachine->setInitialState(titleType);
		this->argumentMachine->addTransition(titleType, matvtkProtocol::DATA_STRING, titleReader);
		this->argumentMachine->addTransition(titleReader, StateCondition::Succeeded, configDataType);
		this->argumentMachine->addTransition(configDataType, matvtkProtocol::DATA_STOP, nullState);
		
	}
	
	bool execute(SceneWindow *win, PlotComponent *component);

	
private:
	StateDataRead<StringSerializer> *titleReader;
	
	
};

class SceneOrientationCommand : public SpecialCommand {
public:
    SceneOrientationCommand() {
    }
    
    //void setupArgumentMachine(DataPort *port);
    bool execute(SceneWindow *win, PlotComponent *comp);
    
};

#pragma mark Legend (color)

class ColorLegendCommand : public SpecialCommand {
public:
    ColorLegendCommand() {
    }
    
    void setupArgumentMachine(DataPort *port);
    bool execute(SceneWindow *win, PlotComponent *comp);
    
private:
    StateDataRead<ColorTransferFunctionDataSerializer> *colorReader;
    StateDataRead<StringSerializer> *titleReader;
    
};

#pragma mark vtkView

class ViewCommand : public SpecialCommand {
public:
    ViewCommand() {
    }
    
    void setupArgumentMachine(DataPort *port);
    bool execute(SceneWindow *win, PlotComponent *comp);
    
private:
    StateDataRead<ArraySerializer> *azimutReader;
    StateDataRead<ArraySerializer> *elevationReader;

};

#pragma mark Annotation

class AnnotationCommand : public SpecialCommand {
public:
    AnnotationCommand() {
    }
    
    void setupArgumentMachine(DataPort *port);
    bool execute(SceneWindow *win, PlotComponent *comp);
    
protected:
    StateDataRead<PointDataSerializer> *pointReader;
    StateDataRead<StringSerializer> *textReader;
    
};


#pragma mark Labels

class LabelsCommand : public AnnotationCommand {
public:
    LabelsCommand() {
    }
    
    void setupArgumentMachine(DataPort *port);
    bool execute(SceneWindow *win, PlotComponent *comp);

protected:
    StateDataRead<PointDataSerializer> *pointReader;
    StateDataRead<ArraySerializer> *numLabelReader;
    StateDataRead<StringSerializer> *textLabelReader;
    

};

#pragma mark GetCommand

class GetCommand : public SpecialCommand {
public:
    GetCommand() {
    }
    
    bool execute(SceneWindow *win, PlotComponent *comp) {
        if(win == NULL) return false;
        
        this->_sceneWindow = win;
        
        if(comp == NULL) {
            comp = win->defaultComponent();
        }
        
        this->_plotComponent = comp;
        
        return true;
    }

protected:
};


#pragma mark CropCommand

class CropCommand : public SpecialCommand {
public:
    CropCommand() {
    }
    
    bool execute(SceneWindow *win, PlotComponent *component) {
        
        if(win == NULL) return false;
        
        CropMode value = CropBox;
        
        if(config.HasField("cropTool"))
           value = (CropMode) config.GetStringValue("cropTool");
        

        win->Crop(component, value);
        return true;
    }
    
protected:
};

#pragma mark SaveCommand

class SaveCommand : public SpecialCommand {
public:
    SaveCommand() { }
    
    void setupArgumentMachine(DataPort *port) {
        
        StateDataType *stringType = new StateDataType(port);
        savePathReader = new StateDataRead<StringSerializer>(port);
        
        this->argumentMachine->setInitialState(stringType);
        this->argumentMachine->addTransition(stringType, matvtkProtocol::DATA_STRING, savePathReader);
        this->argumentMachine->addTransition(savePathReader, StateCondition::Succeeded, configDataType);
        
        this->argumentMachine->addTransition(configDataType, matvtkProtocol::DATA_STOP, nullState);
    }

    bool execute(SceneWindow *win, PlotComponent *component);
    
    
private:
    StateDataRead<StringSerializer> *savePathReader;
};

#pragma mark ConfigCommand

class ConfigCommand : public SpecialCommand {
public:
    ConfigCommand() {
    }
    
    void setupArgumentMachine(DataPort *port) {
        this->argumentMachine->setInitialState(configDataType);
    }

    bool execute(SceneWindow *win, PlotComponent *component);
};




#endif

