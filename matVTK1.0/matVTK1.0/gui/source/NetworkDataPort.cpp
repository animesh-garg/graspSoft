/*
 *  NetworkDataPort.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 28.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "NetworkDataPort.h"

#include <Constants.h>
#include <cstring>
#include <qtcpsocket.h>



DataSize NetworkDataPort::readData(DataSize len, const void *data) {

	//void *tmp = const_cast<void*>(data);
	
	DataSize remainingLength = len;

	// setting a too "large" chunkLen can cause some wait time 
    // on short commands (when less data is transmitted
    DataSize chunkLen = 16*1024*1024; // read in 16mb chunks
	char* chunkPos = (char*) data;
	
	
	while(remainingLength > 0) {
		//Debug("looping...");
		//this->socket->waitForReadyRead(1);
        
		DataSize lenToRead = remainingLength > chunkLen ? chunkLen : remainingLength;
        
        //qDebug() << "want to read" << lenToRead;
        DataSize actualRead = this->socket->read(chunkPos, lenToRead);
	
		chunkPos += actualRead;
		remainingLength -= actualRead;
		
		if(actualRead < lenToRead) {
            //Debug("waiting for data...");
			if(!this->socket->waitForReadyRead(1000*50)) { // wait 50s
                Debug("breaking, no data!");
				break; // leave the reading loop, reading has failed
			}
		}
	}
	
	return len;
}


DataSize NetworkDataPort::writeData(DataSize len, const void *data) {

	DataSize writeCount = 0;
	DataSize lastWrite = 0;
	
	
	do {
		lastWrite = socket->write((const char*)data, len);
		if (lastWrite > 0) {
			writeCount += lastWrite;
		}
	} while (writeCount < len && lastWrite>0);
	
	socket->flush();
	socket->waitForBytesWritten(-1);
	
	return writeCount;
}

bool NetworkDataPort::open() {
	socket->open(QIODevice::ReadWrite);
	
	return socket->isOpen() && socket->isReadable() && socket->isWritable();
}

void NetworkDataPort::flush() {
    this->socket->flush();
}

bool NetworkDataPort::close() {
	socket->close();
	
	return socket->isOpen() == FALSE;
}

bool NetworkDataPort::isConnected() {
    return this->socket->isOpen();
}
