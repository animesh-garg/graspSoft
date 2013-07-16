/*
 *  matVTKguiWindow.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 19.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SCENEWINDOW_H
#define SCENEWINDOW_H

class SceneConfiguration;



#include <qobject.h>
#include <qmainwindow.h>
#include <qmenu.h>
#include <qstring.h>
#include <QVTKWidget.h>
#include <SceneRegistry.h>

#include <CropCallback.h>
//#include <ConfigAdapter.h>

class ConfigDialog;
class PlotComponent;

template<typename T> class ConfigAdapter;

class CameraWrapper;

class SceneWindow : public QObject {

	Q_OBJECT;
	
public:
	SceneWindow();
	virtual ~SceneWindow();
	
	void buildMenus();
	
	void setWindowTitle(QString newTitle);
	QString windowTitle();
	

	//void removeComponent(PlotComponent *comp);
	//void removeComponent(RegistryHandle componentHandle);
	vtkRenderWindow* GetRenderWindow();

	vtkRenderer* GetRenderer() {
		return this->_renderer;
	}
	
    QMainWindow *GetMainWindow() {
        return window;
    }
	
	
	PlotComponent* GetTitleComponent() {
		return this->_titleComponent;
	}
    
    PlotComponent* defaultComponent() {
        return this->_defaultComponent;
    }
    
    void addWindowAction(QAction *action) {
        action->setParent(this->window);
        this->windowMenu->addAction(action);
    }
    
    void removeWindowAction(QAction *action) {
        this->window->removeAction(action);
    }

    void Crop(PlotComponent *component, CropMode mode);

    
signals:
    void continueClient();
	
public slots:
	static SceneWindow* newWindow();
	RegistryHandle addComponent(PlotComponent *component);
	void removeComponent(PlotComponent *comp);
	void removeComponent(RegistryHandle componentHandle);
	
	void SetTitleComponent(PlotComponent* component) {
		if (this->_titleComponent) {
			this->removeComponent(this->_titleComponent);
		}
		
		this->_titleComponent = component;
		
		if(component != NULL) {
			this->addComponent(component);
		}
	}
	
	void closeWindow();
	void showWindow();
	void saveWindow();
    void Configure(SceneConfiguration *config);
    void configureWindow();
    
    
    void refresh();
	void about();
	
private:
    void MakeCropper(CropMode mode);

	QMainWindow *window;
	QWidget *centralWidget;
	QVTKWidget *vtkWidget;
	vtkRenderer *_renderer;
	
	QMenu *fileMenu;
	QMenu *windowMenu;
	
	QAction *newWindowAction;
	QAction *closeWindowAction;
    QAction *saveWindowAction;
    
	std::set<PlotComponent*> components;
    
    PlotComponent *_defaultComponent;
	PlotComponent *_titleComponent;
	
    Cropper cropper;
    
    ConfigDialog *configDialog;
    ConfigAdapter<vtkRenderer> *rendererConfigAdapter;
    ConfigAdapter<CameraWrapper> *cameraConfigAdapter;
    ConfigAdapter<vtkRenderWindow> *renderWindowConfigAdapter;
};

#endif

