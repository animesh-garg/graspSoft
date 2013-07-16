/*
 *  GuiServer.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 21.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "GuiServer.h"
#include "Constants.h"

#include <qtcpsocket.h>

#include <qapplication.h>
#include <qmessagebox.h>

#include <matvtkProtocol.h>
#include <SceneWindow.h>
#include <ConnectionHandler.h>

GuiServer::GuiServer() {

}


GuiServer::~GuiServer() {
	
}


void GuiServer::handleConnection() {
	
	qDebug("processing connection. (direct server handler)");
	
}


// overriding this, to use threaded server
void GuiServer::incomingConnection(int socketDescriptor) {

	ConnectionHandler *thread = new ConnectionHandler(socketDescriptor, this);
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	thread->start();
}

void GuiServer::startup() {
	qDebug("start listening for connections...");
	
	qint16 serverPort = matvtkProtocol::DEFAULT_NETWORK_PORT;
	QHostAddress serverAdress = QHostAddress::LocalHost;
	

	if (!this->listen(serverAdress, serverPort)) {
		QMessageBox::critical(NULL, tr("matVTK Server"),
							  tr("Unable to start the server: %1.").arg(this->errorString()));
		return;
	}
	
	qDebug() << tr("The server is running on port %1.").arg(this->serverPort());
	
	connect(this, SIGNAL(newConnection()), this, SLOT(handleConnection()));
}


void GuiServer::shutdown() {
	Debug("shutting down server...");
	this->close();
	
	
	//SceneRegistry::instance()->closeAllWindows();
}


void GuiServer::openWindow(RegistryHandle *handle) {
	SceneWindow *win = SceneWindow::newWindow();
	*handle = SceneRegistry::instance()->handleFor(win);
}
