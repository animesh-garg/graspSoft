/*
 *  StateMachine.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 04.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "Constants.h"

#include <State.h>

#include <iostream>
#include <map>
#include <set>

typedef std::map<State*, std::map<int,State*> > StateTransitionMap;

class StateMachine {

public:
	StateMachine() : currentState(NULL), failState(NULL) {
		std::cout << "new state machine" << std::endl;
	}
	
	~StateMachine() {
		///TODO: cleanup non end states
		
		std::set<State*>::iterator it;
		for (it = allStates.begin(); it != allStates.end(); ++it) {
			State *s = (*it);
			//Debug() << "deleting state " << s->name().c_str();
			delete s;
		}
	}
	
	void addTransition(State* baseState, int cond, State* next) {
		stateTransitions[baseState][cond] = next;
		allStates.insert(baseState);
		allStates.insert(next);
	}

	
	void addTransition(State* baseState, State* secondState) {
		defaultStates[baseState] = secondState;
		allStates.insert(baseState);
		allStates.insert(secondState);
	}
	
	
	void setInitialState(State* initialState) {
		this->currentState = initialState;
		std::cout << "initial state: " << this->currentState->name() << std::endl;
		
		allStates.insert(initialState);
	}
	
	void addEndState(State *state) {
		this->endStates.insert(state);
		allStates.insert(state);
	}
	
	void setFailState(State *state) {
		this->failState = state;
		endStates.insert(state);
		allStates.insert(state);
	}
	
	int run();
	
	
private:
	
	State* nextState(State* current, int cond);
	
	State *currentState;
	State *failState;
	
	
	
	StateTransitionMap stateTransitions;
	std::map<State*, State*> defaultStates;
	std::set<State*> endStates;
	std::set<State*> allStates;
};

#endif // STATEMACHINE_H
