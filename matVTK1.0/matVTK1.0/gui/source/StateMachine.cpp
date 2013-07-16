/*
 *  StateMachine.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 04.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "StateMachine.h"

#include <set>
#include <stdexcept>

#include <State.h>

int StateMachine::run() {

	if(this->currentState == NULL) return -1;
	int processCondition = 0;
	
	while (endStates.find(currentState) == endStates.end()) {
		
		std::cout << "processing state: " << this->currentState->name() << std::endl;
		currentState->process();
		
		processCondition = currentState->condition();
		
		currentState = this->nextState(currentState, processCondition);
	}
	
	
	std::cout << " running final state: " << this->currentState->name() << std::endl;
	currentState->process();
	
	processCondition = this->currentState->condition();
	
	return processCondition;
}


State* StateMachine::nextState(State *state, int condition) {

	
	State* next = NULL;
	//try {
	//if(stateTransitions[state] == NULL) {
	//	next = startTransitions[state][condition];
	//}
	
	//std::map<int, State*> condMap =
	StateTransitionMap::iterator nextIt = stateTransitions.find(state);
	if (nextIt != stateTransitions.end()) {
		std::map<int, State*>::iterator nextStateIt = (*nextIt).second.find(condition);
			
		if (nextStateIt != stateTransitions[state].end()) {
			next = nextStateIt->second;
		}
	}

		//std::cout << "  next state: " << next->name() << std::endl;
	//}
	//catch (std::out_of_range) {
	
	if(next == NULL) {
		next = defaultStates[state];
	}	
	
	if (next == NULL) {
		next = this->failState;
		std::cout << " next state: " << this->failState << " (default fail state)" << std::endl;
	}
	
	if (next == NULL) {
		std::cerr << "no next state for " << state->name() << " after condition " << condition << std::endl;
	}
	
	return next;
}


/*
void StateMachine::addTransition(State* baseState, State* next) {
	stateTransitions[baseState][cond] = next;
}*/
