/*
 *  StateDataPort.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 04.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef STATEDATAPORT_H
#define STATEDATAPORT_H

#include <iostream>
#include <typeinfo>
#include <State.h>
#include <DataPort.h>

#include <matvtkProtocol.h>
#include <MatvtkCommand.h>
#include <SceneRegistry.h>
#include <SceneWindow.h>

#include <ArraySerializer.h>
#include <VtkDataSerializer.h>

// DataPort related states base class
class StateDataPort : public State {
	
public:
	StateDataPort(DataPort *port) : _port(port) {
			this->_name = "DataPortState";
	}
	
	
protected:
	DataPort *_port;

};


// connection init, version check
class StateConnectionSetup : public StateDataPort {
	
public:
	StateConnectionSetup(DataPort *port) : StateDataPort(port) {
		this->_name = "SetupState";
	}
	
	void process() {
		// send own version
		bool serverStart = _port->write(matvtkProtocol::START);
		bool serverVersion = _port->write(matvtkProtocol::VERSION);
		
		std::clog << "server start: " << serverStart << std::endl;
		std::clog << "server version: " << serverVersion << std::endl;

		
		// read client version
		CommandData clientStart = _port->read<CommandData>();
		CommandData clientVersion = _port->read<CommandData>();
		std::clog << "client start: " << clientStart << std::endl;
		std::clog << "client version: " << clientVersion << std::endl;
		
		// set completion state
		if(clientStart == matvtkProtocol::START && 
		   clientVersion==matvtkProtocol::VERSION && 
		   serverStart && 
		   serverVersion)
			this->_condition = StateCondition::Succeeded;
		else 
			this->_condition = StateCondition::Failed;
	}
};


class StateConnectionClose : public StateDataPort {
public:
	StateConnectionClose(DataPort *port) : StateDataPort(port) {
		this->_name = "CloseState";
	}
	
	void process() {
		bool quitSent = _port->write( (CommandData) matvtkProtocol::QUIT);
		//bool closed = _port->close();
		
		if (quitSent /*&& closed*/) {
			this->_condition = StateCondition::Succeeded;
			std::clog << " clean connection shutdown." << std::endl;
		}
		else {
			this->_condition = StateCondition::Failed;
			std::clog << " unclean connection teardown." << std::endl;
		}

	}
};


class StateCommand : public StateDataPort {

public:
	StateCommand(DataPort *port) : StateDataPort(port) {
		this->_name = "CommandState";
	}
	
	void process();	
private:
	
};


class StateDataType : public StateDataPort {

public:
	StateDataType(DataPort *port) : StateDataPort(port) {
		this->_name = "TypeState";
	}
	
	//void SetAcceptedType(matvtkProtocol::DataType type) { this->_acceptedType = type; }
	//matvtkProtocol::DataType GetAcceptedType() { return this->_acceptedType; }
	
	void process() {
		std::clog << "State: data type..." << std::endl;
		
		std::cout << "CommandData size: " << sizeof(CommandData) << std::endl;
		
		matvtkProtocol::DataType dataType = (matvtkProtocol::DataType) _port->read<CommandData>();
		std::cout << "data: " << dataType << std::endl;
		
		this->_condition = dataType;
	}
	
	/*
private:
	matvtkProtocol::DataType _acceptedType;
	 */
};

template<typename T>
class StateDataRead : public StateDataPort {

public:
	StateDataRead(DataPort *port) : StateDataPort(port), _serializer(NULL) {
		this->_name = "DataState";
	}
	
	virtual ~StateDataRead() {
		if(_serializer) delete _serializer;
	}
	
	T* GetSerializer() { return this->_serializer; }
	//void SetSerializer(DataSerializer *serializer) { this->_serializer = serializer; }
	
	void process() {
		std::clog << "State: Data Reader..." << std::endl;
		
		//matvtkProtocol::DataType dataType = (matvtkProtocol::DataType) _port->read<CommandData>();
		//std::cout << "data: " << dataType << std::endl;
		
		matvtkProtocol::ValueType valueType = (matvtkProtocol::ValueType) _port->read<CommandData>();
		std::cout << "values: " << vtkImageScalarTypeNameMacro(valueType) << std::endl;
		
		
		_serializer = T::newSerializer(valueType);
		
		this->_condition = StateCondition::Failed;
		
		
		if (_serializer != NULL) {
			std::clog << "reader for " << (typeid(T).name()) << " is runnning" << std::endl;
			bool consumeSuccess = _serializer->consumeData(this->_port);

			if(consumeSuccess) this->_condition = StateCondition::Succeeded;
		}
		
		

	}

private:
	T *_serializer;
};



#endif //STATE_DATA_PORT_H
