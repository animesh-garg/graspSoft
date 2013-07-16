/*
 *  ImageDataSerializer.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 10.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "VtkDataSerializer.h"

//#include <Constants.h>
#include <qdebug.h>
#include <iostream>
#include <iomanip>

#include <vtkPoints.h>
#include <vtkImageData.h>
#include <vtkCellArray.h>
#include <vtkIdTypeArray.h>

bool ImageDataSerializer::readData() {
		
	this->_imageData = vtkImageData::New();
	_imageData->SetDimensions(this->_dimensions[0], this->_dimensions[1], this->_dimensions[2]);
	
	_imageData->SetScalarType(this->_valueType);
	_imageData->SetNumberOfScalarComponents(1);
	qDebug() << "image data for type: " << _imageData->GetScalarTypeAsString();
	
	int *dims = _imageData->GetDimensions();
	qDebug() << "image data dims: " << dims[0] << dims[1] << dims[2];
	
	
	_imageData->AllocateScalars();
	qDebug() << "image data memory size: " << _imageData->GetActualMemorySize()*1024L;
	
	dims = _imageData->GetDimensions();
	qDebug() << "image data dims: " << dims[0] << dims[1] << dims[2];

	qDebug() << "components:" << _imageData->GetNumberOfScalarComponents();
	qDebug() << "scalar size:" << _imageData->GetScalarSize();
	
	//_imageData->PrepareForNewData();
	
	DataSize byteCount = this->_dataLength * this->_symbolLength;
	bool allRead = false;
	
	//if(_imageData->GetActualMemorySize() == byteCount) {
		void *dataPtr = _imageData->GetScalarPointer();
		DataSize dataRead = this->_port->read(byteCount, dataPtr);
		allRead = (byteCount == dataRead);
	//}
	
	_imageData->Modified();
	
	if(!allRead) {
		_imageData->Delete();
		_imageData = NULL;
	}
	
	return allRead;
}

void ImageDataSerializer::writeLength() {
    // volume has 3 dimensions
    this->_port->write( (DataSize) 3);
    
    this->_dataLength = 0;
    
    for(DataSize i=0; i<3; i++) {
        DataSize dim = this->_dimensions[i];
        this->_port->write(dim);
        this->_dataLength += (dim * this->_imageData->GetScalarSize());
    }
}


void ImageDataSerializer::writeData() {

    this->_port->write(this->_dataLength, this->_imageData->GetScalarPointer());

}

bool PointDataSerializer::consumeData(DataPort *port) {

	bool consumed = ArraySerializer::consumeData(port);
	
	if (!consumed) {
        this->_points = NULL;
		return false;
	}
	
	this->_points = vtkPoints::New();
	this->_points->SetData(this->_array);
	
	return true;
}


bool CellDataSerializer::readData() {

	this->_cells = vtkCellArray::New();
	this->_cells->Allocate(this->_dataLength*this->_symbolLength);
	this->_cells->Initialize();
	
	
	//DataSize byteLen =  this->_dataLength * this->_symbolLength;
	
	//vtkIdType *idData = new vtkIdType[this->_dataLength];
	//DataSize readLen = this->_port->read(byteLen, idData);
	
	
	const DataSize maxLines = this->_dimensions[0];
    const DataSize pointNum = this->_dimensions[1];
    vtkIdType *pointBuffer = new vtkIdType[pointNum];
	
	std::cout << "reading " << maxLines << " cells, with " << pointNum << " points" << std::endl;
	
	for (DataSize lines=0; lines<maxLines; lines++) {

		DataSize readLen = this->_port->read(pointNum*this->_symbolLength, pointBuffer);
		if(readLen != pointNum*this->_symbolLength) {
			std::cerr << "unable to read all cell values, aborting" << std::endl;
			this->_cells->Delete();
			this->_cells = NULL;
            delete [] pointBuffer;
			return false;
		}
		
		this->_cells->InsertNextCell(pointNum, pointBuffer);
        //qDebug() << "cells" << pointNum << ":" << pointBuffer[0] << pointBuffer[1] << pointBuffer[2]; 
        }
	
        delete[] pointBuffer;

	return true;
}

void CellDataSerializer::writeLength() {
    
    this->_port->write( (DataSize) 2);
    this->_port->write( (DataSize) this->_cells->GetMaxCellSize());
    this->_port->write( (DataSize) this->_cells->GetNumberOfCells());
}


void CellDataSerializer::writeData() {

    
    // this iteration traversal is not threadsafe and might be used during 
    // the drawing itself
    /*
    vtkIdType points;
    vtkIdType *idxList;

    this->_cells->InitTraversal();
    while(this->_cells->GetNextCell(points, idxList)) {
        this->_port->write(points*sizeof(vtkIdType), idxList);
        bytes += (points*sizeof(vtkIdType));
        count += points;
        qDebug() << "cells:" << idxList[0] << idxList[1] << idxList[2]; 
    }*/
    
    // this iteration should have no side effects
    vtkIdTypeArray *array = this->_cells->GetData();
    vtkIdType arrayLen = array->GetNumberOfTuples();

    
    vtkIdType pos=0; 
    while(pos<arrayLen) {
        vtkIdType len = array->GetTuple1(pos++);
        
        for(vtkIdType i=0; i<len; i++) 
            this->_port->write<vtkIdType>((vtkIdType) array->GetTuple1(pos++));
        
        //pos += (len + 1);
    }
    
        
}

bool ColorTransferFunctionDataSerializer::readData() {

	DataSize numLines = this->_dimensions[0];
	DataSize vrgb = this->_dimensions[1];
	
	// must be 4 values per entry: value, red, green, blue
	if(vrgb != 4) {
		this->_port->skip(this->_dataLength * this->_symbolLength);
		return false;
	}
	
	this->_colorTransferFunction = vtkColorTransferFunction::New();
	
        double *buff = new double[vrgb];
	for (DataSize l=0; l<numLines; l++) {
		this->_port->read(vrgb*this->_symbolLength, buff);
		this->_colorTransferFunction->AddRGBPoint(buff[0], buff[1], buff[2], buff[3]);
		std::cout << "reading v/rgb: " << std::setprecision(5) <<  buff[0] << " " << buff[1] << " " << buff[2] << " " << buff[3] << std::endl;
	}
	
        delete[] buff;

	this->_colorTransferFunction->Build();
	return true;
}

void ColorTransferFunctionDataSerializer::writeLength() {
    // lines
    this->_port->write( (DataSize) this->_colorTransferFunction->GetSize());
    // columns
    this->_port->write( (DataSize) 4);
}


void ColorTransferFunctionDataSerializer::writeData() {

    double buffer[6];
    for (int i=0; i<this->_colorTransferFunction->GetSize(); i++) {
        this->_colorTransferFunction->GetNodeValue(i, buffer);
        
        // buffer values: location (X), R, G, and B values, midpoint, and sharpness
        // see: http://www.vtk.org/doc/release/5.6/html/a00331.html

        // we only serialize: location(X), R, G, B
        this->_port->write(sizeof(double)*4, buffer);
    }
}




