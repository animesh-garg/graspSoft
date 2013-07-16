/*
 *  DataPort.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 28.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

// for DataSize type

#ifndef DATAPORT_H
#define DATAPORT_H

#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <ostream>
//#include <typeinfo>
#include <matvtkProtocol.h>

class DataPort {

public: 
	DataPort();

	template <typename T> 
	T read() {
		T t;
		this->read<T>(&t);
		
		return t;
	}

	
	
#pragma mark writing functions


	bool write(CommandData cd);
	bool write(DataSize s);
	
	template <typename T> 
	bool write(T t) {
		DataSize typelen = sizeof(T);
		DataSize written = writeData(typelen, &t);
		
		return written == typelen;
	}
	
	
	
	void skip(DataSize len);
		
	DataSize write(DataSize len, const void* buffer);
	DataSize read(DataSize len, const void* buffer);
	
	virtual bool open() = 0;
	virtual bool close() = 0;
	virtual bool isConnected() = 0;
    
    quint32 swap (quint32 value) 	{
        quint32 b1 = (value >>  0) & 0xff;
		quint32 b2 = (value >>  8) & 0xff;
		quint32 b3 = (value >> 16) & 0xff;
		quint32 b4 = (value >> 24) & 0xff;
        
        return b1 << 24 | b2 << 16 | b3 << 8 | b4 << 0; 
    }
    
    
	quint64 swap (quint64 value) 	{
		quint64 b1 = (value >>  0) & 0xff;
		quint64 b2 = (value >>  8) & 0xff;
		quint64 b3 = (value >> 16) & 0xff;
		quint64 b4 = (value >> 24) & 0xff;
		quint64 b5 = (value >> 32) & 0xff;
		quint64 b6 = (value >> 40) & 0xff;
		quint64 b7 = (value >> 48) & 0xff;
		quint64 b8 = (value >> 56) & 0xff;
		
		return b1 << 56 | b2 << 48 | b3 << 40 | b4 << 32 | b5 << 24 | b6 << 16 | b7 <<  8 | b8 <<  0;
	}
	
    virtual void flush() { }
    
protected:
	
	template <typename T> 
	bool read(T *t);
	
	virtual DataSize readData(DataSize len, const void* data) = 0;
	virtual DataSize writeData(DataSize len, const void* data) = 0;

};

#endif // DATAPORT_H
