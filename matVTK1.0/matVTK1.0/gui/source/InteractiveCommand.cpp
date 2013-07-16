//
//  InteractiveCommand.cpp
//  matvtk_guiserver
//
//  Created by Erich Birngruber on 15.09.10.
//  Copyright (c) 2010 __MyCompanyName__. All rights reserved.
//

#include "InteractiveCommand.h"

#include "matvtkProtocol.h"
#include "Constants.h"
#include "DataPort.h"

#include <qthread.h>
#include <qwaitcondition.h>
#include <qaction.h>
#include <qapplication.h>

// factory method for InteractiveCommand Instances
MatvtkCommand* InteractiveCommand::newCommand(matvtkProtocol::MinorCommand minor) {

    MatvtkCommand *command = NULL;
    switch(minor) {

        case matvtkProtocol::SELECTPOINTS:
            command = new SelectPointsCommand;
            break;
            
        case matvtkProtocol::WAIT:
            command = new WaitCommand;
            break;

        default:
            command = NULL;
    }
    
    return command;
}

bool InteractiveCommand::execute(SceneWindow *win, PlotComponent *component) {
    this->_window = win;
    component = NULL;
    
    QAction *action = new QAction(QString("&Continue"), NULL);

    action->moveToThread(QApplication::instance()->thread());
    this->moveToThread(QApplication::instance()->thread());
       
    this->_window->addWindowAction(action);
    
    

    QObject::connect(win, SIGNAL(continueClient()), this, SLOT(letClientGo()), Qt::QueuedConnection);
    QObject::connect(action, SIGNAL(triggered()), this, SLOT(letClientGo()), Qt::QueuedConnection);
    
    
    
    this->holdClient();

    /// FIXME: handle situation, when window is closed but vtkwait() is still running
    //QObject::disconnect(win, SIGNAL(continueClient()), this, SLOT(letClientGo()));
    //QObject::disconnect(action, SIGNAL(triggered()), this, SLOT(letClientGo()));
    
    this->_window->removeWindowAction(action);
    delete action;
    
    //Debug("selecting points");
    return true;
}



void InteractiveCommand::holdClient() {
    this->_holdClient = true;
    emit holdingClient();
    // network loop
    //DataSize dummy;
    DataSize count = 0;
    while(this->_holdClient && this->_port->isConnected()) {
        Debug("zzz...");
        
        // this is used to slow down the server... DIRTY HACK!
        CommandData d = this->_port->read<CommandData>();
        qDebug() << "read value:" << d;
        
        this->_port->write( (CommandData) 1);
        this->_port->flush();
        
        
        qDebug() << "count:" << ++count;
    }
    
    
    // this is the signal to the client, that he can continue
    this->_port->read<CommandData>();
    this->_port->write( (CommandData) 0);
    this->_port->flush();
}

void InteractiveCommand::letClientGo() {
    
    Debug("letting client go.");
    this->_holdClient = false;
}

