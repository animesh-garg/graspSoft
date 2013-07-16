/*
 *  matvtkProtocol.h
 *  qtdemo
 *
 *  Created by Erich Birngruber on 28.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MATVTK_PROTOCOL
#define MATVTK_PROTOCOL

#include <qglobal.h>
#include <vtkType.h>
#include <cstring>

typedef quint16 RegistryHandle;
typedef	quint16 CommandData;
typedef quint64 DataSize;

//#define VERSION ((short) 1)

namespace matvtkProtocol {

	extern const qint16 DEFAULT_NETWORK_PORT;
	
	extern const CommandData VERSION;
	extern const CommandData START;
	extern const CommandData QUIT;
	
	//extern const CommandData DATA;
	//extern const CommandData CONFIG;
	
// MAJOR commands
typedef enum {
	MAJOR_ERROR = 0,
	PLOT = 1,
    GET_MAJOR,
	SET,
	SPECIAL,
    INTERACTIVE
} MajorCommand;


// MINOR commands
typedef enum {
	MINOR_ERROR = 0,
	POINT = 1,
	LINE,
	VOLUME,
	MESH,
	TENSOR,
	CUTPLANE,
	QUIVER,
	ISOSURFACE,
	CLOSE,
	OPEN,
	REMOVE,
	GRID,
	TITLE,
    ORIENTATION,
    COLOR_LEGEND,
    VIEW_DIRECTION,
    ANNOTATION,
    LABELS,
    GET,
    SELECTPOINTS,
    WAIT,
    CROP,
    SAVE,
    CONFIG
} MinorCommand;

typedef enum {
	DATA_ERROR = -1,
	DATA_STOP = 0,
	DATA_MATRIX = 1, // == array
	DATA_VOLUME, // imageData
	DATA_POINTS, // vtkPoint[s|Data]
	DATA_CELL,
	DATA_COLOR_LUT,
	DATA_STRING, // string
	DATA_CONFIG // string (for now), json key/value
} DataType;

/*
#define VTK_VOID            0
 25 #define VTK_BIT             1
#define VTK_CHAR            2
 27 #define VTK_SIGNED_CHAR    15
 28 #define VTK_UNSIGNED_CHAR   3
 29 #define VTK_SHORT           4
 30 #define VTK_UNSIGNED_SHORT  5
 31 #define VTK_INT             6
 32 #define VTK_UNSIGNED_INT    7
 33 #define VTK_LONG            8
 34 #define VTK_UNSIGNED_LONG   9
 35 #define VTK_FLOAT          10
 36 #define VTK_DOUBLE         11
 37 #define VTK_ID_TYPE        12
 38 
 39 / These types are not currently supported by GetDataType, but are for
 40    completeness.  /
 41 #define VTK_STRING         13

	*/
	
typedef enum {
	VALUE_VOID = VTK_VOID,
	VALUE_BIT = VTK_BIT,
	VALUE_CHAR = VTK_CHAR,
	VALUE_SIGNED_CHAR = VTK_SIGNED_CHAR,
	VALUE_UNSIGNED_CHAR = VTK_UNSIGNED_CHAR,
	VALUE_SHORT = VTK_SHORT,
	VALUE_UNSIGNED_SHORT = VTK_UNSIGNED_SHORT,
	VALUE_INTEGER = VTK_INT,
	VALUE_UNSIGNED_INTEGER = VTK_UNSIGNED_INT,
	VALUE_LONG = VTK_LONG,
	VALUE_UNSIGNED_LONG = VTK_UNSIGNED_LONG,
	VALUE_FLOAT = VTK_FLOAT,
	VALUE_DOUBLE = VTK_DOUBLE,
	VALUE_IDTYPE = VTK_ID_TYPE,
	VALUE_STRING = VTK_STRING,
    VALUE_LONG_LONG = VTK_LONG_LONG,
    VALUE_UNSIGNED_LONG_LONG = VTK_UNSIGNED_LONG_LONG
} ValueType;

	
}

// 
#endif


