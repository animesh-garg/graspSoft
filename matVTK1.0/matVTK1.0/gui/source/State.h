/*
 *  State.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 04.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef STATE_H
#define STATE_H

#include <string>

namespace StateCondition {
	enum Enum {
		Unknown = -1,
		Succeeded,
		Failed
	};	
}


class State {


public:
	State() : _condition(StateCondition::Unknown), _name("BaseState") {
	}
	
	virtual ~State() {};
	
	virtual void process() = 0;
	
	int condition() {
		return _condition;
	}

	std::string name() {
		return _name;
	}
	
	
protected:
	int _condition;
	
	std::string _name;
	
	friend class StateMachine;
};


template<StateCondition::Enum defaultCondition=StateCondition::Succeeded>
class StateNull : public State {

public:
	StateNull() { 
		this->_name = "NullState";
	}
	
	void process() { 
		this->_condition = defaultCondition;
	}
};

typedef StateNull<StateCondition::Failed> StateFail;
typedef StateNull<StateCondition::Succeeded> StateSuccess;


#endif // STATE_H
