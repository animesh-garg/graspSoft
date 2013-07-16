/*
 *  DataPort.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 28.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "DataPort.h"
#include "matvtkProtocol.h"
#include <iostream>
#include <ostream>

#ifdef unix 
#include <arpa/inet.h>
#endif

#ifdef WIN32
#include <Winsock2.h>
#endif 

#include <stdlib.h>


DataPort::DataPort() {

}

#pragma mark reading functions

template <typename T> 
bool DataPort::read(T *t) {
	
	//std::clog << "reading * of " << typeid(T).name() << std::endl;
	
	DataSize len = sizeof(T);
	DataSize readlen = readData(len, t);
	
	return len == readlen;
}


template<>
bool DataPort::read<CommandData>(CommandData *d) {
	//std::clog << "reading CommandData " << std::endl;
	
	DataSize len = sizeof(CommandData);
	DataSize readlen = readData(len, d);

	*d = ntohs(*d);
	
	return len == readlen;
}


template<>
bool DataPort::read<DataSize>(DataSize *s) {
	//std::clog << "reading DataSize " << std::endl;

	
	DataSize len = sizeof(DataSize);
	DataSize readlen = readData(len, s);
	
	*s = swap(*s);
	
	return len == readlen;
}

#pragma mark write functions

bool DataPort::write(CommandData s) {
	DataSize typelen = sizeof(s);
	
	short netData = htons(s);
	
	//std::clog << "sending short: " << s << " converted to " << netData << std::endl;
	
	DataSize written = writeData(typelen, &netData);
	
	return written == typelen;
}


bool DataPort::write(DataSize s) {
	DataSize typelen = sizeof(s);
	
	DataSize netData = swap(s);
	
	//std::clog << "sending DataSize: " << s << " converted to " << netData << std::endl;
	
	DataSize written = writeData(typelen, &netData);
	
	return written == typelen;
}





void DataPort::skip(DataSize totalLen) {
	DataSize buffSize = 8192;
	void* buff = malloc(buffSize);

	DataSize remaining = totalLen;
	int lastRead = 0;
	
	while (remaining > 0 && lastRead != -1) {
		DataSize sizeToRead = (remaining > buffSize) ? buffSize : remaining;
		lastRead = this->readData(sizeToRead, buff);
		remaining -= lastRead;
	}
	
	free(buff);
	
}

DataSize DataPort::write(DataSize len, const void* buffer) {
	return this->writeData(len, buffer);
}

DataSize DataPort::read(DataSize len, const void* buffer) {
	return this->readData(len, buffer);
}



