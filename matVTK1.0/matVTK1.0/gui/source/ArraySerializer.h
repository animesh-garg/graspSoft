/*
 *  MatrixReader.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 03.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef ARRAYSERIALIZER_H
#define ARRAYSERIALIZER_H

#include "DataSerializer.h"

#include "Constants.h"

#include <iostream>
#include <vtkDataArray.h>

#include <iomanip>
#include <matvtkProtocol.h>



class ArraySerializer : public DataSerializer {

public:
	static ArraySerializer* newSerializer(matvtkProtocol::ValueType value) {

		ArraySerializer *s = NULL;
		Debug() << "new ArraySerializer for value type" << value;
		s = new ArraySerializer(value);
		
		return s;
	}
	
	
	ArraySerializer(matvtkProtocol::ValueType type) : _array(NULL) {
		//this->_symbolLength = sizeof(T);
		
		switch (type) {
			vtkTemplateMacro(this->_symbolLength=sizeof(VTK_TT));
                
            case VTK_BIT:
                this->_symbolLength = 1;
                
            default:
                this->_symbolLength = 0;
		}
		
		this->_valueType = type;
	}
	
	bool acceptsData(matvtkProtocol::DataType data) {
		return data == matvtkProtocol::DATA_MATRIX;
	}
	
	bool acceptsValue(matvtkProtocol::ValueType value) {
		return this->_valueType == value;
	}
	
	virtual ~ArraySerializer() {
		Debug("BaseArraySerializer Destructing");
		delete[] this->_dimensions;
		if(_array) _array->Delete();
	}
	
	vtkDataArray* GetDataArray() { return this->_array; }

protected:
	virtual DataSize readLength();
	virtual bool readData();
  	
    virtual void writeLength();
	virtual void writeData();

	
	vtkDataArray* _array;
	DataSize* _dimensions;
	
};



#endif // MATRIXREADER_H
