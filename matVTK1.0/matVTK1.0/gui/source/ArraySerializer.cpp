/*
 *  MatrixReader.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 03.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ArraySerializer.h"

#include <iostream>



DataSize ArraySerializer::readLength() {
    
    
    DataPort *aPort = this->_port;
    DataSize dimensionsNum = aPort->read<DataSize>();
    
    std::cout << " reading array with " << dimensionsNum << " dimensions" << std::endl;		
    this->_dimensions = new DataSize[dimensionsNum];
    
    DataSize totalLength = 1;
    for(DataSize i=0; i<dimensionsNum; i++) {
        this->_dimensions[i] = aPort->read<DataSize>();
        std::clog << " dimension " << i << ": " << this->_dimensions[i] << std::endl;
        totalLength *= this->_dimensions[i];
    }
    
    std::clog << " total length: " << totalLength << std::endl;
    
    this->_dataLength = totalLength;
    
    return totalLength;
}



bool ArraySerializer::readData() {
    
    
    DataSize byteLength = this->_dataLength * this->_symbolLength;
    void* buffer = malloc(byteLength);
    
    this->_port->read(byteLength, buffer);
    
    this->_array = vtkDataArray::CreateDataArray(this->_valueType);
    this->_array->SetNumberOfTuples(this->_dimensions[0]);
    this->_array->SetNumberOfComponents(this->_dimensions[1]);
    
    this->_array->SetVoidArray((void*)buffer, this->_dataLength, 0);
    
    
    /*
     for(vtkIdType i=0; i<_array->GetNumberOfTuples(); i++) {
     
     double *tuple = _array->GetTuple(i);
     std::cout << " line " << setw(3) << i+1 << ": ";
     
     for(vtkIdType c=0; c<_array->GetNumberOfComponents(); c++) {
     std::cout << setiosflags(ios::fixed) <<  setprecision(4) << tuple[c] << " ";
     }
     std::cout << std::endl;
     }
     */
    
    return true;
}

void ArraySerializer::writeLength() {
    
    this->_port->write( (DataSize) 2 );
    
    this->_port->write( (DataSize) this->_array->GetNumberOfTuples());
    this->_port->write( (DataSize) this->_array->GetNumberOfComponents());
    
}


void ArraySerializer::writeData() {

    DataSize len = this->_array->GetNumberOfTuples() * this->_array->GetNumberOfComponents() * this->_array->GetElementComponentSize();
    this->_port->write(len, this->_array->GetVoidPointer(0));
    
}



