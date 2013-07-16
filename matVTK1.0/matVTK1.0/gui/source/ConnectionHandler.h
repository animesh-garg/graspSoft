/*
 *  ConnectionHandler.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 22.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include <qthread.h>
#include <qtcpsocket.h>

class StateMachine;
class DataPort;
//class MatvtkCommand;

class ConnectionHandler : public QThread {

	Q_OBJECT;
	
public:
	ConnectionHandler(int socketDescriptor, QObject *parent) :  QThread(parent), socketDescriptor(socketDescriptor) {
		qDebug() << "##### new connection #####";
	}
	
	void run();
	
signals:
	void error(QTcpSocket::SocketError socketError);

public slots:
	void socketIsReady();
	
private:
	StateMachine *createProtocolMachine(DataPort *port);
	
	int socketDescriptor;
	
	//friend class MatvtkCommand;
};


#endif // CONNECTIONHANDLER_H

