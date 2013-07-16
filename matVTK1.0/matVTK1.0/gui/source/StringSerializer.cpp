/*
 *  StringReader.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 03.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "StringSerializer.h"

#include <iostream>
#include <Constants.h>
#include "DataPort.h"

DataSize StringSerializer::readLength() {
		
	DataSize len = 0;
	len = _port->read<DataSize>();
	Debug("  expected string size: %llu", len);
	
	_dataLength = len;
	
	return len;
}

bool StringSerializer::consumeData(DataPort *port) {
	
	this->_port = port;
	
	DataSize len = this->readLength();
	
	if(len > 0) {
		bool success = this->readData();
		
		/*
		 switch (this->_valueType) {
		 vtkTemplateMacro(success = this->readData<VTK_TT>());
		 }
		 */
		
		std::clog << " reading success: " << success << std::endl;
		return success;
	}
	
	
	
	return true;
}

bool StringSerializer::readData() {
	
	this->_dataString.resize(this->_dataLength);
	
	DataSize readLen = this->_port->read(this->_dataLength, _dataString.data());
	
	return (this->_dataLength == readLen);
}

std::string StringSerializer::dataString() {
	//return this->_stringBuffer.str();
	return _dataString;
}

void StringSerializer::writeLength() {
    DataSize len = this->_dataString.length();

    // one dimension of length strlen()
    this->_port->write((DataSize) 1);
    this->_port->write(len);
}


void StringSerializer::writeData() {
    
    this->_port->write(this->_dataString.length(), this->_dataString.c_str());
}
