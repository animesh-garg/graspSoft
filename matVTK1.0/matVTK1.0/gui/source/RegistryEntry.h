/*
 *  RegistryEntry.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 29.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef REGISTRYENTRY_H
#define REGISTRYENTRY_H

#include <map>
#include <iostream>


#include <matvtkProtocol.h>


class PlotComponent;
class SceneWindow;

class RegistryEntry {

typedef std::map<RegistryHandle, PlotComponent*> EntryMap;
        
public:	
	RegistryEntry() {
		std::clog << "default RegistryEntry contructor. why?" << std::endl;
	}
	
	RegistryEntry(SceneWindow *r, RegistryHandle h=0) : _handle(h), _reference(r) {
		std::clog << "new entry: handle " << _handle << " for " << r << std::endl;
	}
	
	SceneWindow* reference() {
		return _reference;
	}
	
	void AddComponent(RegistryHandle h, PlotComponent *c) {
		componentMap[h] = c;
	}
	
	PlotComponent* GetComponent(RegistryHandle h) {
		return componentMap[h];
	}
    
    RegistryHandle GetHandle(PlotComponent *c) {
        EntryMap::iterator it;

        RegistryHandle handle = 0;
        
        for(it = componentMap.begin(); it != componentMap.end(); ++it) {
            if((*it).second == c) 
                return (*it).first;
        }
        
        return handle;
    }
	
private:
	RegistryHandle _handle;
	SceneWindow* _reference;
	
	// container for child components
	EntryMap componentMap;
	
	
	friend class SceneRegistry;
};

#endif // REGISTRYENTRY_H


