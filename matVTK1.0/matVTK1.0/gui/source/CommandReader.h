/*
 *  CommandParser.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 22.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

/** should read the command sequence (2 shorts) and configure data parser appropriately.
	then read data, create vtk structures and add them to the scene (depending on command)
 
 */

#ifndef COMMANDREADER_H
#define COMMANDREADER_H

#include <matvtkProtocol.h>

class CommandPort;

class CommandReader {
	
public:
	CommandReader(CommandPort *port) : port(port) {
	
	}
	
	void readMajorCommand(); // reads operation type: get, set, plot, ...
	void readMinorCommand(); // reads operation subtype: i.e. plot-points, get-triangulation...
	void readHandle(); // reads "address"/handle of window, plot component
	void readArguments(); // reads matrix data
	void readConfig(); // reads config parameters
	

protected:
	//MajorCommand major;
	//MinorCommand minor;
	
	RegistryHandle windowHandle;
	RegistryHandle componentHandle;
	
	CommandPort *port;
};

#endif // COMMANDREADER_H