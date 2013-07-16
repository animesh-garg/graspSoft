/*
 *  StateConnectionSetup.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 04.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

/*
#ifndef STATECONNECTIONSETUP_H
#define STATECONNECTIONSETUP_H

#include <State.h>
#include <StateDataPort.h>

class StateConnectionSetup : public StateDataPort {

public:
	
	void process() {
		// read client version
		bool clientStart = false;
		bool clientVersion = false;
		
		
		// send own version
		bool serverStart = false;
		bool serverVersion = false;
		
		// set completion state
		if(clientStart && clientVersion && serverStart && serverVersion)
			this->_condition = StateCondition::Succeeded;
		else 
			this->_condition = StateCondition::Failed;
	}
};

#endif

*/