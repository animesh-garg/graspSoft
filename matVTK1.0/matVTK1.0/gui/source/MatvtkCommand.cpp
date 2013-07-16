/*
 *  MatvtkCommand.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 05.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "MatvtkCommand.h"

#include <Constants.h>
#include <matvtkProtocol.h>

#include <DataPort.h>
#include <StateDataPort.h>
#include <StringSerializer.h>

#include <PlotComponent.h>


MatvtkCommand::MatvtkCommand() {
	argumentMachine = NULL;
	
	configDataType = NULL;
	configReader = NULL;

	stopState = NULL;
	nullState = NULL;
	failState = NULL;
	
    _sceneWindow = NULL;
    _plotComponent = NULL;
}

MatvtkCommand::~MatvtkCommand() {
	if (argumentMachine) delete argumentMachine;
    
    // free return data memory
    std::vector<DataSerializer*>::iterator it;
    for(it = this->_returnValues.begin(); it != this->_returnValues.end(); ++it) {

        // delete data serializer
        delete (*it);
    }
	
}

bool MatvtkCommand::readArguments(DataPort *port) {
	this->argumentMachine = new StateMachine();
	
	this->stopState = new StateDataType(port);
	this->nullState = new StateSuccess;
	this->failState = new StateFail;
	
	this->argumentMachine->setFailState(failState);
	
	this->argumentMachine->addEndState(stopState);
	this->argumentMachine->addEndState(nullState);

	
	
	configDataType = new StateDataType(port);
	configReader = new StateDataRead<StringSerializer>(port);
	this->argumentMachine->addTransition(configDataType, matvtkProtocol::DATA_CONFIG, configReader);
    this->argumentMachine->addTransition(configDataType, matvtkProtocol::DATA_STOP, nullState);
	this->argumentMachine->addTransition(configReader, stopState);
	
	
	// setup the state machine for the specific command
	this->setupArgumentMachine(port);
	
	// read data from DataPort using the state machine
	int argumentResult = this->argumentMachine->run();
	
	if(configReader->GetSerializer()) {
		config.Deserialize(configReader->GetSerializer()->dataString());
	}
	
	qDebug() << "reading arguments result: " << argumentResult;
	return (argumentResult == StateCondition::Succeeded);
}

void MatvtkCommand::addLastArgument(State *typeState, int data, State *lastState) {
	this->argumentMachine->addTransition(typeState, matvtkProtocol::DATA_STOP, nullState);
	this->argumentMachine->addTransition(typeState, matvtkProtocol::DATA_CONFIG, configReader);
	this->argumentMachine->addTransition(typeState, data, lastState);

	//this->addLastArgument(lastState, StateCondition::Succeeded, configDataType);
	this->argumentMachine->addTransition(lastState, stopState);
}


bool MatvtkCommand::execute(SceneWindow *win, PlotComponent *component) {
	qDebug() << "executing command on component " << component <<  " in window " << win;
	
	return true;
}

bool MatvtkCommand::writeReturnData(DataPort *port) {
    
    std::vector<DataSerializer*>::iterator it;
    
    for(it=this->_returnValues.begin(); it != this->_returnValues.end(); ++it) {
        Debug("writing return data:");
        DataSerializer *serializer = (*it);
        serializer->produceData(port);
    }
    
    return true;
}

void MatvtkCommand::submitComponent(PlotComponent *component, SceneWindow *window=NULL) {

    
    this->_plotComponent = component;
    if(this->_plotComponent) {
        this->_plotComponent->Configure(&config);
    }
    
    
    if(window) {
        this->_sceneWindow = window;
        this->_sceneWindow->Configure(&config);
    }
    
    emit(componentCreated(component));
    
    
}

/*
static MatvtkCommand* newCommand(matvtkProtocol::MinorCommand minor) {

	return NULL;
} */

