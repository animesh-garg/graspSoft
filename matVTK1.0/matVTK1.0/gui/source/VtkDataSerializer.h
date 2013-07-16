/*
 *  ImageDataSerializer.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 10.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef VTKDATASERIALIZER_H
#define VTKDATASERIALIZER_H

#include <Constants.h>
#include <ArraySerializer.h>
#include <vtkCellArray.h>
#include <vtkColorTransferFunction.h>
#include <vtkPoints.h>

//class vtkCellArray;
#include <vtkImageData.h>


class ImageDataSerializer : public ArraySerializer {
	
public:
	ImageDataSerializer(matvtkProtocol::ValueType value) : ArraySerializer(value), _imageData(NULL) {

	}
	
	virtual ~ImageDataSerializer() {
		if(_imageData) _imageData->Delete();
	}
	
	static ImageDataSerializer* newSerializer(matvtkProtocol::ValueType value) {
		Debug("new image data serializer");
		ImageDataSerializer *s = NULL;
		s = new ImageDataSerializer(value);
		
		return s;
	}
	
	
	vtkImageData *GetImageData() {return this->_imageData; };
	
	bool acceptsData(matvtkProtocol::DataType data) {
		return data == matvtkProtocol::DATA_VOLUME;
	}
	
	bool acceptsValue(matvtkProtocol::ValueType value) {
		return this->_valueType == value;
	}
	
	
protected:
	virtual bool readData();
	
    virtual void writeLength();
	virtual void writeData();

    
	vtkImageData *_imageData;
	
};


class PointDataSerializer : public ArraySerializer {
	
public:
	static PointDataSerializer *newSerializer(matvtkProtocol::ValueType value) {
		return new PointDataSerializer(value);
	}
	
	PointDataSerializer(matvtkProtocol::ValueType value) : ArraySerializer(value), _points(NULL) { }
	
	virtual ~PointDataSerializer() {
		Debug("PointDataSerializer destructing");
		if(_points) _points->Delete();
	}
	
	bool consumeData(DataPort *port);
	vtkPoints* GetPoints() { return this->_points; }
	
	bool acceptsData(matvtkProtocol::DataType data) {
		return data == matvtkProtocol::DATA_POINTS;
	}
	
	bool acceptsValue(matvtkProtocol::ValueType value) {
		return this->_valueType == value;
	}
	
	
protected:
    //virtual void writeLength();
	//virtual void writeData();

    
	vtkPoints *_points;
};


class CellDataSerializer : public ArraySerializer {

public:
	static CellDataSerializer* newSerializer(matvtkProtocol::ValueType value) {
		return new CellDataSerializer(value);
	}
	
	CellDataSerializer() : ArraySerializer(matvtkProtocol::VALUE_IDTYPE), _base(NULL), _cells(NULL) {
		
	}
	
	CellDataSerializer(matvtkProtocol::ValueType type) :  ArraySerializer(type), _base(NULL), _cells(NULL) {
	}
    
    CellDataSerializer(vtkCellArray *cellArray) : ArraySerializer(matvtkProtocol::VALUE_IDTYPE), _cells(cellArray) {
        this->_base = vtkObjectBase::New();
        this->_cells->Register(_base);
    }
	
	virtual bool readData();
	vtkCellArray* GetCells() { return this->_cells; }
	
	virtual ~CellDataSerializer() {
        

        if(this->_base && this->_cells) this->_cells->UnRegister(this->_base);
        
        if(this->_base) this->_base->Delete();
		if (this->_cells) this->_cells->Delete();
	}
	
	bool acceptsData(matvtkProtocol::DataType data) {
		return data == matvtkProtocol::DATA_MATRIX;
	}
	
	bool acceptsValue(matvtkProtocol::ValueType value) {
		return value == matvtkProtocol::VALUE_IDTYPE;
	}
	
	
protected:
    virtual void writeLength();
	virtual void writeData();

    vtkObjectBase *_base;
    
	vtkCellArray *_cells;
	
};


class ColorTransferFunctionDataSerializer : public ArraySerializer {
	
public:
	static ColorTransferFunctionDataSerializer* newSerializer(matvtkProtocol::ValueType value) {
		return new ColorTransferFunctionDataSerializer(value);
	}
		
	ColorTransferFunctionDataSerializer(matvtkProtocol::ValueType type=matvtkProtocol::VALUE_DOUBLE) : ArraySerializer(type), _colorTransferFunction(NULL) {
	}
	
	virtual ~ColorTransferFunctionDataSerializer() {
		if(_colorTransferFunction != NULL) _colorTransferFunction->Delete();
	}
	
	bool acceptsData(matvtkProtocol::DataType data) {
		return data == matvtkProtocol::DATA_COLOR_LUT;
	}
	
	bool acceptsValue(matvtkProtocol::ValueType value) {
		return value == matvtkProtocol::VALUE_DOUBLE;
	}
	
	vtkColorTransferFunction* GetColorTransferFunction() { return this->_colorTransferFunction; }
	
	virtual bool readData();
	
protected:
    virtual void writeLength();
	virtual void writeData();

	vtkColorTransferFunction *_colorTransferFunction;
};



#endif

