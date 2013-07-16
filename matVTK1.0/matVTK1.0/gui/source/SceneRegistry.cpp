/*
 *  SceneRegistry.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 29.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "SceneRegistry.h"

#include "Constants.h"

#include <stdlib.h>
#include <iostream>
#include <SceneWindow.h>
#include <RegistryEntry.h>
#include <PlotComponent.h>

#include <qapplication.h>

SceneRegistry* SceneRegistry::_instance = new SceneRegistry();

SceneRegistry::SceneRegistry() {
	Debug("SceneRegistry started.");
}

SceneRegistry::~SceneRegistry() {
	Debug("SceneRegistry shutdown.");
	
	RegistryMapType::iterator it;
	for(it = registryMap.begin(); it!=registryMap.end(); ++it) {

		RegistryEntry *regEntry = (*it).second;
		
		SceneWindow *win = regEntry->reference();
		delete win;
		
	}
	
}


SceneRegistry* SceneRegistry::instance() {
    return SceneRegistry::_instance;
}


RegistryHandle SceneRegistry::generateHandle() {

        RegistryHandle h = rand();
	
	while (handlePool.find(h) != handlePool.end()) {
		std::cerr << "handle " << h << " already taken, generating new handle." << std::endl;
                h = rand();

	}
	
	return h;
}

// register window or component
RegistryHandle SceneRegistry::registerWindow(SceneWindow *window) {
	
	RegistryHandle winHandle = this->generateHandle();
	handlePool.insert(winHandle);
	
	
	this->setDefaultWindowHandle(winHandle);
	registryMap[winHandle] = new RegistryEntry(window, winHandle);
	qDebug() << " register window" << window << "with hanbdle" << winHandle;
	
	return winHandle;
}



RegistryHandle SceneRegistry::registerComponent(SceneWindow *window, PlotComponent *component) {
	
	RegistryHandle winHandle = this->handleFor(window);
	RegistryHandle componentHandle = this->generateHandle();
	
	handlePool.insert(componentHandle);
	
	registryMap[winHandle]->AddComponent(componentHandle, component);
	
	return componentHandle;
	
}

// remove window/components from registry
void SceneRegistry::unregisterWindow(SceneWindow *window) {
	
	RegistryHandle handle = this->handleFor(window);

	// if we close the current default window, we have to find a new one
	if(this->defaultWindowHandle() == handle) {
		RegistryMapType::reverse_iterator it = registryMap.rbegin();

        while(registryMap.rend() != it && it->second->reference() == window) {
			it++;
		}
		
		qDebug() << "old default handle: " << handle << " new: " << it->first;

		if (it->first != handle) {
       		this->setDefaultWindowHandle(it->first);
        }
        else {
			Warning("unable to find new default window");
			Warning("new default handle postcondition failed!");
            this->setDefaultWindowHandle(0);
		}

	}
	
	qDebug() << "deleting window handle " << handle;
    delete registryMap[handle];
	registryMap.erase(handle);
	handlePool.erase(handle);
	
}

void SceneRegistry::unregisterComponent(SceneWindow *window, PlotComponent *component) {
    window = NULL;
	RegistryHandle compHandle = this->handleFor(component);
	
	handlePool.erase(compHandle);
}

// get handle for concrete instance
RegistryHandle SceneRegistry::handleFor(SceneWindow *window) {
	
	
	RegistryMapType::iterator it;
	for(it = registryMap.begin(); it!=registryMap.end(); ++it) {
		RegistryEntry *entry = (*it).second;
								
		if (entry->reference() == window) {
			return (*it).first;
		}
	}
	return 0;
}

RegistryHandle SceneRegistry::handleFor(PlotComponent *component) {
    
    RegistryHandle handle = 0;
    RegistryMapType::iterator it;
	for(it = registryMap.begin(); it!=registryMap.end(); ++it) {
		RegistryEntry *entry = (*it).second;
            
        handle = entry->GetHandle(component);
        if(handle) return handle;
	}
    
	return handle;
}


// lookup concrete instance for a handle
SceneWindow* SceneRegistry::windowForHandle(RegistryHandle handle) {

	if(handle == 0) handle = this->defaultWindowHandle();
	
    if(handle == 0) return NULL;
    
	RegistryEntry *entry = registryMap[handle];
	
	// we have a sane entry.
	if (entry) {
		return entry->reference();
	}
	
	// nothing was found for this handle
	return NULL;
}

PlotComponent* SceneRegistry::componentForHandle(RegistryHandle handle) {
	
	RegistryMapType::iterator mapIt;
	
    // iterate over window entries, try to find component with handle
	for(mapIt = registryMap.begin(); mapIt != registryMap.end(); ++mapIt) {
        PlotComponent *pc = mapIt->second->GetComponent(handle);
        if(pc) return pc;
	}
	
	return NULL;
}

void SceneRegistry::newWindow() {
	qDebug() << "Registry:: NEW WINDOW";
	
	//SceneWindow *win = 
    SceneWindow::newWindow();
}

void SceneRegistry::newWindow(RegistryHandle* handle) {
	qDebug() << "Registry:: NEW WINDOW";
	
	SceneWindow *win = SceneWindow::newWindow();
	*handle = this->handleFor(win);
}


void SceneRegistry::closeAllWindows() {

	SceneWindow *win = this->windowForHandle(this->defaultWindowHandle());
	
	while(win != NULL) {
		qDebug() << "closing window:" << win;
		win->closeWindow();
		win = this->windowForHandle(this->defaultWindowHandle());
	}
}

void SceneRegistry::aboutToQuit() {
    
}

void SceneRegistry::shutdownApplication() {
	this->closeAllWindows();
    QApplication::flush();
	QApplication::instance()->quit();
}
