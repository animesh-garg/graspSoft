/*
 *  DataReader.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 03.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "DataSerializer.h"

#include <stdio.h>
#include <iostream>

#include <vtkSetGet.h>
#include <assert.h>



bool DataSerializer::consumeData(DataPort *port) {

	this->_port = port;
	
	DataSize len = this->readLength();

	if(len > 0) {
		bool success = this->readData();
	
		std::clog << " reading success: " << success << std::endl;
		return success;
	}
	return false;
}

bool DataSerializer::produceData(DataPort *port) {
    
    this->_port = port;
    
    // send data type (array, volume, color, ...)
    
    
    // write value type: float, double, char, int64, ...
    this->_port->write( (CommandData) this->_valueType);
    
    // write data to wire
    this->writeLength();
    this->_port->flush();
    
    this->writeData();
    this->_port->flush();
    
    return true;
}

DataSerializer::~DataSerializer() {
    
}