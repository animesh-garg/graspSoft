/*
 *  SceneRegistry.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 29.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */



#ifndef SCENEREGISTRY_H
#define SCENEREGISTRY_H

#include <qdebug.h>
#include <qobject.h>

#include <map>
#include <set>
#include "matvtkProtocol.h"

//#include <SceneWindow.h>
//#include <PlotComponent.h>
#include <RegistryEntry.h>


class SceneWindow;
class PlotComponent;

//typedef std::map<RegistryHandle, RegistryEntry<SceneWindow*,PlotComponent*>* > RegistryMapType;
typedef std::map<RegistryHandle, RegistryEntry* > RegistryMapType;

class SceneRegistry : public QObject {

	Q_OBJECT;
	
public:
	
	// register window or component
	RegistryHandle registerWindow(SceneWindow *window);
	RegistryHandle registerComponent(SceneWindow *window, PlotComponent *component);
	
	// remove window/components from registry
	void unregisterWindow(SceneWindow *window);
	void unregisterComponent(SceneWindow *window, PlotComponent *component);
	
	// get handle for concrete instance
	RegistryHandle handleFor(SceneWindow *window);
	RegistryHandle handleFor(PlotComponent *component);

	
	// lookup concrete instance for a handle
	SceneWindow* windowForHandle(RegistryHandle handle);
	PlotComponent* componentForHandle(RegistryHandle handle);
	
	
	
	// singleton pattern for the SceneRegistry (see also: private constructor)
	static SceneRegistry* instance();
	
	
	// generate unique handle
	RegistryHandle generateHandle();
	
	
	// accessors for default window
	void setDefaultWindowHandle(RegistryHandle h) {
		this->_defaultWindowHandle = h;
	}
	
	RegistryHandle defaultWindowHandle() { return this->_defaultWindowHandle; };
	
	// accessors for default component
	void setDefaultComponentHandle(RegistryHandle h) {
		this->_defaultComponentHandle = h;
	}
	
	RegistryHandle defaultComponentHandle() { return this->_defaultComponentHandle; };


	
	virtual ~SceneRegistry();
	
	public slots:
	void newWindow();
	void newWindow(RegistryHandle* handle);
  	void closeAllWindows();
	void shutdownApplication();
    void aboutToQuit();

private:
	SceneRegistry();
	
	static SceneRegistry* _instance;
	
	// containers for registry data
	RegistryMapType registryMap;
	std::set<RegistryHandle> handlePool;
	
	// default handle values
	RegistryHandle _defaultWindowHandle;
	RegistryHandle _defaultComponentHandle;
};



#endif // SCENEREGISTRY_H
