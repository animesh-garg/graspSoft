/*
 *  matVTKguiWindow.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 19.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "SceneWindow.h"

#include <qobject.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qaction.h>
#include <qboxlayout.h>
#include <qstring.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qdesktopservices.h>


#include <QVTKWidget.h> 
#include <vtkRenderer.h> 
#include <vtkRenderWindow.h>
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include <vtkCamera.h>

#include <vtkImageWriter.h>
#include <vtkPNGWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkWindowToImageFilter.h>

#include <SceneRegistry.h>
#include <SceneConfiguration.h>
#include <PlotComponent.h>

#include <ConfigDialog.h>

SceneWindow::SceneWindow() {

	qDebug("SceneWindow: constructor");
	
	this->_titleComponent = NULL;
	this->_defaultComponent = NULL;
    
	this->window = new QMainWindow();
	this->window->setAttribute(Qt::WA_DeleteOnClose);
	
	this->centralWidget = new QWidget(window);

	this->centralWidget->setLayout(new QGridLayout());
	window->setCentralWidget(centralWidget);	
	
	window->resize(640, 480);
	
	this->vtkWidget = new QVTKWidget(centralWidget);
	
	vtkWidget->resize(window->size());
	vtkWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	this->_renderer = vtkRenderer::New();

	
	this->vtkWidget->GetRenderWindow()->AddRenderer(this->_renderer);     
	
    ///TODO: is this ok? performance?
    this->vtkWidget->GetRenderWindow()->PointSmoothingOn();
    this->vtkWidget->GetRenderWindow()->LineSmoothingOn();
    this->vtkWidget->GetRenderWindow()->PolygonSmoothingOn();
    
	centralWidget->layout()->addWidget(vtkWidget); 

    this->configDialog = NULL;
    rendererConfigAdapter = new ConfigAdapter<vtkRenderer>(this->_renderer, "Renderer");
   
    renderWindowConfigAdapter = new ConfigAdapter<vtkRenderWindow>(this->GetRenderWindow(), "Window");
    
    CameraWrapper *cameraWrapper = new CameraWrapper(this->_renderer->GetActiveCamera());
    cameraConfigAdapter = new ConfigAdapter<CameraWrapper>(cameraWrapper, "Camera");

	this->buildMenus();
}

void SceneWindow::buildMenus() {
	fileMenu = window->menuBar()->addMenu(tr("&File"));
	
	// FILE MENU
	// add window menu entry + action
	newWindowAction = new QAction(("&New Window"), window);
	newWindowAction->setShortcuts(QKeySequence::New);
	newWindowAction->setStatusTip(("Create a new Window"));

    fileMenu->addAction(newWindowAction);
	connect(newWindowAction, SIGNAL(triggered()), this, SLOT(newWindow()));
	
    // export scene / save as image
    saveWindowAction = new QAction(("&Save Window As..."), window);
    saveWindowAction->setShortcuts(QKeySequence::SaveAs);
    saveWindowAction->setStatusTip(("Save window as image"));
    fileMenu->addAction(saveWindowAction);
    connect(saveWindowAction, SIGNAL(triggered()), this, SLOT(saveWindow()));
    
    
	// close window menu entry + action
	closeWindowAction = new QAction(("Close &Window"), window);
	closeWindowAction->setShortcuts(QKeySequence::Close);
	closeWindowAction->setStatusTip(("close current Window"));
	
	fileMenu->addAction(closeWindowAction);
	connect(closeWindowAction, SIGNAL(triggered()), this, SLOT(closeWindow()));
	
	// quit application entry
	QAction *quitAction = new QAction(tr("&Quit"), window);
	quitAction->setShortcuts(QKeySequence::Quit);
	fileMenu->addAction(quitAction);
	connect(quitAction, SIGNAL(triggered()), SceneRegistry::instance(), SLOT(shutdownApplication()));
	//connect(quitAction, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
	

	// add WINDOW menu
	
	this->windowMenu = window->menuBar()->addMenu(tr("Window"));
    
    QAction* configureAction = new QAction("Scene settings...", window);
    //configureAction->setShortcuts(QKeySequence::Configure);
    this->windowMenu->addAction(configureAction);
    connect(configureAction, SIGNAL(triggered()), this, SLOT(configureWindow()));
            
    this->windowMenu->addSeparator();
	
    // ABOUT MENU
	
	QMenu *helpMenu = window->menuBar()->addMenu(tr("&Help"));
	
	QAction *aboutAction = new QAction("About", window);
	helpMenu->addAction(aboutAction);
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
	
}

void SceneWindow::saveWindow() {
    qDebug("saving window.");
    
    QString docDir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation).append("/");
    
    const QString caption = "Save scene as image";
    const QString dir = docDir.append(QString("SceneWindow_%1.png").arg(SceneRegistry::instance()->handleFor(this)));
    
    const QString filter =  tr("PNG Image (*.png) ;; JPEG Image (*.jpg)");
    QString selectedFilter = "PNG Image (*.png)";
    //Options option = 0;
    QString saveFile = QFileDialog::getSaveFileName ( centralWidget, caption, dir, filter, &selectedFilter);
    qDebug() << "saving to:" << saveFile << " as: " << selectedFilter;
    

    // no file was selected, user clicked "cancel": return without doing anything
    if(saveFile.length() == 0) {
        Debug("cancelled saving file.");
        return;
    }
    
    
    vtkSmartPointer<vtkWindowToImageFilter> imageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    imageFilter->SetInput(this->vtkWidget->GetRenderWindow());

    
    vtkSmartPointer<vtkImageWriter> imageWriter;
    if(selectedFilter == "PNG Image (*.png)") {
        imageWriter = vtkSmartPointer<vtkPNGWriter>::New();        
    }
    else if(selectedFilter == "JPEG Image (*.jpg)") {
        imageWriter = vtkSmartPointer<vtkJPEGWriter>::New();
    }
    else {
        //Default to PNG writer
        imageWriter = vtkSmartPointer<vtkPNGWriter>::New();
    }

        
    imageWriter->SetInput(imageFilter->GetOutput());
    imageWriter->SetFileName(saveFile.toStdString().c_str());
    imageWriter->Write();
    
}

void SceneWindow::refresh() {

	//this->_renderer->Render();
	this->vtkWidget->update();
	//this->vtkWidget->GetRenderWindow()->Render();
}


SceneWindow* SceneWindow::newWindow() {
	
	qDebug("creating new window.");
	
	SceneWindow *win = new SceneWindow(); //(NULL, Qt::WA_DeleteOnClose);
	
	SceneRegistry::instance()->registerWindow(win);
	
	QString title = "matVTK visu server (%1)";
	
	static int count = 1;
	title = title.arg(count++);
	
	win->setWindowTitle(title);
	win->showWindow();
	
	qDebug("done new window.");
	
	return win;
}

void SceneWindow::showWindow() {
	window->show();
}

void SceneWindow::closeWindow() {
	qDebug("closing window");

    // let client go, if someone is waiting
    //emit(continueClient());
    
    // close window
	window->close();
    
	//window->hide();
	
    
    emit(continueClient());
    this->disconnect();
    //QObject::disconnect(win, SIGNAL(continueClient()), 0, 0);
    
    // unregister handle
	SceneRegistry::instance()->unregisterWindow(this);
	
	// free memory from Qt runloop
	this->deleteLater();
}

void SceneWindow::about() {
	
	qDebug("about message");
    
    QString revString = "$Revision: 10941 $";

    revString = revString.section(" ", 1, 1);
    
    QString aboutText;
    aboutText.append("<center><b>matVTK</b></center>\n");
    //aboutText.append("<p>matVTK version ").append(QString("MATVTK_VERSION").append(" (rev ").append(revString).append(")</p>\n");
    QString versionLine = QString("<p>matVTK version %1 (rev %2)</p>").arg(QString("MATVTK_VERSION"), revString);
    aboutText.append(versionLine);
    
    aboutText.append("<p>For more information: <a href=\"http://www.cir.meduniwien.ac.at/matvtk/\">visit the matVTK Homepage</a></p>\n");
    aboutText.append("<p>Contact: <a href=\"mailto:erich.birngruber@meduniwien.ac.at\">Erich Birngruber (erich.birngruber@meduniwien.ac.at)</a></p>\n");
    
	QMessageBox::about( this->window, "matVTK GUI Server", aboutText);
	
}

void SceneWindow::configureWindow() {
    qDebug() << "CONFIGURE WINDOW.";
    
    if(this->configDialog == NULL) {
        this->configDialog = new ConfigDialog(this->window);
        this->configDialog->buildConfig(rendererConfigAdapter);
        this->configDialog->buildConfig(renderWindowConfigAdapter);
        this->configDialog->buildConfig(cameraConfigAdapter);
        
        connect(configDialog,SIGNAL(configChanged(SceneConfiguration*)), this, SLOT(Configure(SceneConfiguration*)));
        connect(configDialog,SIGNAL(configChanged(SceneConfiguration*)), this, SLOT(refresh()));
    }
    
    this->configDialog->show();
    this->configDialog->raise();
    this->configDialog->activateWindow();
}

void SceneWindow::setWindowTitle(QString newTitle) {
	window->setWindowTitle(newTitle);
}

QString SceneWindow::windowTitle() {
	return window->windowTitle();
}


RegistryHandle SceneWindow::addComponent(PlotComponent *component) {
	
    this->_defaultComponent = component;
	RegistryHandle handle = SceneRegistry::instance()->registerComponent(this, component);
	
	bool firstComponent = (this->components.size() == 0);
	
    // register component, create menu entry
	this->components.insert(component);
	windowMenu->addMenu(component->componentMenu(this->window));

	
    component->AddToWindow(this);
		
	// when adding first component: automatically reset view
	if (firstComponent) {
		this->vtkWidget->GetRenderWindow()->Render();
		this->_renderer->ResetCamera();
	}
	this->refresh();
	
	return handle;
}

void SceneWindow::removeComponent(PlotComponent *comp) {
    // remove menu entry
    comp->removeMenu();
    
    // remove plot part
	SceneRegistry::instance()->unregisterComponent(this, comp);
    comp->RemoveFromWindow(this);
	this->components.erase(comp);
	
    if(this->_defaultComponent == comp) {
        std::set<PlotComponent*>::reverse_iterator it;
        for(it = components.rbegin(); it != components.rend(); ++it) {
            this->_defaultComponent = (*it);
            break;
        }
    }

    if(comp) {
        qDebug() << "comp reference count:" << comp->GetReferenceCount();

        if (comp->GetProp()) 
            qDebug() << "prop reference count:" << comp->GetProp()->GetReferenceCount();   
    }
	comp->Delete();
    
    
	this->refresh();
}

void SceneWindow::removeComponent(RegistryHandle componentHandle) {
	PlotComponent *comp = SceneRegistry::instance()->componentForHandle(componentHandle);
	this->removeComponent(comp);
}


vtkRenderWindow* SceneWindow::GetRenderWindow() {
	return this->vtkWidget->GetRenderWindow();
}


void SceneWindow::Crop(PlotComponent *component, CropMode mode=CropPlane) {
    //Debug("adding crop component %p to vec %p", component, components);
    
    Debug("making cropper");
    this->MakeCropper(mode);

    if(component != NULL)
        cropper.AddComponent(component);
    else {
        std::set<PlotComponent*>::iterator it;
        for(it = this->components.begin(); it != this->components.end(); ++it) {
            PlotComponent *comp = (*it);
            cropper.AddComponent(comp);
        }
        
    }
    
    Debug("placing cropper");
    cropper.Place(this->_renderer->ComputeVisiblePropBounds());
    
    
}


void SceneWindow::MakeCropper(CropMode mode) {
    if(cropper.IsActive()) return;
    
    cropper.SetCropMode(mode);
    cropper.GetWidget()->SetInteractor(this->vtkWidget->GetInteractor());
}

void SceneWindow::Configure(SceneConfiguration *config) {

    rendererConfigAdapter->Read(config);
    rendererConfigAdapter->Write(config);
    
    renderWindowConfigAdapter->Read(config);
    renderWindowConfigAdapter->Write(config); 
    
    cameraConfigAdapter->Read(config);
    cameraConfigAdapter->Write(config);
}

SceneWindow::~SceneWindow() {
	qDebug("SceneWindow destructor.");

    // delete plot components first, they are used by most parts of a scene (renderer, menu, ...)
    std::set<PlotComponent*>::iterator it;
	for(it = components.begin(); it != components.end(); ++it) {
		PlotComponent *comp = (*it);
		//qDebug() << "PlotComponent references: " << comp->GetReferenceCount();
		comp->Delete();
	}
    
    this->_renderer->Delete();

	
	//this->vtkWidget->deleteLater();
	//this->centralWidget->deleteLater();

    // does deleteLater() work here as intended?
	// this may get deferred and cause vtk to spill leaks messages. 
	// works fine when using delete window
	//this->window->deleteLater();
    //delete this->window;
	
    
    
    // emit(continueClient());
	
	// Qt memory management cleans these up:
	//delete this->vtkWidget;
	//QVTKWidget *vtkWidget;
	
	//delete newWindowAction;
	//delete closeWindowAction;

	//disconnect(this);
    qDebug("SceneWindow destructor - done.");
}




