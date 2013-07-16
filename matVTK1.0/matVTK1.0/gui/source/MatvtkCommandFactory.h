/*
 *  MatvtkCommandFactory.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 11.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef MATVTKCOMMANDFACTORY_H
#define MATVTKCOMMANDFACTORY_H

#include <matvtkProtocol.h>
#include <MatvtkCommand.h>
#include <PlotCommand.h>
#include <SpecialCommand.h>
#include <InteractiveCommand.h>

using namespace matvtkProtocol;

class MatvtkCommandFactory  {
	
public:
	static MatvtkCommand* newCommand(MajorCommand major, MinorCommand minor) {
		
		MatvtkCommand *command = NULL;
		
		switch (major) {
				
			case matvtkProtocol::PLOT:
				std::clog << "major: PLOT." << std::endl;
				command = PlotCommand::newCommand(minor);
				break;
				
			case matvtkProtocol::GET:
				std::clog << "major: GET." << std::endl;
				command = NULL;
				break;
				
			case matvtkProtocol::SET:
				std::clog << "major: SET." << std::endl;
				command = NULL;
				break;
				
			case matvtkProtocol::SPECIAL:
				std::clog << "major: SPECIAL." << std::endl;
				command = SpecialCommand::newCommand(minor);
				break;
				
            case matvtkProtocol::INTERACTIVE:
                std::clog << "major: INTERACTIVE" << std::endl;
                command = InteractiveCommand::newCommand(minor);
                break;
                
			case matvtkProtocol::MAJOR_ERROR:
			default:
				std::cerr << "MAJOR command error." << std::endl;
				break;
		}
		
		return command;
	}
};


#endif

