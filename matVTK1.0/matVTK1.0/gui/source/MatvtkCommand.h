/*
 *  MatvtkCommand.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 05.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MATVTKCOMMAND_H
#define MATVTKCOMMAND_H

#include <qobject.h>

#include <Constants.h>
#include <StateMachine.h>
#include <matvtkProtocol.h>

//#include <StateDataPort.h>

#include <StringSerializer.h>
#include <SceneConfiguration.h>
#include <SceneWindow.h>

template <typename T> class StateDataRead;

class StateDataType;
class PlotComponent;
class DataPort;


class MatvtkCommand : public QObject {
	Q_OBJECT;
public:
	
	MatvtkCommand();
	/*: argumentMachine(NULL), stopState(NULL), nullState(NULL), faileState(NULL), configReader(NULL), configDataType(NULL) {´
	}*/

	static MatvtkCommand* newCommand(matvtkProtocol::MinorCommand minor);
    
	virtual bool readArguments(DataPort *port);
	virtual bool execute(SceneWindow *win, PlotComponent *component=NULL);
    virtual bool writeReturnData(DataPort *port);
    
    SceneWindow *sceneWindow() { return this->_sceneWindow; }
    PlotComponent *plotComponent() { return this->_plotComponent; }


                         
	virtual ~MatvtkCommand();

	signals:
	void componentCreated(PlotComponent *comp);
	
protected:
	virtual void setupArgumentMachine(DataPort *port) = 0;
    void addLastArgument(State *typeState, int outcome, State *lastState);
    
    // add newly created plot component to rendering window
    void submitComponent(PlotComponent *component, SceneWindow *window);

    
	StateMachine *argumentMachine;
		
	StateDataRead<StringSerializer> *configReader;
	StateDataType *configDataType;
	StateDataType *stopState;
	
	StateSuccess *nullState;
	StateFail *failState;
	
	SceneConfiguration config;
    
    SceneWindow *_sceneWindow;
    PlotComponent *_plotComponent;
    
    std::vector<DataSerializer*> _returnValues;
};


#endif
