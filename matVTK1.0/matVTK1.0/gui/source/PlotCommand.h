/*
 *  PlotCommand.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 05.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef PLOTCOMMAND_H
#define PLOTCOMMAND_H

#include "MatvtkCommand.h"

#include "matvtkProtocol.h"
#include "StateDataPort.h"


// template <typename T>
//  class StateDataRead;

class ArraySerializer;
class PointDataSerializer;
class ImageDataSerializer;
class StringSerializer;

class PlotComponent;

class PlotCommand : public MatvtkCommand {

public:
	static PlotCommand* newCommand(matvtkProtocol::MinorCommand minor);
	
protected:
	
};



class PlotPointsCommand : public PlotCommand {

public:
	PlotPointsCommand() : pointsReader(NULL), labelsReader(NULL), colorLutReader(NULL) {
		Debug("PlotPointsCommand");
	}
	
	virtual ~PlotPointsCommand() {
		/*
		if(pointsReader) delete pointsReader;
		if(labelsReader) delete labelsReader;
		if(colorLutReader) delete colorLutReader;
		 */
	}
	virtual bool execute(SceneWindow *window, PlotComponent *component);

protected:
	virtual void setupArgumentMachine(DataPort *port);
	
	StateDataRead<PointDataSerializer> *pointsReader;
	StateDataRead<ArraySerializer> *labelsReader;
	StateDataRead<ColorTransferFunctionDataSerializer> *colorLutReader;
};


#pragma mark LINES

class PlotLinesCommand : public PlotCommand {
	
public:
	PlotLinesCommand() : pointsReader(NULL), startIdxReader(NULL), listIdxReader(NULL), labelsReader(NULL), colorLutReader(NULL) {
	}
	
	virtual ~PlotLinesCommand() {
	}
	
	virtual bool execute(SceneWindow *window, PlotComponent *component);
	
protected:
	virtual void setupArgumentMachine(DataPort *port);
	
	StateDataRead<PointDataSerializer> *pointsReader;
	StateDataRead<CellDataSerializer> *startIdxReader;
	StateDataRead<CellDataSerializer> *listIdxReader;
	StateDataRead<ArraySerializer> *labelsReader;
	StateDataRead<ColorTransferFunctionDataSerializer> *colorLutReader;
	
};

#pragma mark VOLUME

class PlotVolumeCommand : public PlotCommand {

public:
	PlotVolumeCommand() : volumeReader(NULL), colorReader(NULL), opacityReader(NULL) {
	}
	
	virtual ~PlotVolumeCommand() {
		/*
		if(volumeReader) delete volumeReader;
		if(colorReader) delete colorReader;
		if(opacityReader) delete opacityReader;
		 */
	}

	virtual bool execute(SceneWindow *window, PlotComponent *component);
	
protected:
	virtual void setupArgumentMachine(DataPort *port);

	StateDataRead<ImageDataSerializer> *volumeReader;
	StateDataRead<ColorTransferFunctionDataSerializer> *colorReader;
	StateDataRead<ArraySerializer> *opacityReader;
	
};


#pragma mark MESH
class PlotMeshCommand : public PlotCommand {
	
public:
	PlotMeshCommand() : pointReader(NULL), facesReader(NULL), labelReader(NULL), colorReader(NULL) {
	}
	
	virtual ~PlotMeshCommand() {
		/*
		if(pointReader) delete pointReader;
		if(facesReader) delete facesReader;
		if(labelReader) delete labelReader;
		if(colorReader) delete colorReader;
		 */
	}
	
	virtual bool execute(SceneWindow *window, PlotComponent *component);
	
protected:
	virtual void setupArgumentMachine(DataPort *port);
	
	StateDataRead<PointDataSerializer> *pointReader;
	StateDataRead<CellDataSerializer> *facesReader;

	StateDataRead<ArraySerializer> *labelReader;
	StateDataRead<ColorTransferFunctionDataSerializer> *colorReader;

	
};


#pragma mark TENSORS


class PlotTensorsCommand : public PlotCommand {
	
public:
	PlotTensorsCommand() : pointReader(NULL), tensorReader(NULL), labelReader(NULL), colorReader(NULL) {
	}
	
	virtual ~PlotTensorsCommand() {
		/*
		if(pointReader) delete pointReader;
		if(tensorReader) delete tensorReader;
		if(labelReader) delete labelReader;
		if(colorReader) delete colorReader;
		 */
	}
	
	virtual bool execute(SceneWindow *window, PlotComponent *component);
	
protected:
	virtual void setupArgumentMachine(DataPort *port);
	
	StateDataRead<PointDataSerializer> *pointReader;
	StateDataRead<ArraySerializer> *tensorReader;
	
	StateDataRead<ArraySerializer> *labelReader;
	StateDataRead<ColorTransferFunctionDataSerializer> *colorReader;
	
	
};


#pragma mark CUTPLANES

//usage: vtkplotcutplanes([HANDLE,] volume, points, vectors, [LUTname | colors,] [,config])
class PlotCutplanesCommand : public PlotCommand {
	
public:
	PlotCutplanesCommand() : volumeReader(NULL), pointReader(NULL), vectorReader(NULL), colorReader(NULL) {
		Debug("PlotCutplanesCommand");
	}
	
	virtual ~PlotCutplanesCommand() {
		/*
		if(volumeReader) delete volumeReader;
		if(pointReader) delete pointReader;
		if(vectorReader) delete vectorReader;
		if(colorReader) delete colorReader;
		 */
	}
	
	virtual bool execute(SceneWindow *window, PlotComponent *component);
	
protected:
	virtual void setupArgumentMachine(DataPort *port);

	StateDataRead<ImageDataSerializer> *volumeReader;
	StateDataRead<PointDataSerializer> *pointReader;
	StateDataRead<ArraySerializer> *vectorReader;
	StateDataRead<ColorTransferFunctionDataSerializer> *colorReader;
	
	
};

#pragma mark QUIVER

// vtkquiver([HANDLE], points, vectors, [config, ...])
class PlotQuiverCommand : public PlotCommand {
	
public:
	PlotQuiverCommand() : pointReader(NULL), vectorReader(NULL) {
		Debug("PlotQuiverCommand");
	}
	
	virtual ~PlotQuiverCommand() {
		/*
		if(pointReader) delete pointReader;
		if(vectorReader) delete vectorReader;
		 */
	}
	
	virtual bool execute(SceneWindow *window, PlotComponent *component);
	
protected:
	virtual void setupArgumentMachine(DataPort *port);
	
	StateDataRead<PointDataSerializer> *pointReader;
	StateDataRead<ArraySerializer> *vectorReader;
};


#pragma mark ISOSURFACE
//vtkisosurface([HANDLE], volume, [isoValues | colorLUT], [config, ...])

class PlotIsosurfaceCommand : public PlotCommand {
	
public:
	PlotIsosurfaceCommand() : volumeReader(NULL), isoReader(NULL), colorReader(NULL) {
		Debug("PlotIsosurfaceCommand");
	}
	
	virtual ~PlotIsosurfaceCommand() {
		/*
		if(volumeReader) delete volumeReader;
		if(isoReader) delete isoReader;
		if(colorReader) delete colorReader;
		 */
	}
	
	virtual bool execute(SceneWindow *window, PlotComponent *component);
	
protected:
	virtual void setupArgumentMachine(DataPort *port);
	
	StateDataRead<ImageDataSerializer> *volumeReader;
	StateDataRead<ArraySerializer> *isoReader;
	StateDataRead<ColorTransferFunctionDataSerializer> *colorReader;
};

#endif

