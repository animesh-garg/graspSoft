/*
 *  ConnectionHandler.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 22.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ConnectionHandler.h"
#include <qtcpsocket.h>
#include <qabstractsocket.h>
#include <qhostaddress.h>

#include <State.h>
#include <StateMachine.h>
#include <NetworkDataPort.h>
#include <StateDataPort.h>
#include <matvtkProtocol.h>


void ConnectionHandler::run() {
	QTcpSocket tcpSocket;

	if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
		qDebug() << "##### socket error: " << tcpSocket.error();
		qDebug();
		emit error(tcpSocket.error());
		return;
	}
	
	QString cleanPeer = tcpSocket.peerAddress().toString().replace('"', "");

	qDebug() << "client: " << cleanPeer;
	//connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT(socketIsReady()));

	// switch do abstract data reader/writer interface
	DataPort *dataPort = new NetworkDataPort(&tcpSocket);
	
	
	StateMachine *machine = this->createProtocolMachine(dataPort);
	machine->run();
	
	// do something right here
	//dataPort->write(matvtkProtocol::START);
	//dataPort->write(matvtkProtocol::VERSION);
	
	// read first 2 short from stream
	// switch-case over value
	
	// start parser for respective command
	
	delete machine;
	delete dataPort;

	
    /*
     * connection is closed automagically when tcpsocket goes out of scope
     *
	// close tcp connection
	tcpSocket.disconnectFromHost();
	
    */
	// join thread, if we have to wait for connection to close
	if(tcpSocket.state() != QAbstractSocket::UnconnectedState)
		tcpSocket.waitForDisconnected();
	
    
	qDebug("##### done, connection closed.");
	qDebug();
}

StateMachine* ConnectionHandler::createProtocolMachine(DataPort *port) {
	StateMachine *machine = new StateMachine();
	
	StateConnectionSetup *setupState = new StateConnectionSetup(port);
	StateConnectionClose *closeState = new StateConnectionClose(port);
	
	StateCommand *commandState = new StateCommand(port);
	//StateDataRead *readerState = new StateDataRead(port);
	
	machine->setInitialState(setupState);
	machine->addEndState(closeState);
	machine->setFailState(closeState);
	
	machine->addTransition(setupState, StateCondition::Succeeded, commandState);
	machine->addTransition(commandState, StateCondition::Succeeded, closeState);
	//machine->addTransition(readerState, StateCondition::Succeeded, closeState);
	
	return machine;
}


void ConnectionHandler::socketIsReady() {

	std::cout << "read ready" << std::endl;
}
