/*
 *  StateDataPort.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 06.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include <StateDataPort.h>

#include <qobject.h>

#include "MatvtkCommand.h"
#include "PlotCommand.h"

#include "MatvtkCommandFactory.h"

void StateCommand::process() {
	matvtkProtocol::MajorCommand major = (matvtkProtocol::MajorCommand) this->_port->read<CommandData>();
	std::cout << "major command: " << major << std::endl;

	matvtkProtocol::MinorCommand minor = (matvtkProtocol::MinorCommand) this->_port->read<CommandData>();
	std::cout << "minor command: " << minor << std::endl;

	RegistryHandle windowHandle = this->_port->read<CommandData>();
	std::cout << "window handle: " << windowHandle << std::endl;

	RegistryHandle componentHandle = this->_port->read<CommandData>();
	std::cout << "component handle: " << componentHandle << std::endl; 



	MatvtkCommand *command = MatvtkCommandFactory::newCommand(major, minor);
	qDebug() << "new command:" << command;
	
	// break with error if command implementation does not exist
	if (command == NULL) {
		this->_condition = StateCondition::Failed;
		std::cerr << "command " << major << " " << minor << " not defined." << std::endl;
		return;
	}

    SceneRegistry *registry = SceneRegistry::instance();
    // get actual objects for scene and component handle
    SceneWindow *window = registry->windowForHandle(windowHandle);
    PlotComponent *component = registry->componentForHandle(componentHandle);
	
	bool argumentsValid = command->readArguments(this->_port);
    bool executionResult = false;
    
    if(window == NULL) argumentsValid = false;
    /*
    if(window == NULL && windowHandle == 0) {
        QObject::connect(this, SIGNAL(newWindow(RegistryHandle*)), SceneRegistry::instance(), SLOT(newWindow(RegistryHandle*)), Qt::BlockingQueuedConnection);
        window = SceneWindow::newWindow();
        qDebug("no default window, creating new");
    }
    else if(window == NULL && windowHandle != 0) {
        argumentsValid = false;
        qDebug() << "window handle invalid:" << windowHandle;
    }
    */
    
    
	if(argumentsValid) {

		///TODO: use Qt:QueuedConnection, does not block signal emission, makes network interface faster, but: PlotComponent must retain vtk objects
		QObject::connect(command, SIGNAL(componentCreated(PlotComponent*)), window, SLOT(addComponent(PlotComponent*)), Qt::BlockingQueuedConnection);
		
		executionResult = command->execute(window, component);
        
        //if(command->sceneWindow())
        window = command->sceneWindow();
        //if(command->plotComponent()) 
        component = command->plotComponent();
    }
  
    // write return value (command call success/fail)
    if(executionResult) this->_port->write((CommandData)0);
    else this->_port->write((CommandData)1);
    
    // return handle values

    qDebug() << "window:" << window;
    qDebug() << "component:" << component;

    windowHandle = registry->handleFor(window);
    componentHandle = registry->handleFor(component);
    
    qDebug() << "window handle:" << windowHandle;
    qDebug() << "component handle:" << componentHandle;
    
    this->_port->write((CommandData)windowHandle);
    this->_port->write((CommandData)componentHandle);
    
    this->_port->flush();
    
    // here should be the return values
    command->writeReturnData(this->_port);
    
    this->_port->flush();

    delete command;

	if (argumentsValid && executionResult) {
		this->_condition = StateCondition::Succeeded;
	}
	else {
		std::cerr << "command execution failed." << std::endl;
		this->_condition = StateCondition::Failed;
	}


}
