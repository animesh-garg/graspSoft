/*
 *  GuiServer.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 21.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <qobject.h>
#include <qtcpserver.h>
#include <qhostaddress.h>

#include <matvtkProtocol.h>

class GuiServer : public QTcpServer {

	Q_OBJECT;
	
	public:
	GuiServer();
	~GuiServer();
	virtual void incomingConnection(int socketDescriptor);
	
	
	public slots:
		void startup();
		void shutdown();
		void handleConnection();
	void openWindow(RegistryHandle* handle);
	
	protected:
	
	private:
	//QHostAddress serverAdress;
	//qint16 serverPort;

};
