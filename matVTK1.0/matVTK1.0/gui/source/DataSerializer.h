/*
 *  DataReader.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 03.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DATASERIALIZER_H
#define DATASERIALIZER_H

#include <stddef.h>
#include "DataPort.h"
#include "matvtkProtocol.h"

class DataSerializer  {
	
public:
    DataSerializer() : _valueType(matvtkProtocol::VALUE_VOID), _dataType(matvtkProtocol::DATA_ERROR), _dataLength(0), _symbolLength(0) {
        
    }
    
	virtual bool consumeData(DataPort *port);
    virtual bool produceData(DataPort *port);
    
	virtual bool acceptsData(matvtkProtocol::DataType dataType) = 0;
	virtual bool acceptsValue(matvtkProtocol::ValueType valueType) = 0;
	virtual ~DataSerializer();
	
protected:
	virtual DataSize readLength() = 0;
	virtual bool readData() = 0;

    virtual void writeLength() = 0;
	virtual void writeData() = 0;

	
	DataPort *_port;
	matvtkProtocol::ValueType _valueType;
	
    matvtkProtocol::DataType _dataType;
	DataSize _dataLength;
	DataSize _symbolLength;
};

#endif // DATASERIALIZER_H
