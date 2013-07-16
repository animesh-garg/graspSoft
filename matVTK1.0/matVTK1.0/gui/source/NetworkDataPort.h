/*
 *  NetworkDataPort.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 28.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include <DataPort.h>

#include <qtcpsocket.h>


class NetworkDataPort : public DataPort {

public:
	NetworkDataPort(QTcpSocket *s) : socket(s) {
		socket->setReadBufferSize(16*1024*1024);
	}
    
    virtual void flush();
    virtual bool isConnected();
	
protected:
	virtual DataSize readData(DataSize len, const void* data);
	virtual DataSize writeData(DataSize len, const void* data);
	
	virtual bool open();
	virtual bool close();

	
private:
	QTcpSocket *socket;
};

