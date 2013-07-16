/*
 *  SpecialCommand.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 11.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "SpecialCommand.h"

#include <SceneWindow.h>
#include <iostream>
#include <qdebug.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkProperty.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkCaptionActor2D.h>
#include <vtkRenderWindow.h>
#include <vtkScalarBarActor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSet.h>
#include <vtkStringArray.h>
#include <vtkDataArray.h>
#include <vtkIdFilter.h>
#include <vtkStringArray.h>
#include <vtkLabelSizeCalculator.h>
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkLabelPlacer.h>
#include <vtkLabeledDataMapper.h>
#include <vtkPointData.h>
#include <vtkLabelHierarchy.h>

#include <vtkRenderLargeImage.h>
#include <vtkPNGWriter.h>
#include <vtkVRMLExporter.h>
#include <vtkPOVExporter.h>

#include <vtkSmartPointer.h>

#include <json.h>

using namespace matvtkProtocol;

MatvtkCommand* SpecialCommand::newCommand(matvtkProtocol::MinorCommand minor) {

	MatvtkCommand* command = NULL;
	
	switch (minor) {
		case CLOSE:
			command = new CloseCommand();
			break;
			
		case OPEN:
			command = new OpenCommand();
			break;
			
		case GRID:
			command = new SceneGridCommand();
			break;

		case TITLE:
			command = new SceneTitleCommand();
			break;
            
        case ORIENTATION:
            command = new SceneOrientationCommand();
            break;
            
        case COLOR_LEGEND:
            command = new ColorLegendCommand();
            break;
            
        case VIEW_DIRECTION:
            command = new ViewCommand();
            break;
            
        case ANNOTATION:
            command = new AnnotationCommand();
            break;

        case LABELS:
            command = new LabelsCommand();
            break;
            
        case GET:
            command = new GetCommand();
            break;
            
        case CROP:
            command = new CropCommand();
            break;
            
        case SAVE:
            command = new SaveCommand();
            break;
            
        case CONFIG:
            command = new ConfigCommand();
            break;
            
		default:
			Debug("WARNING: Command unknown");
			break;
	}
	
	return command;
}

# pragma mark SceneTitleCommand

bool SceneTitleCommand::execute(SceneWindow *win, PlotComponent *component) {
	
	connect(this, SIGNAL(newTitleComponent(PlotComponent*)), win, SLOT(SetTitleComponent(PlotComponent*)), Qt::BlockingQueuedConnection);
	
	std::string title = titleReader->GetSerializer()->dataString();

	qDebug() << "new title string:"  << title.c_str();
	
	if(title.empty()) {
		emit(newTitleComponent(NULL));
		Debug("title empty -> REMOVING title.");
		disconnect(this, SIGNAL(newTitleComponent(PlotComponent *)), win, SLOT(SetTitleComponent(PlotComponent*)));

		return true;
	}
	
	vtkSmartPointer<vtkTextActor> titleActor = vtkSmartPointer<vtkTextActor>::New();
	
	titleActor->SetTextScaleModeToViewport();
	titleActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	titleActor->GetPositionCoordinate()->SetValue(0.5,0.95);
	titleActor->SetHeight(0.05);
	
	Debug("setting new title: %s", title.c_str());
	titleActor->SetInput(title.c_str());
	
	titleActor->GetTextProperty()->SetJustificationToCentered();
	titleActor->GetTextProperty()->SetVerticalJustificationToTop();
	
	//config.Configure(titleActor->GetTextProperty());
	
	component = PlotComponent::NewComponent(titleActor);
	component->SetName("Title");
	component->AddConfigAdapter(new ConfigAdapter<vtkTextProperty>(titleActor->GetTextProperty()));
	
	qDebug() << " window: " << win;
	qDebug() << " component: " << component;
	
	emit(newTitleComponent(component));
	disconnect(this, SIGNAL(newTitleComponent(PlotComponent *)), win, SLOT(SetTitleComponent(PlotComponent*)));
	
	this->_plotComponent = component;
	this->_sceneWindow = win;
	
	return true;
}

# pragma mark SceneOrientation

bool SceneOrientationCommand::execute(SceneWindow *win, PlotComponent *comp) {
    

	std::string name;
    
    if(config.IsOn("cube"))
       name = "cube";
    else if(config.IsOn("axis"))
       name = "axis";
    
    // set to widget
    vtkSmartPointer<vtkOrientationMarkerWidget> marker = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    marker->SetOutlineColor(0.93, 0.57, 0.13);

    if(name.compare("cube") == 0) {    
		vtkSmartPointer<vtkAnnotatedCubeActor> cube = vtkSmartPointer<vtkAnnotatedCubeActor>::New();
		cube->SetXPlusFaceText("R");
		cube->SetXMinusFaceText("L");
		cube->SetYPlusFaceText("A");
		cube->SetYMinusFaceText("P");
		cube->SetZPlusFaceText("I");
		cube->SetZMinusFaceText("S");
		cube->SetXFaceTextRotation(180);
		cube->SetYFaceTextRotation(180);
		cube->SetZFaceTextRotation(-90);
		cube->SetFaceTextScale(0.65);
    
    
		// cube properties
		vtkProperty *property = cube->GetCubeProperty();
		property->SetColor(0.5, 1, 1);
    
    
		property = cube->GetTextEdgesProperty();
		property->SetLineWidth(1);
		property->SetDiffuse(0);
		property->SetAmbient(1);
		property->SetColor(0.18, 0.28, 0.23);
		
		property = cube->GetXPlusFaceProperty();
		property->SetColor(0, 0, 1);
		property->SetInterpolationToFlat();
		
		property = cube->GetXMinusFaceProperty();
		property->SetColor(0, 0, 1);
		property->SetInterpolationToFlat();
		
		property = cube->GetYPlusFaceProperty();
		property->SetColor(0, 1, 0);
		property->SetInterpolationToFlat();
		
		property = cube->GetYMinusFaceProperty();
		property->SetColor(0, 1, 0);
		property->SetInterpolationToFlat();
		
		property = cube->GetZPlusFaceProperty();
		property->SetColor(1, 0, 0);
		property->SetInterpolationToFlat();
		
		property = cube->GetZMinusFaceProperty();
		property->SetColor(1, 0, 0);
		property->SetInterpolationToFlat();
			
        Debug("annotation cube.");
        //assembly->AddPart(cube);
        marker->SetOrientationMarker(cube);
        marker->SetViewport(0.0, 0.0, 0.15, 0.15);
        
        
    }
    
    ConfigAdapterBase* textConfig = NULL;
    if(name.compare("axis") == 0) {
		// prepare axes
		vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
		axes->SetShaftTypeToCylinder();
		axes->SetXAxisLabelText("x");
		axes->SetYAxisLabelText("y");
		axes->SetZAxisLabelText("z");
		axes->SetTotalLength(1.5, 1.5, 1.5);
		
		vtkSmartPointer<vtkTextProperty> tprop = vtkSmartPointer<vtkTextProperty>::New();
		
		//config.Configure(tprop.GetPointer());
		
		axes->GetXAxisCaptionActor2D()->SetCaptionTextProperty(tprop);
		vtkSmartPointer<vtkTextProperty> tprop2 = vtkSmartPointer<vtkTextProperty>::New();
		tprop2->ShallowCopy(tprop);

			
			
		axes->GetYAxisCaptionActor2D()->SetCaptionTextProperty(tprop2);
		
		vtkSmartPointer<vtkTextProperty> tprop3 = vtkSmartPointer<vtkTextProperty>::New();
		tprop3->ShallowCopy(tprop);
		axes->GetZAxisCaptionActor2D()->SetCaptionTextProperty(tprop3);
		

		/*
		 # Add the composite marker to the widget.  The widget
		 # should be kept in non-interactive mode and the aspect
		 # ratio of the render window taken into account explicitly, 
		 # since the widget currently does not take this into 
		 # account in a multi-renderer environment.
		 #
		 */
			
		Debug("annotation axes.");
		marker->SetOrientationMarker(axes);
		marker->SetViewport(0, 0, 0.15, 0.2);
        textConfig = new ConfigAdapter<vtkTextProperty>(tprop.GetPointer());
    }

    
       
    //marker->SetInteractor(win->GetRenderWindow()->GetInteractor());

    
    comp = PlotComponent::NewComponent(marker);
    comp->SetName("Orientation marker");

    
    if(textConfig) comp->AddConfigAdapter(textConfig);

    submitComponent(comp, win);


    return true;
}

#pragma mark Color Legend

void ColorLegendCommand::setupArgumentMachine(DataPort *port) { 
    colorReader = new StateDataRead<ColorTransferFunctionDataSerializer>(port);
    titleReader = new StateDataRead<StringSerializer>(port);
    
    StateDataType *colorType = new StateDataType(port);
    StateDataType *stringType = new StateDataType(port);
    
    this->argumentMachine->setInitialState(colorType);
	this->addLastArgument(colorType, matvtkProtocol::DATA_COLOR_LUT, colorReader);
   	
    
    this->argumentMachine->addTransition(colorReader, StateCondition::Succeeded, stringType);
    this->addLastArgument(stringType, matvtkProtocol::DATA_STRING, titleReader);
    
    this->argumentMachine->addTransition(titleReader, StateCondition::Succeeded, configDataType);
    
}

bool ColorLegendCommand::execute(SceneWindow *window, PlotComponent *component) {
    
    vtkColorTransferFunction *colorLut = colorReader->GetSerializer()->GetColorTransferFunction();
    
    
    vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetLookupTable(colorLut);
    
    scalarBar->SetTitle("Values");
    
    if(titleReader->GetSerializer())
        scalarBar->SetTitle(titleReader->GetSerializer()->dataString().c_str());
    
    scalarBar->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    scalarBar->GetPositionCoordinate()->SetValue(0.01, 0.2);
    scalarBar->SetOrientationToVertical();
    scalarBar->SetWidth(0.1);
    scalarBar->SetHeight(0.8);
    
    //config.Configure(scalarBar->GetTitleTextProperty());
    
    component = PlotComponent::NewComponent(scalarBar);
    component->AddConfigAdapter(new ConfigAdapter<vtkTextProperty>(scalarBar->GetTitleTextProperty()));
    component->SetName("ColorLegend");
	submitComponent(component, window);

    

    return true;
}

#pragma mark ViewCommand

void ViewCommand::setupArgumentMachine(DataPort *port) { 
    StateDataType *azimutType = new StateDataType(port);
    StateDataType *elevationType = new StateDataType(port);
    
    azimutReader = new StateDataRead<ArraySerializer>(port);
    elevationReader = new StateDataRead<ArraySerializer>(port);
    
    this->argumentMachine->setInitialState(azimutType);
    this->argumentMachine->addTransition(azimutType, matvtkProtocol::DATA_MATRIX, azimutReader);
    
    this->argumentMachine->addTransition(azimutReader, StateCondition::Succeeded, elevationType);
    this->argumentMachine->addTransition(elevationType, matvtkProtocol::DATA_MATRIX, elevationReader);      
    
    this->argumentMachine->addTransition(elevationReader, StateCondition::Succeeded, configDataType);
    
}


bool ViewCommand::execute(SceneWindow *window, PlotComponent *component) {
    
    
    
    double azi = azimutReader->GetSerializer()->GetDataArray()->GetTuple1(0);
    double ele = elevationReader->GetSerializer()->GetDataArray()->GetTuple1(0);
    
    //Debug("azimuth: %.2f, elevation: %.2f\n", azi, ele);
    
    vtkRenderer *renderer = window->GetRenderer();
    
    vtkCamera *camera = renderer->GetActiveCamera();
    
    //pipe->GetRenderer()->ResetCamera();
    //pipe->GetRenderer()->ResetCameraClippingRange();
    
    camera->Azimuth(azi);
    camera->Elevation(ele);
    //camera->Dolly(10.0);
    //camera->OrthogonalizeViewUp();
    
    

    renderer->ResetCamera();
    renderer->ResetCameraClippingRange();
    
    renderer->Modified();

    window->Configure(&config);
        
    component = NULL;
    return true;
}

#pragma mark AnnotationCommand

void AnnotationCommand::setupArgumentMachine(DataPort *port) {
    pointReader = new StateDataRead<PointDataSerializer>(port);
    textReader= new StateDataRead<StringSerializer>(port);

    StateDataType *pointType = new StateDataType(port);
    StateDataType *textType = new StateDataType(port);
    
    this->argumentMachine->setInitialState(pointType);
    this->argumentMachine->addTransition(pointType, matvtkProtocol::DATA_MATRIX, pointReader);
    
    this->argumentMachine->addTransition(pointReader, StateCondition::Succeeded, textType);

    this->argumentMachine->addTransition(textType, matvtkProtocol::DATA_STRING, textReader);
    this->argumentMachine->addTransition(textReader, StateCondition::Succeeded, configDataType);
}

bool AnnotationCommand::execute(SceneWindow *window, PlotComponent *component) {

    

    
    vtkSmartPointer<vtkCaptionActor2D> caption = vtkSmartPointer<vtkCaptionActor2D>::New();
    
    caption->SetCaption(textReader->GetSerializer()->dataString().c_str());
    
    //config.Configure(caption->GetCaptionTextProperty());
    //config.Configure(caption->GetTextActor());
    //  config.Configure(caption->GetTextActor()->GetTextProperty());
    

    caption->SetAttachmentPoint(pointReader->GetSerializer()->GetDataArray()->GetTuple3((vtkIdType) 0));
    caption->BorderOff();
    caption->Modified();
    

    component = PlotComponent::NewComponent(caption);
    component->SetName("Annotation Caption");
    
    component->AddConfigAdapter(new ConfigAdapter<vtkTextActor>(caption->GetTextActor()));
    component->AddConfigAdapter(new ConfigAdapter<vtkTextProperty>(caption->GetCaptionTextProperty()));
    
	submitComponent(component, window);
    
    return true;
}

#pragma mark LabelsCommand

void LabelsCommand::setupArgumentMachine(DataPort *port) {
    StateDataType *pointType = new StateDataType(port);
    StateDataType *labelType = new StateDataType(port);
    
    pointReader = new StateDataRead<PointDataSerializer>(port);
    textLabelReader= new StateDataRead<StringSerializer>(port);
    numLabelReader = new StateDataRead<ArraySerializer>(port);
    
    this->argumentMachine->setInitialState(pointType);
    this->argumentMachine->addTransition(pointType, matvtkProtocol::DATA_POINTS, pointReader);
    this->argumentMachine->addTransition(pointReader, StateCondition::Succeeded, labelType);
    
    this->addLastArgument(labelType, matvtkProtocol::DATA_MATRIX, numLabelReader);
    this->addLastArgument(labelType, matvtkProtocol::DATA_STRING, textLabelReader);
    
    this->argumentMachine->addTransition(numLabelReader, StateCondition::Succeeded, configDataType);
    this->argumentMachine->addTransition(textLabelReader, StateCondition::Succeeded, configDataType);
}



#define LBL_NAME "LabelText"

bool LabelsCommand::execute(SceneWindow *win, PlotComponent *component) {

    
	vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();

    vtkPoints *points = pointReader->GetSerializer()->GetPoints();
    poly->SetPoints(points);
    
    
	vtkSmartPointer<vtkStringArray> labels = vtkSmartPointer<vtkStringArray>::New();
        
    if(numLabelReader->GetSerializer()) {
        Debug("setting numerical labels...\n");
        vtkDataArray *numLabels = numLabelReader->GetSerializer()->GetDataArray();
        
        vtkIdType numComponents = numLabels->GetNumberOfComponents();
        vtkIdType numTuples = numLabels->GetNumberOfTuples();
        
        labels->SetNumberOfValues(numTuples);
        
        // write numerical array to string
        for(vtkIdType i=0; i<numTuples; i++) {
            stringstream buf;
            buf << "(";
            for (vtkIdType c=0; c<numComponents; c++) {
                buf << numLabels->GetComponent(i, c);
                if(c<numComponents-1) buf << ", ";
            }
            buf << ")";
            labels->SetValue(i, buf.str().c_str());
        }
        
        labels->SetName("LabelText");
        poly->GetPointData()->AddArray(labels);
        poly->GetPointData()->CopyFieldOn(LBL_NAME);

    }
    else if(textLabelReader->GetSerializer()) {
        Debug("setting Strings...");

        const char *json_char = textLabelReader->GetSerializer()->dataString().c_str();
        
        json_object *root = json_tokener_parse(json_char);        
        
        array_list *json_arr = json_object_get_array(root);
        DataSize arrLen = array_list_length(json_arr);
        Debug("JSON String list values");
        

        labels->Resize(arrLen);
        
        for(DataSize i=0; i<arrLen; i++) {
            
            const char* str = json_object_get_string((json_object*) array_list_get_idx (json_arr, i));
            labels->InsertValue(i, str);
        }
        
        json_object_put(root);

        


        //textLabelReader->GetSerializer()->dataString();
        labels->SetName("LabelText");
        poly->GetPointData()->AddArray(labels);
        poly->GetPointData()->CopyFieldOn(LBL_NAME);


         
    }
    else {
        Debug("using ids as labels");
        std::cout << flush;
        vtkSmartPointer<vtkIdFilter> idFilter = vtkSmartPointer<vtkIdFilter>::New();
        idFilter->SetInput(poly);
        idFilter->FieldDataOn();
        idFilter->SetIdsArrayName("LabelText");
        idFilter->PointIdsOn();
        
        vtkDataSet *idData = idFilter->GetOutput();
        
        
        // force update, to fix cell id's +1
        idFilter->Update();
        
        // fix matlab index
        vtkDataArray *scalarsArray = idData->GetPointData()->GetArray(LBL_NAME);
        
        vtkIdType size = scalarsArray->GetSize();

        
        
        labels->SetNumberOfValues(scalarsArray->GetNumberOfTuples());
        
        for(vtkIdType i=0; i<size; i++) {
            stringstream buf;
            buf << "idx: " << scalarsArray->GetTuple1(i)+1.0;
            labels->SetValue(i, buf.str().c_str());
        }
        labels->SetName("LabelText");
        
		poly->GetPointData()->AddArray(labels);
    }
    
    Debug("normal filter pipe");
    std::cout << flush;
    
    /* works with string arrays only
     */
    vtkSmartPointer<vtkLabelSizeCalculator> labelSizeCalculator = vtkSmartPointer<vtkLabelSizeCalculator>::New();
    
    labelSizeCalculator->SetInput(poly);
    
    //config.Configure(labelSizeCalculator->GetFontProperty());
    //labelSizeCalculator->GetFontProperty()->SetFontSize( 12 );
    //labelSizeCalculator->GetFontProperty()->SetFontFamily( vtkTextProperty::GetFontFamilyFromString( "Arial" ) );
    
    labelSizeCalculator->SetInputArrayToProcess( 0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "LabelText" );
    labelSizeCalculator->SetLabelSizeArrayName( "LabelSize" );
    
    
    
    vtkSmartPointer<vtkPointSetToLabelHierarchy> pointSetToLabelHierarchy = vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();
    pointSetToLabelHierarchy->AddInputConnection(labelSizeCalculator->GetOutputPort());
    //pointSetToLabelHierarchy->AddInput(poly);
    pointSetToLabelHierarchy->SetInputArrayToProcess( 0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "Priority" );
    pointSetToLabelHierarchy->SetInputArrayToProcess( 1, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "LabelSize" );
    pointSetToLabelHierarchy->SetInputArrayToProcess( 2, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "LabelText" );
    
    /// TODO: make config parameters
    //int maxLevels = 5;
    //int targetLabels = 32;
    //pointSetToLabelHierarchy->SetMaximumDepth( maxLevels );
    //pointSetToLabelHierarchy->SetTargetLabelCount( targetLabels );
    
    
    vtkSmartPointer<vtkLabelPlacer> labelPlacer = vtkSmartPointer<vtkLabelPlacer>::New();
    labelPlacer->SetInputConnection( pointSetToLabelHierarchy->GetOutputPort() );
    labelPlacer->SetRenderer( win->GetRenderer() );
    
    bool showBounds = true;
    double labelRatio = 1.0;
    //int iteratorType = vtkLabelHierarchy::FULL_SORT;
    //int iteratorType = vtkLabelHierarchy::QUEUE;
    int iteratorType = vtkLabelHierarchy::DEPTH_FIRST;
    
    labelPlacer->SetIteratorType( iteratorType );
    labelPlacer->SetOutputTraversedBounds( showBounds );
    labelPlacer->SetMaximumLabelFraction( labelRatio );
    labelPlacer->SetGeneratePerturbedLabelSpokes( true );
    //labelPlacer->UseDepthBufferOn();
    
    
    vtkSmartPointer<vtkPolyDataMapper> polyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    polyDataMapper->SetInputConnection(labelPlacer->GetOutputPort());
    
    
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(polyDataMapper);
    
    
    
    
    component = PlotComponent::NewComponent(actor);
    component->SetName("Labels 2nd Actor");
    component->AddConfigAdapter(new ConfigAdapter<vtkTextProperty>(labelSizeCalculator->GetFontProperty()));
	emit(componentCreated(component));
    
    
    vtkSmartPointer<vtkLabeledDataMapper> labeledMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
    
    labeledMapper->SetInputConnection(labelPlacer->GetOutputPort());
    labeledMapper->SetLabelTextProperty(labelSizeCalculator->GetFontProperty());
    labeledMapper->SetFieldDataName("LabelText");
    labeledMapper->SetLabelModeToLabelFieldData();
    //labeledMapper->GetLabelTextProperty()->SetColor(0.0, 0.8, 0.2);
    
    
    vtkSmartPointer<vtkActor2D> textActor = vtkSmartPointer<vtkActor2D>::New();
    textActor->SetMapper(labeledMapper);
    
    component = PlotComponent::NewComponent(textActor);
    component->SetName("Labels");
	emit(componentCreated(component));
    

    
    /*
    labelSizeCalculator->Delete();
    pointSetToLabelHierarchy->Delete();
    labelPlacer->Delete();
    polyDataMapper->Delete();
    actor->Delete();
    
    labeledMapper->Delete();
    textActor->Delete();
    
    poly->Delete();
    
*/
    
    return true;
}

#pragma mark SaveCommand

bool SaveCommand::execute(SceneWindow *win, PlotComponent *component) {
    
    //vtkGraphicsFactory::SetUseMesaClasses(1);
    //vtkGraphicsFactory::SetOffScreenOnlyMode(1);
    //pipe->SetupWindow();
    //pipe->GetRenderer()->GetRenderWindow()->OffScreenRenderingOn();
    
    //pipe->GetRenderer()->ResetCamera();
    
    // set scene camera parameters
    //config.Configure(win->GetRenderer());
    //config.Configure(win->GetRenderer()->GetActiveCamera());
    
    
    vtkRenderLargeImage *largeImg = vtkRenderLargeImage::New();
    largeImg->SetInput(win->GetRenderer());
    
    // magnification default=1
    largeImg->SetMagnification(1);
    
    // set magnification
    if(config.IsScalar("magnification"))
        largeImg->SetMagnification(static_cast<int>(config.GetScalar("magnification")));
    
    // read image size
    
    // read filename
    std::string fileName = this->savePathReader->GetSerializer()->dataString();
    
    // TODO: fix enforce rerendering
    //win->GetRenderer()->Render();
    
    int format = 0;
    if(config.IsString("saveFormat")) format = config.GetStringValue("saveFormat");
    

    
    if(format == StringTable::find("saveFormat", "png")) {
        Debug("writing PNG.");
        
        // 'imageDimension', [16 9]
        if(config.IsPointer("imageDimension", 2)) {
            double *dims = config.GetPointer("imageDimension");
            Debug("image size: %ix%i px.", (int)dims[0], (int)dims[1]);
            win->GetRenderWindow()->SetSize((int)dims[0], (int)dims[1]);
        }
        
        vtkPNGWriter *pngWriter = vtkPNGWriter::New();
        pngWriter->SetFileName(fileName.c_str());
        pngWriter->SetInputConnection(largeImg->GetOutputPort());
        pngWriter->Write();
        
        pngWriter->Delete();
        largeImg->Delete();
    }
    else if(format == StringTable::find("saveFormat", "wrl")) {
        Debug("writing VRML.");
        vtkVRMLExporter *vrml = vtkVRMLExporter::New();
        vrml->SetInput(win->GetRenderWindow());
        vrml->SetFileName(fileName.c_str());
        vrml->Write();
        vrml->Delete();
    }
    else if(format == StringTable::find("saveFormat", "pov")) {
        Debug("writing POV format.");
        vtkPOVExporter *pov = vtkPOVExporter::New();
        
        pov->SetInput(win->GetRenderWindow());
        pov->SetFileName(fileName.c_str());
        pov->Write();
        pov->Delete();
    }
    
    Debug("done.");
    
    //win->GetRenderer()->GetRenderWindow()->OffScreenRenderingOff();
    
    //vtkGraphicsFactory::SetOffScreenOnlyMode(0);
    //vtkGraphicsFactory::SetUseMesaClasses(0);
    
    component = NULL;
    return true;
}


#pragma mark ConfigCommand
bool ConfigCommand::execute(SceneWindow *win, PlotComponent *component) {
    
    Debug("CONFIG command.");

    if(component) {
        qDebug() << "configuring component: " << component->name().c_str();
        component->Configure(&config);
        //component->ExportConfig(&config);
    }
    
    if(win) { 
        Debug("configuring window...");
        win->Configure(&config);
        
        // redraw to make settings visible
        win->refresh();
    }
    

    std::string configData = config.Serialize();
    qDebug() << "config data to return:";
    qDebug() << configData.c_str();
    
    StringSerializer *serializer = new StringSerializer(config.Serialize(), matvtkProtocol::DATA_CONFIG);
    this->_returnValues.push_back(serializer);
    
    return true;
}


