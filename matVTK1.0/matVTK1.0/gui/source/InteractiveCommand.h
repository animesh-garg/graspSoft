//
//  InteractiveCommand.h
//  matvtk_guiserver
//
//  Created by Erich Birngruber on 15.09.10.
//  Copyright (c) 2010 __MyCompanyName__. All rights reserved.
//

#include <MatvtkCommand.h>
#include <qobject.h>
#include <SceneWindow.h>
#include <qmutex.h>

class QAction;

class InteractiveCommand : public MatvtkCommand {
    Q_OBJECT
public:
    
	static MatvtkCommand* newCommand(matvtkProtocol::MinorCommand minor);
    
    virtual void setupArgumentMachine(DataPort *port) { 
        this->_port = port;
        this->argumentMachine->setInitialState(this->nullState);
    }
    
    virtual bool execute(SceneWindow *win, PlotComponent *component);

    


public slots:
    void letClientGo();

    
signals:
    void holdingClient();
    
        
protected:
    void holdClient();
    
    
    bool _holdClient;
    DataPort *_port;
    SceneWindow *_window;
    QAction *_continueAction;
};



class SelectPointsCommand : public InteractiveCommand {

public:
    SelectPointsCommand() { 
        //this->name = "SelectPointsCommand";
    }
    virtual ~SelectPointsCommand() { }
    
    void setupArgumentMachine(DataPort *port) { 
        InteractiveCommand::setupArgumentMachine(port);
    }
    
    bool execute(SceneWindow *win, PlotComponent *component) {
        InteractiveCommand::execute(win, component);
        return true;
    }
    
};


class WaitCommand : public InteractiveCommand {

public:
    WaitCommand() { 
        //this->name = "WaitCommand";
    }
    virtual ~WaitCommand() { }
    
    // we need no arguments
    void setupArgumentMachine(DataPort *port) { 
        InteractiveCommand::setupArgumentMachine(port);
    }
    
    bool execute(SceneWindow *win, PlotComponent *component) {
        InteractiveCommand::execute(win, component);
        return true;
    }
};




