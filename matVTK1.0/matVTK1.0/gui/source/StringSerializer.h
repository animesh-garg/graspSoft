/*
 *  StringReader.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 03.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef STRINGSERIALIZER_H
#define STRINGSERIALIZER_H

#include "DataSerializer.h"
#include <matvtkProtocol.h>

#include <string>
#include <sstream>


class StringSerializer : public DataSerializer {
	
public:
	static StringSerializer* newSerializer(matvtkProtocol::ValueType type) {
        StringSerializer *s = new StringSerializer();
        s->_valueType = type;
		s->_dataType = matvtkProtocol::DATA_STRING;
		return s;
	}
	
    StringSerializer()  {
        this->_valueType = matvtkProtocol::VALUE_STRING;
        this->_dataType = matvtkProtocol::DATA_STRING;
    }
    
    StringSerializer(std::string str,  matvtkProtocol::DataType data=matvtkProtocol::DATA_STRING) : _dataString(str) {
        this->_valueType = matvtkProtocol::VALUE_STRING;
        this->_dataType = data;
    }
    
	std::string dataString();
    void SetDataString(std::string dataString) {
        this->_dataString = dataString;
    }
	
	bool acceptsData(matvtkProtocol::DataType data) {
		return data == matvtkProtocol::DATA_STRING || data == matvtkProtocol::DATA_CONFIG;
	}
	
	bool acceptsValue(matvtkProtocol::ValueType value) {
		return value == matvtkProtocol::VALUE_STRING;
	}
	
	bool consumeData(DataPort *port);
	
protected:	
	virtual DataSize readLength();
	virtual bool readData();
    
    virtual void writeLength();
	virtual void writeData();


	
private:
	std::string _dataString;
	//std::istringstream _stringBuffer;
	
};



#endif