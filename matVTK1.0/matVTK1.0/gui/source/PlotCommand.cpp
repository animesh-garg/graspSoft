/*
 *  PlotCommand.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 05.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "PlotCommand.h"
#include "matvtkProtocol.h"


#include "DataPort.h"
#include <assert.h>


#include <VtkDataSerializer.h>
#include <StringSerializer.h>
#include <StateDataPort.h>
#include <PlotComponent.h>

#include <SceneConfiguration.h>

#include <vtkRenderer.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkSphereSource.h>
#include <vtkGlyph3D.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkTubeFilter.h>
#include <vtkLookupTable.h>
#include <vtkVolumeMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolume.h>
#include <vtkVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkVolumeTextureMapper2D.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkDelaunay3D.h>
#include <vtkUnstructuredGrid.h>
#include <vtkGeometryFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkTensorGlyph.h>
#include <vtkAppendPolyData.h>
#include <vtkCutter.h>
#include <vtkArrowSource.h>
#include <vtkContourFilter.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkSmartPointer.h>

//#include <vtkSmartVolumeMapper.h>


PlotCommand* PlotCommand::newCommand(matvtkProtocol::MinorCommand minor) {

	PlotCommand *command = NULL;
	
	switch (minor) {
		case matvtkProtocol::POINT:
			command = new PlotPointsCommand;
			break;
		
		case matvtkProtocol::LINE:
			command = new PlotLinesCommand;
			break;
		
		case matvtkProtocol::VOLUME:
			command = new PlotVolumeCommand;
			break;
			
		case matvtkProtocol::MESH:
			command = new PlotMeshCommand;
			break;

		case matvtkProtocol::TENSOR:
			command = new PlotTensorsCommand;
			break;
			
		case matvtkProtocol::CUTPLANE:
			command = new PlotCutplanesCommand;
			break;
			
		case matvtkProtocol::QUIVER:
			command = new PlotQuiverCommand;
			break;
			
		case matvtkProtocol::ISOSURFACE:
			command = new PlotIsosurfaceCommand;
			break;
			
		default:
			Debug("unknown plot command specified");
			command = NULL;
			break;
			//assert(false);
	}
		
	return command;
}


#pragma mark POINTS

void PlotPointsCommand::setupArgumentMachine(DataPort *port) {
	
	// vtkplotpoints([HANDLE], points, [labels | labels, colors], [config, ...])
	
	StateDataType *pointsDataType = new StateDataType(port);
	StateDataType *labelsDataType = new StateDataType(port);
	StateDataType *colorDataType = new StateDataType(port);
	
	
	pointsReader = new StateDataRead<PointDataSerializer>(port);
	labelsReader = new StateDataRead<ArraySerializer>(port);
	colorLutReader = new StateDataRead<ColorTransferFunctionDataSerializer>(port);
	
	
	
	this->argumentMachine->setInitialState(pointsDataType);
	
	this->addLastArgument(pointsDataType, matvtkProtocol::DATA_POINTS, pointsReader);
	this->argumentMachine->addTransition(pointsReader, StateCondition::Succeeded, labelsDataType);

	this->addLastArgument(labelsDataType, matvtkProtocol::DATA_MATRIX, labelsReader);
	this->argumentMachine->addTransition(labelsReader, StateCondition::Succeeded, colorDataType);

	
	this->addLastArgument(colorDataType, matvtkProtocol::DATA_COLOR_LUT, colorLutReader);
	this->argumentMachine->addTransition(colorLutReader, StateCondition::Succeeded, configDataType);
    
}



bool PlotPointsCommand::execute(SceneWindow *window, PlotComponent *component) {
	
	
	vtkPoints *points = this->pointsReader->GetSerializer()->GetPoints();
	std::cout << "vtk points: " << points << std::endl;
	//std::cout << "point config: " << this->configReader->GetSerializer()->dataString() << std::endl;
	
	vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
	poly->SetPoints(points);
	
	vtkDataArray *labels = NULL;
	if(this->labelsReader->GetSerializer() != NULL) {
		labels = this->labelsReader->GetSerializer()->GetDataArray();
		
		// labels matrix has a bad shape (not 1xN)
		if(labels->GetNumberOfComponents() != 1 || labels->GetNumberOfTuples() != points->GetNumberOfPoints()) {
			
            
			return false;
		}
		
		std::cout << "point labels: " << labels << std::endl;
		labels->SetName("PointLabels");
		// set labels as scalar data to the points
		poly->GetPointData()->SetScalars(labels);
	}
	
    // we want to see spheres at the points
	vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
	//config.Configure(sphere.GetPointer());
	
	
	
	
	// config glyphs: map scalars to colors
	vtkSmartPointer<vtkGlyph3D> glyphs = vtkSmartPointer<vtkGlyph3D>::New();
	glyphs->SetInput(0, poly);
	glyphs->SetInputConnection(1, sphere->GetOutputPort());


	
	glyphs->ScalingOff();
	glyphs->OrientOn();
	glyphs->ClampingOff();
	glyphs->SetVectorModeToUseVector();
	glyphs->SetIndexModeToOff();
	
	
	
	// set up mapper that shows shperes at points
	vtkSmartPointer<vtkPolyDataMapper> glyphMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	glyphMapper->SetInputConnection(glyphs->GetOutputPort());
	
	
	// build and set lut if available
	if(labels) {
        Debug("setting labels");
		glyphMapper->SetColorModeToMapScalars();
		glyphMapper->SetScalarRange(labels->GetRange());
	}
	
	if(colorLutReader->GetSerializer()) {
        Debug("setting colors");
		vtkColorTransferFunction *colorLut = colorLutReader->GetSerializer()->GetColorTransferFunction();
		glyphMapper->SetLookupTable(colorLut);
	}
	// labels, but no color: auto mapping
	
    
	
	//glyphs->Delete();
	
	vtkSmartPointer<vtkActor> glyphActor = vtkSmartPointer<vtkActor>::New();
	glyphActor->SetMapper(glyphMapper);

	

	
	
	// general actor config
	//config.Configure(glyphActor.GetPointer());
	
	window = NULL;
	// create plot component and add to scene
	component = PlotComponent::NewComponent(glyphActor);
	component->SetName("Points");
    component->AddConfigAdapter(new ConfigAdapter<vtkSphereSource>(sphere.GetPointer(), "points"));
    //component->AddConfigAdapter(new ConfigAdapter<vtkActor>(glyphActor.GetPointer()));
    submitComponent(component, window);


    
	return true;
}


#pragma mark LINES

void PlotLinesCommand::setupArgumentMachine(DataPort *port) {

	// vtkplotlines([HANDLE], points, [startIdx | startIdx, idxList], [pointLabels | pointLabels, colorLUT], [config, ...])
	
	StateDataType *pointDataType = new StateDataType(port);
	StateDataType *labelsType = new StateDataType(port);

	StateDataType *startIdxType = new StateDataType(port);
	StateDataType *listIdxType = new StateDataType(port);
	StateDataType *colorLutType = new StateDataType(port);
	
	
	pointsReader = new StateDataRead<PointDataSerializer>(port);
	labelsReader = new StateDataRead<ArraySerializer>(port);
	colorLutReader = new StateDataRead<ColorTransferFunctionDataSerializer>(port); 

	
	startIdxReader = new StateDataRead<CellDataSerializer>(port);
	listIdxReader = new StateDataRead<CellDataSerializer>(port);
	
	
		
	this->argumentMachine->setInitialState(pointDataType);
	
	// read points
	//this->addLastArgument(pointDataType, matvtkProtocol::DATA_POINTS, pointsReader);
	this->argumentMachine->addTransition(pointDataType, matvtkProtocol::DATA_POINTS, pointsReader);
	this->argumentMachine->addTransition(pointsReader, StateCondition::Succeeded, startIdxType);
	
	// optional read: start indexes
	this->argumentMachine->addTransition(startIdxType, matvtkProtocol::DATA_MATRIX, labelsReader);
	this->addLastArgument(startIdxType, matvtkProtocol::DATA_CELL, startIdxReader);
	
	this->argumentMachine->addTransition(startIdxReader, StateCondition::Succeeded, listIdxType);
	
    
    this->addLastArgument(listIdxType, matvtkProtocol::DATA_CELL, listIdxReader);
    
	// optional read: line indexes
	this->argumentMachine->addTransition(listIdxType, matvtkProtocol::DATA_MATRIX, labelsReader);
	
    
	this->argumentMachine->addTransition(listIdxReader, StateCondition::Succeeded, labelsType);

    this->addLastArgument(labelsType, matvtkProtocol::DATA_MATRIX, labelsReader);
                                         
	this->argumentMachine->addTransition(labelsReader, StateCondition::Succeeded, colorLutType);

	this->addLastArgument(colorLutType, matvtkProtocol::DATA_COLOR_LUT, colorLutReader);

	this->argumentMachine->addTransition(colorLutReader, StateCondition::Succeeded, configDataType);

	
}

bool PlotLinesCommand::execute(SceneWindow *window, PlotComponent *component) {
 
	    
	// read in the points
    vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
	
	vtkPoints *points = pointsReader->GetSerializer()->GetPoints();
	
	poly->SetPoints(points);
	
	
	
	// set indices, if given (start indexes)
	vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();

	if(startIdxReader->GetSerializer()) {

		vtkSmartPointer<vtkCellArray> startIdx = startIdxReader->GetSerializer()->GetCells();

		
		vtkIdType* buff;
		vtkIdType len = 1;

		vtkIdType lineBegin=0, lineEnd=0;
	
		vtkSmartPointer<vtkCellArray> lineIdx = NULL;
		if (listIdxReader->GetSerializer()) {
			lineIdx = listIdxReader->GetSerializer()->GetCells();
		}
		else {
			lineIdx = vtkSmartPointer<vtkCellArray>::New();
			lineIdx->Initialize();
			for (vtkIdType i=0; i<(points->GetNumberOfPoints()); i++) {
				lineIdx->InsertNextCell(1);
				lineIdx->InsertCellPoint(i);
			}
		}

		startIdx->InitTraversal();
		lineIdx->InitTraversal();
		
		
		startIdx->GetNextCell(len, buff);
		lineBegin = buff[0];
		
		// iterate over line begin indexes, "fill" line points until next begin index
		while(startIdx->GetNextCell(len, buff)) {
            //std::cout << "cell len: " << len << std::endl;
			lineEnd = buff[0];
						
			//std::cout << "line: " << lineBegin << " - " << lineEnd-1 << std::endl;
			lines->InsertNextCell(lineEnd-lineBegin);
            
			for (vtkIdType i=lineBegin; i<lineEnd; i++) {
				lineIdx->GetNextCell(len, buff);
				lines->InsertCellPoint(buff[0]);
				//std::cout << " " << buff[0];
			}
			//std::cout << std::endl;
			
			lineBegin = lineEnd;		
		}

	}
	// no line parts set, assuming 1 long line
	else {
		lines->InsertNextCell(points->GetNumberOfPoints());
		for (vtkIdType i=0; i<points->GetNumberOfPoints(); i++) {
			lines->InsertCellPoint(i);
		}
	}

	
	
	// if index list is available, use it

	
	poly->SetLines(lines);

	
	
	vtkSmartPointer<vtkPolyDataMapper> polyMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	
	// set labels and color LUT if they are available
	if(labelsReader->GetSerializer()) {
		vtkSmartPointer<vtkDataArray> scalars = labelsReader->GetSerializer()->GetDataArray();

		poly->GetPointData()->SetScalars(scalars);
		
		polyMapper->ScalarVisibilityOn();
		
		vtkSmartPointer<vtkScalarsToColors> colorLut;
		
		if(colorLutReader->GetSerializer()) {
			colorLut = colorLutReader->GetSerializer()->GetColorTransferFunction();
		}
		// create default LUT when only labels are set
		else {
			vtkSmartPointer<vtkLookupTable> table = vtkSmartPointer<vtkLookupTable>::New();
			table->SetHueRange(0.666667, 0.0);
			
			colorLut = table;
			polyMapper->SetScalarRange(scalars->GetRange());
		}
		
		polyMapper->SetLookupTable(colorLut);
	}
	 
	
	// added to renderer: real lines, or with tube filter
	vtkSmartPointer<vtkPolyData> sceneData = poly;
	
	if(config.IsOn("lineTubes")) {
		vtkSmartPointer<vtkTubeFilter> tubes = vtkSmartPointer<vtkTubeFilter>::New();
		tubes->SetInput(poly);

		
		if(config.IsScalar("tubeRadius"))
			tubes->SetRadius(config.GetScalar("tubeRadius"));
		
		if(config.IsScalar("tubeSides")) tubes->SetNumberOfSides((int) config.GetScalar("tubeSides"));
		else tubes->SetNumberOfSides(6);
		
		sceneData = tubes->GetOutput();
	}
	
	
	
	polyMapper->SetInput(sceneData);

	
		
	vtkSmartPointer<vtkActor> polyDataActor = vtkSmartPointer<vtkActor>::New();
	
	polyDataActor->SetMapper(polyMapper);

	
	
	// configure the scene
	//config.Configure(polyDataActor.GetPointer());
	

	component = PlotComponent::NewComponent(polyDataActor);
	component->SetName("Lines");
	submitComponent(component, window);
    
	return true;
	
}

#pragma mark VOLUME

void PlotVolumeCommand::setupArgumentMachine(DataPort *port) {
	// vtkplotvolume([HANDLE], volume, [LUTname | colors, opacity], [config, ...])
	
	StateDataType *volumeType = new StateDataType(port);
	StateDataType *colorsType = new StateDataType(port);
	StateDataType *opacityType = new StateDataType(port);
	
	volumeReader = new StateDataRead<ImageDataSerializer>(port);
	colorReader = new StateDataRead<ColorTransferFunctionDataSerializer>(port);
	opacityReader = new StateDataRead<ArraySerializer>(port);
	
	
	this->argumentMachine->setInitialState(volumeType);
	this->addLastArgument(volumeType, matvtkProtocol::DATA_VOLUME, volumeReader);
	this->argumentMachine->addTransition(volumeReader, StateCondition::Succeeded, colorsType);
	
	
	this->addLastArgument(colorsType, matvtkProtocol::DATA_COLOR_LUT, colorReader);
	this->argumentMachine->addTransition(colorsType, matvtkProtocol::DATA_STOP, nullState);
	
	this->argumentMachine->addTransition(colorReader, StateCondition::Succeeded, opacityType);
	this->argumentMachine->addTransition(opacityType, matvtkProtocol::DATA_MATRIX, opacityReader);
	this->argumentMachine->addTransition(opacityReader, StateCondition::Succeeded, configDataType);
	this->argumentMachine->addTransition(configDataType, matvtkProtocol::DATA_STOP, nullState);
	
	
	
}

bool PlotVolumeCommand::execute(SceneWindow *window, PlotComponent *component) {


	// scene basics
	
	// convert matlab volume to vtk
	vtkSmartPointer<vtkImageData> volumeData(volumeReader->GetSerializer()->GetImageData());
	double *range = volumeData->GetScalarRange();
	
	if(config.IsPointer("volumeSpacing"))
		volumeData->SetSpacing(config.GetPointer("volumeSpacing"));
	
	
	// needed to set color and opacity
	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	
	// set color transfer function
	vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction;
	vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
	// use color LUT input data
	if(colorReader->GetSerializer()) {
		colorTransferFunction = colorReader->GetSerializer()->GetColorTransferFunction();
		
		// if we have colors, we also must have opacity
		vtkDataArray* opacityValues = opacityReader->GetSerializer()->GetDataArray();
		double *valueOpacity;
		for(vtkIdType i=0; i<opacityValues->GetNumberOfTuples(); i++) {
			valueOpacity = opacityValues->GetTuple2(i);
		
			opacityTransferFunction->AddPoint(valueOpacity[0], valueOpacity[1]);
			//mexPrintf("opacity: %.2f->%1.2f\n", opacity[i], opacity[dimLen[0]+i]);
		}

	}
	// create default color LUT
	else {
		Debug("default color LUT for range: %.2f - %.2f", range[0], range[1]);
		colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
		//colorTransferFunction->HSVWrapOff();
		//colorTransferFunction->SetColorSpaceToHSV();
		
        colorTransferFunction->SetColorSpaceToRGB();
        
		//AddHSVSegment (double x1, double h1, double s1, double v1, double x2, double h2, double s2, double v2)
		//colorTransferFunction->AddHSVPoint(range[0], 0.6667, 1, 1);
		//colorTransferFunction->AddHSVPoint(range[1], 0, 1, 1);
		

		// debug print of color table
        double dataMin = range[0]; double dataMax = range[1];
		double dataDiff = (dataMax-dataMin);

        // auto jet colors
        if(dataDiff > 0.0) {
            // matlab like jet color table
            colorTransferFunction->AddRGBPoint(dataMin + (dataDiff*0.000), 0.0, 0.0, 0.5);
            colorTransferFunction->AddRGBPoint(dataMin + (dataDiff*0.125), 0.0, 0.0, 1.0);
            colorTransferFunction->AddRGBPoint(dataMin + (dataDiff*0.375), 0.0, 1.0, 1.0);
            colorTransferFunction->AddRGBPoint(dataMin + (dataDiff*0.625), 1.0, 1.0, 0.0);
            colorTransferFunction->AddRGBPoint(dataMin + (dataDiff*0.875), 1.0, 0.0, 0.0);
            colorTransferFunction->AddRGBPoint(dataMin + (dataDiff*1.000), 0.5, 0.0, 0.0);

            // coming in later VTK versions
            // colorTransferFunction->SetNanColor(0.0, 0.0, 0.0);
#ifdef DEBUG
            double step = (dataDiff/10.0);
            for(double i=range[0]; i<=range[1]; i+=step) {
                double *color = colorTransferFunction->GetColor(i);
                Debug("auto color: %.2f -> %.2f %.2f %.2f", i, color[0], color[1], color[2]);
            }
#endif
        }
        // only 1 data value in volume, set to blue
        else {
            colorTransferFunction->AddRGBPoint(dataMin, 0.0, 0.0, 1.0);
        }
        

		// create default opacity lut, constant opacities over whole data range
		double opacity = 0.05;
		if(config.IsScalar("opacity")) opacity = config.GetScalar("opacity");
		opacityTransferFunction->AddPoint(range[0], opacity);
		opacityTransferFunction->AddPoint(range[1], opacity);

	}

	
    // set colors
    volumeProperty->SetColor(colorTransferFunction);
	
	// set opacity
	volumeProperty->SetScalarOpacity(opacityTransferFunction);
	
	
	// fast but limited renderer: samples volume down
	// raycasting may be nicer but is limited on data types
	//vtkSmartVolumeMapper *smartVolumeMapper = vtkSmartVolumeMapper::New();
    vtkSmartPointer<vtkVolumeMapper> volumeMapper;

    ConfigAdapterBase *rayCastConfig = NULL;
	if(config.IsOn("rayCast")) {
		Debug("raycasting.");
		
		vtkGPUVolumeRayCastMapper *gpuRaycaster = vtkGPUVolumeRayCastMapper::New();

		
		if (gpuRaycaster->IsRenderSupported(window->GetRenderWindow(), volumeProperty)) {
			Debug("GPU OpenGL RayCasting enabled!");
			volumeMapper.TakeReference(gpuRaycaster);
            rayCastConfig = new ConfigAdapter<vtkGPUVolumeRayCastMapper>(gpuRaycaster, "ray cast");
            
            volumeMapper.TakeReference(gpuRaycaster);
		}
		// no fast GPU raycasting available.
		else {
       		gpuRaycaster->Delete();	
			Debug("SLOW software ray casting.");
			//vtkVolumeRayCastCompositeFunction *compositeFunction = vtkVolumeRayCastCompositeFunction::New();
			vtkFixedPointVolumeRayCastMapper *raycastVolumeMapper = vtkFixedPointVolumeRayCastMapper::New();
			raycastVolumeMapper->SetBlendModeToComposite();
			//raycastVolumeMapper->SetVolumeRayCastFunction(compositeFunction);
			//compositeFunction->Delete();
			//raycastVolumeMapper->AutoAdjustSampleDistancesOn();
            //config.Configure(raycastVolumeMapper);
            rayCastConfig = new ConfigAdapter<vtkFixedPointVolumeRayCastMapper>(raycastVolumeMapper, "ray cast");
            Debug() << "sample distance" << raycastVolumeMapper->GetSampleDistance();
            
			volumeMapper.TakeReference(raycastVolumeMapper);
			//raycastVolumeMapper->Delete();
		}
		
	
	}
	else {

        // max texture size for 3d textures: 128*256*256
        // otherwise it is resized bevor rendering(SLOW!)
        Debug("3D texture mapping.");
        
        int dims[3];
        volumeData->GetDimensions(dims);
        if(dims[0] > 128 || dims[1] > 256 || dims[2] > 256) {
            Debug("WARNING: volume too large for efficient 3d texture mapping (128x256x256).");
        }
		
        volumeMapper.TakeReference(vtkVolumeTextureMapper3D::New());
        /*
        else if(volumeData->Get) {
            // use 2d texture mapping for bigger volumes
            Debug("2D texture mapping.");
            volumeMapper = vtkVolumeTextureMapper2D::New();
        }
        */
		//volumeMapper->Delete();
	}
	
	volumeMapper->SetInput(volumeData);
	
	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
	
	volume->SetMapper(volumeMapper);
	

	//config.Configure(volumeProperty);
	volume->SetProperty(volumeProperty);
	//volumeProperty->Delete();
	
	//config.Configure(volume.GetPointer());
	//volumeMapper->Modified();
	//volume->Modified();
	
	component = PlotComponent::NewComponent(volume);
	component->SetName("Volume");
	
    if(rayCastConfig) {
      component->AddConfigAdapter(rayCastConfig);   
    }
    
	submitComponent(component, window);

	return true;
}


#pragma mark MESH

void PlotMeshCommand::setupArgumentMachine(DataPort *port) {
	StateDataType *pointsType = new StateDataType(port);
	StateDataType *facesType = new StateDataType(port);
	StateDataType *labelsType = new StateDataType(port);
	StateDataType *colorsType = new StateDataType(port);
	
	pointReader = new StateDataRead<PointDataSerializer>(port);
	facesReader = new StateDataRead<CellDataSerializer>(port);
	labelReader = new StateDataRead<ArraySerializer>(port);
	colorReader = new StateDataRead<ColorTransferFunctionDataSerializer>(port);

	
	this->argumentMachine->setInitialState(pointsType);
	this->argumentMachine->addTransition(pointsType, matvtkProtocol::DATA_POINTS, pointReader);
	this->argumentMachine->addTransition(pointReader, StateCondition::Succeeded, facesType);
	
	this->addLastArgument(facesType, matvtkProtocol::DATA_CELL, facesReader);
	this->argumentMachine->addTransition(facesReader, StateCondition::Succeeded, labelsType);
	
	this->addLastArgument(labelsType, matvtkProtocol::DATA_MATRIX, labelReader);
	this->argumentMachine->addTransition(labelReader, StateCondition::Succeeded, colorsType);
	
	this->addLastArgument(colorsType, matvtkProtocol::DATA_COLOR_LUT, colorReader);
	this->argumentMachine->addTransition(colorReader, StateCondition::Succeeded, configDataType);
}

bool PlotMeshCommand::execute(SceneWindow *window, PlotComponent *component) {
	//usage: vtkplotmesh(VTKHANDLE, vertices, [faces, vertexLabels, colors, config])
    
	vtkSmartPointer<vtkDataSetMapper> surfMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	
	// read in the vertices/faces
	vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
	
	// read point data
	vtkSmartPointer<vtkPoints> points = pointReader->GetSerializer()->GetPoints();
	poly->SetPoints(points);
	
	// read triangulation data
	if(facesReader->GetSerializer()) {
		// no need for smartpointer here, just getting instance to use in setter
        vtkCellArray *faces = facesReader->GetSerializer()->GetCells();
		poly->SetPolys(faces);
	}
	
	vtkPolyData *renderData = poly;
	
	// to be used if no triangulation is given
	vtkSmartPointer<vtkDelaunay3D> triangulator = vtkSmartPointer<vtkDelaunay3D>::New();
	
	//vtkUnstructuredGrid* usg = vtkUnstructuredGrid::New();
	vtkSmartPointer<vtkGeometryFilter> geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New();

	if(facesReader->GetSerializer() == NULL) {
		/*
		 # Delaunay3D is used to triangulate the points. The Tolerance is the
		 # distance that nearly coincident points are merged
		 # together. (Delaunay does better if points are well spaced.) The
		 # alpha value is the radius of circumcircles, circumspheres. Any mesh
		 # entity whose circumcircle is smaller than this value is output.
		 */
		triangulator->SetInput(poly);
		//triangulator->SetTolerance(3); // of omitting (nearly) coincident points
		//triangulator->SetAlpha(2);
		//triangulator->BoundingTriangulationOff();
		
		if(config.IsScalar("triangulationTolerance"))
			triangulator->SetTolerance(config.GetScalar("triangulationTolerance"));
		
		if(config.IsScalar("triangulationAlpha"))
			triangulator->SetTolerance(config.GetScalar("triangulationAlpha"));
		
		
		//usg = triangulator->GetOutput();
		
		
		//polyData->SetPoints(usg->GetPoints());
		//polyData->SetPolys(usg->GetCells());
		//poly->SetPoints(usg->GetPoints());
		//poly->SetPolys(usg->GetCells());
		// Get surface triangles with geometry filter
		
		geometryFilter->SetInput(triangulator->GetOutput());
		
		
		
		//geometryFilter->MergingOn();
		// connect triangulator, filter and set output 
		//cellFilter->SetInput(triangulator->GetOutput());
		geometryFilter->Update();
		renderData = geometryFilter->GetOutput();
		
		
		
		// set the triangulation cells as output data
		vtkSmartPointer<vtkCellArray> faceCells = renderData->GetPolys();

        // serializer will be freed when PlotCommand instance is destroyed
        CellDataSerializer *serializer = new CellDataSerializer(faceCells);
		this->_returnValues.push_back(serializer);
        
    }
	
	
	
	// read and set labels, if available
	if(labelReader->GetSerializer() != NULL) {
		vtkDataArray* labels = labelReader->GetSerializer()->GetDataArray();
		
		// set labels as scalar data to the points
		renderData->GetPointData()->SetScalars(labels);
		
		// map scalars to colors
		surfMapper->ScalarVisibilityOn();
		surfMapper->SetScalarRange(labels->GetRange());
		
	}
	
	
	// build and set lut
	if(colorReader->GetSerializer() != NULL) {
		vtkColorTransferFunction *colorLut = colorReader->GetSerializer()->GetColorTransferFunction();
		surfMapper->SetLookupTable(colorLut);
	}
	else {
		vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
		lut->SetHueRange(0.6667, 0);
		surfMapper->SetLookupTable(lut);
	}
	
	surfMapper->SetInput(renderData);
	
	vtkSmartPointer<vtkActor> surfActor = vtkSmartPointer<vtkActor>::New();
	surfActor->SetMapper(surfMapper);
	
	// configure actor et all
	//config.Configure(surfActor.GetPointer());
	
	component = PlotComponent::NewComponent(surfActor);
	component->SetName("Mesh");
    component->AddConfigAdapter(new ConfigAdapter<vtkActor>(surfActor.GetPointer()));
    
	submitComponent(component, window);

  
	return true;
}

#pragma mark TENSORS

void PlotTensorsCommand::setupArgumentMachine(DataPort *port) {

	pointReader = new StateDataRead<PointDataSerializer>(port);
	tensorReader = new StateDataRead<ArraySerializer>(port);
	labelReader = new StateDataRead<ArraySerializer>(port);
	colorReader = new StateDataRead<ColorTransferFunctionDataSerializer>(port);

	StateDataType *pointType = new StateDataType(port);
	StateDataType *tensorType = new StateDataType(port);
	StateDataType *labelType = new StateDataType(port);
	StateDataType *colorType = new StateDataType(port);
	
	
	
	this->argumentMachine->setInitialState(pointType);
	this->argumentMachine->addTransition(pointType, matvtkProtocol::DATA_POINTS, pointReader);
	this->argumentMachine->addTransition(pointReader, StateCondition::Succeeded, tensorType);
	
	this->argumentMachine->addTransition(tensorType, matvtkProtocol::DATA_MATRIX, tensorReader);
	this->argumentMachine->addTransition(tensorReader, StateCondition::Succeeded, labelType);
	
	this->addLastArgument(labelType, matvtkProtocol::DATA_MATRIX, labelReader);
	this->argumentMachine->addTransition(labelReader, StateCondition::Succeeded, colorType);
	
	this->addLastArgument(colorType, matvtkProtocol::DATA_COLOR_LUT, colorReader);
	this->argumentMachine->addTransition(colorReader, StateCondition::Succeeded, configDataType);
}

bool PlotTensorsCommand::execute(SceneWindow *window, PlotComponent *component) {
	
	
    //usage: vtkplottensors([VTKHANDLE, ] points, tensors [, labels, colors, config])

	
	vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
	
    // only used for this setter
	vtkSmartPointer<vtkPoints> points = pointReader->GetSerializer()->GetPoints();
	poly->SetPoints(points);
	
    // only used for setter
	vtkDataArray *tensors = tensorReader->GetSerializer()->GetDataArray();
	poly->GetPointData()->SetTensors(tensors);
	
	
	
	
	vtkSmartPointer<vtkSphereSource> source = vtkSmartPointer<vtkSphereSource>::New();
		
	
	// config glyphs: map scalars to colors
	vtkSmartPointer<vtkTensorGlyph> glyphs = vtkSmartPointer<vtkTensorGlyph>::New();
	glyphs->SetInput(0, poly);
	glyphs->SetInputConnection(1, source->GetOutputPort());
	
	glyphs->ColorGlyphsOn();
	//
	


	
	if(config.IsScalar("scaleFactor")) {
		glyphs->SetScaleFactor(config.GetScalar("scaleFactor"));
	}
	if(config.IsScalar("maxScaleFactor")) {
		glyphs->ClampScalingOn();
		glyphs->SetMaxScaleFactor(config.GetScalar("maxScaleFactor"));
	}
	
	vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
	normals->SetInputConnection(glyphs->GetOutputPort());
	
	
	vtkSmartPointer<vtkPolyDataMapper> glyphMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	glyphMapper->SetInputConnection(normals->GetOutputPort());


	    
	
	if(labelReader->GetSerializer()) {
		// only used here in setter
        vtkDataArray *labels = labelReader->GetSerializer()->GetDataArray();
		poly->GetPointData()->SetScalars(labels);

		
		
		if(colorReader->GetSerializer()) {
			vtkColorTransferFunction *colorLut = colorReader->GetSerializer()->GetColorTransferFunction();
			glyphMapper->SetLookupTable(colorLut);
		}
		else {
			vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
			lut->SetHueRange (0.6667, 0.0);
			glyphMapper->SetLookupTable(lut);
			lut->Delete();
			
			glyphs->SetColorModeToScalars();
			glyphMapper->SetScalarRange(labels->GetRange());
		}
	}
	else {
		Debug("no labels: -> using eigenvalues for color mapping");
		glyphs->ExtractEigenvaluesOn();
		glyphs->SetColorModeToEigenvalues();
		glyphs->Update();

		glyphMapper->SetScalarRange(glyphs->GetOutput()->GetPointData()->GetScalars()->GetRange());
		vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
		lut->SetHueRange (0.6667, 0.0);
		glyphMapper->SetLookupTable(lut);
	}
	
	
	
	vtkSmartPointer<vtkActor> glyphActor = vtkSmartPointer<vtkActor>::New();
	glyphActor->SetMapper(glyphMapper);
	

	// config.Configure(source.GetPointer());
	// config.Configure(glyphActor.GetPointer());

	component = PlotComponent::NewComponent(glyphActor);
	component->SetName("Tensors");
    component->AddConfigAdapter(new ConfigAdapter<vtkSphereSource>(source.GetPointer()));
    component->AddConfigAdapter(new ConfigAdapter<vtkActor>(glyphActor.GetPointer()));
	
    submitComponent(component, window);

	
	
	return true;
}


#pragma mark CUTPLANES

void PlotCutplanesCommand::setupArgumentMachine(DataPort *port) {

	//usage: vtkplotcutplanes([HANDLE,] volume, points, vectors, [LUTname | colors,] [,config])
	
	volumeReader = new StateDataRead<ImageDataSerializer>(port);
	pointReader = new StateDataRead<PointDataSerializer>(port);
	vectorReader = new StateDataRead<ArraySerializer>(port);
	colorReader = new StateDataRead<ColorTransferFunctionDataSerializer>(port);
	
	StateDataType *volumeType = new StateDataType(port);
	StateDataType *pointType = new StateDataType(port);
	StateDataType *vectorType = new StateDataType(port);
	StateDataType *colorType = new StateDataType(port);
	
	this->argumentMachine->setInitialState(volumeType);

	this->argumentMachine->addTransition(volumeType, matvtkProtocol::DATA_VOLUME, volumeReader);
	this->argumentMachine->addTransition(volumeReader, StateCondition::Succeeded, pointType);

	
	this->argumentMachine->addTransition(pointType, matvtkProtocol::DATA_POINTS, pointReader);
	this->argumentMachine->addTransition(pointReader, StateCondition::Succeeded, vectorType);
	
	this->argumentMachine->addTransition(vectorType, matvtkProtocol::DATA_MATRIX, vectorReader);
	this->argumentMachine->addTransition(vectorReader, StateCondition::Succeeded, colorType);
	
	this->addLastArgument(colorType, matvtkProtocol::DATA_COLOR_LUT, colorReader);
	this->addLastArgument(colorReader, StateCondition::Succeeded, configDataType);


	
}

bool PlotCutplanesCommand::execute(SceneWindow *window, PlotComponent *component) {
	
	
	// convert matlab volume to vtk
    vtkImageData *volumeData = volumeReader->GetSerializer()->GetImageData();
    vtkPoints *origins = pointReader->GetSerializer()->GetPoints();
    vtkDataArray *normals = vectorReader->GetSerializer()->GetDataArray();
	
    if(config.IsPointer("volumeSpacing"))
        volumeData->SetSpacing(config.GetPointer("volumeSpacing"));
	
    vtkSmartPointer<vtkAppendPolyData> appender = vtkSmartPointer<vtkAppendPolyData>::New();
	
    double normal[] = { 0, 0, 0};
    for(vtkIdType i=0;i<origins->GetNumberOfPoints(); i++) {
		vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
		plane->SetOrigin(origins->GetPoint(i));
		
		normals->GetTuple(i, normal);
		plane->SetNormal(normal);
		
		vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
		cutter->SetCutFunction(plane);
		cutter->SetInput(volumeData);
		
		appender->AddInput(cutter->GetOutput());
    }
	
    vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    planeMapper->SetInput(appender->GetOutput());
    planeMapper->SetScalarRange(volumeData->GetScalarRange());
	
	if(colorReader->GetSerializer()) {
		vtkColorTransferFunction *colorLut = colorReader->GetSerializer()->GetColorTransferFunction();
		planeMapper->SetLookupTable(colorLut);
	}
	else {
		vtkLookupTable *lut = vtkLookupTable::New();
		lut->SetHueRange (0.6667, 0.0);
		planeMapper->SetLookupTable(lut);
		planeMapper->SetScalarRange(volumeData->GetScalarRange());
		lut->Delete();
	}
	
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(planeMapper);
	
    //config.Configure(actor.GetPointer());
	
    component = PlotComponent::NewComponent(actor);
	component->SetName("CutPlanes");
    component->AddConfigAdapter(new ConfigAdapter<vtkActor>(actor.GetPointer()));
    
	submitComponent(component, window);
	
	return true;
}


#pragma mark QUIVER

void PlotQuiverCommand::setupArgumentMachine(DataPort *port) {
	
	StateDataType *pointState = new StateDataType(port);
	StateDataType *vectorState = new StateDataType(port);
	
	pointReader = new StateDataRead<PointDataSerializer>(port);
	vectorReader = new StateDataRead<ArraySerializer>(port);
	
	this->argumentMachine->setInitialState(pointState);
	this->argumentMachine->addTransition(pointState, matvtkProtocol::DATA_POINTS, pointReader);
	this->argumentMachine->addTransition(pointReader, StateCondition::Succeeded, vectorState);
	this->argumentMachine->addTransition(vectorState, matvtkProtocol::DATA_MATRIX, vectorReader);
	this->argumentMachine->addTransition(vectorReader, StateCondition::Succeeded, configDataType);
	
	
}

bool PlotQuiverCommand::execute(SceneWindow *window, PlotComponent *component) {
	//mexPrintf("state: mxPoints=%p, *mxLabels=%p, *mxColors=%p, *mxConfig=%p size:%.2f\n", mxPoints, mxLabels, mxColors, mxConfig, dblSize);
	
    // vtkquiver([HANDLE], points, vectors, [config, ...])
       
    
	// read in the points
	vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
	
	vtkPoints *points = pointReader->GetSerializer()->GetPoints();
	poly->SetPoints(points);
	
	
	vtkDataArray *vectors = vectorReader->GetSerializer()->GetDataArray();
	poly->GetPointData()->SetVectors(vectors);
	
	/*
	 vtkAssignAttribute* assign = vtkAssignAttribute::New();
	 assign->SetInput(poly);
	 assign->Assign(vtkDataSetAttributes::VECTORS, vtkDataSetAttributes::SCALARS,
	 vtkAssignAttribute::POINT_DATA);
	 */
	
	//vtkVectorNorm *norm = vtkVectorNorm::New();
	//norm->SetInput(poly);
	//norm->NormalizeOff();
	
    // we want to see spheres at the points
	vtkSmartPointer<vtkArrowSource> arrow = vtkSmartPointer<vtkArrowSource>::New();
	
	
	
	// config glyphs: map scalars to colors
	vtkSmartPointer<vtkGlyph3D> glyphs = vtkSmartPointer<vtkGlyph3D>::New();
	
	
	glyphs->SetInput(0, poly);
	glyphs->SetInputConnection(1, arrow->GetOutputPort());

    glyphs->ScalingOn();
    glyphs->SetScaleModeToScaleByVector();
    
    
	if(config.IsScalar("scaling") && config.IsOff("scaling")) {
        glyphs->ScalingOff();
	}
    
    if(config.IsScalar("scaleFactor")) {
        glyphs->SetScaleFactor(config.GetScalar("scaleFactor"));
    }
	
	glyphs->OrientOn();
	glyphs->ClampingOff();
	glyphs->SetVectorModeToUseVector();
	glyphs->SetIndexModeToOff();
	
	
	
	// This creates a blue to red lut.
	vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New(); 
	lut->SetHueRange (0.6667, 0.0);
	
	
	// set up mapper that shows shperes at points
	vtkSmartPointer<vtkPolyDataMapper> glyphMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	glyphMapper->SetInputConnection(glyphs->GetOutputPort());
	
	// don't use lut if we have an explicit color
	if(!config.IsPointer("color")) {
		glyphs->SetColorModeToColorByVector();
		glyphMapper->ScalarVisibilityOn();
		glyphMapper->SetScalarRange(vectors->GetRange(-1)); // get range of norms
		glyphMapper->SetLookupTable(lut);
	}
	
	
	//lut->Delete();
	
	//mexPrintf("scalars range: %f, %f\n", poly->GetPointData()->GetScalars()->GetRange()[0], poly->GetPointData()->GetScalars()->GetRange()[1]);
	//mexPrintf("vectors range: %f, %f\n", glyphs->GetRange()[0], glyphs->GetRange()[1]);
	//mexPrintf("mapper range: %f, %f\n", glyphMapper->GetScalarRange()[0], glyphMapper->GetScalarRange()[1]);

	
	// copy config
	
	
	vtkSmartPointer<vtkActor> glyphActor = vtkSmartPointer<vtkActor>::New();
	//config.Configure(glyphActor.GetPointer());
	
	glyphActor->SetMapper(glyphMapper);
	
	component = PlotComponent::NewComponent(glyphActor);
	component->SetName("Quiver");
    component->AddConfigAdapter(new ConfigAdapter<vtkActor>(glyphActor.GetPointer()));
    
	submitComponent(component, window);
	
	return true;
}


#pragma mark ISOSURFACE

void PlotIsosurfaceCommand::setupArgumentMachine(DataPort *port) {
	StateDataType *volumeType = new StateDataType(port);
	StateDataType *isoType = new StateDataType(port);

	
	volumeReader = new StateDataRead<ImageDataSerializer>(port);
	isoReader = new StateDataRead<ArraySerializer>(port);
	colorReader = new StateDataRead<ColorTransferFunctionDataSerializer>(port);
	
	this->argumentMachine->setInitialState(volumeType);
	this->argumentMachine->addTransition(volumeType, matvtkProtocol::DATA_VOLUME, volumeReader);
	
	this->argumentMachine->addTransition(volumeReader, StateCondition::Succeeded, isoType);
	
	//this->argumentMachine->addTransition(isoType, matvtkProtocol::DATA_MATRIX, isoReader);
	this->addLastArgument(isoType, matvtkProtocol::DATA_MATRIX, isoReader);
	this->argumentMachine->addTransition(isoReader, StateCondition::Succeeded, configDataType);
	
	this->addLastArgument(isoType, matvtkProtocol::DATA_COLOR_LUT, colorReader);
	this->argumentMachine->addTransition(colorReader, StateCondition::Succeeded, configDataType);
	
	
}


bool PlotIsosurfaceCommand::execute(SceneWindow *window, PlotComponent *component) {
    // vtkisosurface([HANDLE], volume, [isoValues | colorLUT], [config, ...])
    
	// convert matlab volume to vtk
	vtkImageData *volumeData = volumeReader->GetSerializer()->GetImageData();
	
	if(config.IsPointer("volumeSpacing"))
		volumeData->SetSpacing(config.GetPointer("volumeSpacing"));
	
	
    vtkSmartPointer<vtkContourFilter> iso = vtkSmartPointer<vtkContourFilter>::New();
	
    // this increses rendering speed, we don't need normals or gradients
    ///TODO: check if this is really ok!
    iso->ComputeNormalsOff();
    iso->ComputeGradientsOff();
    iso->ComputeScalarsOff();
    
    iso->SetInput(volumeData);
	
	vtkScalarsToColors *lut = NULL;
	vtkSmartPointer<vtkLookupTable> alut = vtkSmartPointer<vtkLookupTable>::New();
	alut->SetHueRange (0.6667, 0.0);
	
	lut = alut;
	
	if(isoReader->GetSerializer()) {
		vtkSmartPointer<vtkDataArray> isoValues = isoReader->GetSerializer()->GetDataArray();
		
		// read data array and set iso surface values
		for(vtkIdType i=0; i<isoValues->GetNumberOfTuples(); i++) {
			iso->SetValue(i, isoValues->GetTuple1(i));
		}
		Debug("explicit iso values...\n");
	}
	else if(colorReader->GetSerializer()) {
		vtkColorTransferFunction *colorLUT = colorReader->GetSerializer()->GetColorTransferFunction();
		
		vtkIdType lines = colorLUT->GetSize();
		double values[6];
		for(vtkIdType i=0; i<lines; i++) {
			colorLUT->GetNodeValue(i, values);
			iso->SetValue(i, values[0]);
		}
		

		lut = colorLUT;
//		mxIsoValues = mxColors; // for now: to be compatible with cropping
		
		Debug("iso values from color LUT.\n");
	}
	else {
		iso->GenerateValues(10, volumeData->GetScalarRange());
		Debug("auto generated iso values.\n");
	}
	
	
    
    //iso->ComputeGradientsOn();
    //iso->ComputeScalarsOff();
    
    //vtkVectorNorm *gradient = vtkVectorNorm::New();
    //gradient->SetInputConnection(iso->GetOutputPort());
	
    vtkSmartPointer<vtkPolyDataMapper> isoMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    isoMapper->SetInputConnection(iso->GetOutputPort());
	
    if(config.IsPointer("color")) {
		isoMapper->SetLookupTable(NULL);
		isoMapper->ScalarVisibilityOff();
		Debug("have explicit config color...\n");
		
    }
    else {
		iso->ComputeScalarsOn();
		isoMapper->ScalarVisibilityOn();
		isoMapper->SetScalarRange(volumeData->GetScalarRange());
		isoMapper->SetLookupTable(lut);
		
    }
	
	
    //lut->Delete();
	
    vtkSmartPointer<vtkActor> isoActor = vtkSmartPointer<vtkActor>::New();
    isoActor->SetMapper(isoMapper);
	
	
    // run config for whole scene
    //config.Configure(isoActor.GetPointer());
    //isoMapper->Update();
	
    component = PlotComponent::NewComponent(isoActor);
	component->SetName("IsoSurface");
    component->AddConfigAdapter(new ConfigAdapter<vtkActor>(isoActor.GetPointer()));
    
	submitComponent(component, window);


	return true;
}

