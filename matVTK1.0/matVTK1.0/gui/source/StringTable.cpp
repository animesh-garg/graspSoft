/* ********************************************************************
 matVTK: 3D visualization for Matlab
 see http://www.cir.meduniwien.ac.at/matvtk/
 
 If you use this software for research purposes, please cite
 the paper mentioned on the homepage in any resulting publication.  
 
 Developed at the Medical University of Vienna, Austria
 Erich Birngruber <erich.birngruber@meduniwien.ac.at>, 2009
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 see also LICENSE file or http://www.gnu.org/
 ******************************************************************** */

#include "StringTable.h"

#include "Constants.h"
#include "vtkCubeAxesActor.h"
#include "vtkTextProperty.h"

StringTable* StringTable::instance = NULL;

StringTable::StringTable() {
        
    // shading
    ListEntry shadingMap;
    shadingMap["flat"] = 0;
    shadingMap["gouroud"] = 1;
    shadingMap["phong"] = 2;
    table["interpolation"] = shadingMap;
        
    ListEntry volumeInterPolation;
	volumeInterPolation["nearest"] = 0;
	volumeInterPolation["linear"] = 1;
	table["volumeInterpolation"] = volumeInterPolation;
        
    // representation
    ListEntry representationMap;
    representationMap["points"] = 0;
    representationMap["wireframe"] = 1;
    representationMap["surface"] = 2;
    table["representation"] = representationMap;

    ListEntry cropTool;
    cropTool["box"] = 0;
    cropTool["plane"] = 1;
    table["cropTool"] = cropTool;

    ListEntry cameraProjection;
    cameraProjection["perspective"] = 0;
    cameraProjection["parallel"] = 1;
    table["cameraProjection"] = cameraProjection;

    ListEntry gridFly;
    gridFly["outerEdges"] = VTK_FLY_OUTER_EDGES;
    gridFly["closestTriad"] = VTK_FLY_CLOSEST_TRIAD;
    gridFly["furthestTriad"] = VTK_FLY_FURTHEST_TRIAD;
    gridFly["staticTriad"] = VTK_FLY_STATIC_TRIAD;
    gridFly["staticEdges"] = VTK_FLY_STATIC_EDGES;
    table["gridFly"] = gridFly;

    ListEntry gridTickLocation;
    gridTickLocation["inside"] = VTK_TICKS_INSIDE;
    gridTickLocation["outside"] = VTK_TICKS_OUTSIDE;
    gridTickLocation["both"] = VTK_TICKS_BOTH;
    table["gridTickLocation"] = gridTickLocation;


    ListEntry textJustification;
    textJustification["left"] = VTK_TEXT_LEFT;
    textJustification["centered"] = VTK_TEXT_CENTERED;
    textJustification["right"] = VTK_TEXT_RIGHT;
    table["textJustification"] = textJustification;


    ListEntry textVerticalJustification;
    textVerticalJustification["bottom"]=VTK_TEXT_BOTTOM;
    textVerticalJustification["centered"]=VTK_TEXT_CENTERED;
    textVerticalJustification["top"]=VTK_TEXT_TOP;
    table["textVerticalJustification"] = textVerticalJustification;


    ListEntry saveFormat;
    saveFormat["png"] = 0;
    saveFormat["vrml"] = 1;
    saveFormat["pov"] = 2;

    table["saveFormat"]=saveFormat;
  }
    

StringTable* StringTable::getInstance() {

	if(!instance) instance = new StringTable;
	
	return instance;
}

int StringTable::find(std::string tablename, std::string value) {
        
        TableMap::iterator iter = getInstance()->table.find(tablename);
        
        if(iter != getInstance()->table.end()) {
           ListEntry::iterator valueIter = iter->second.find(value);
            
            if(valueIter != iter->second.end()) {
                return valueIter->second;
            }
            else {
                Debug("Warning: no entry for %s\n", value.c_str());
            }
            
        }
        else Warning("value table not found error.\n");
        
        // if not found: return 0 == default
      return 0;
}

ListEntry StringTable::lookup(string tablename) {
    return getInstance()->table[tablename];
}

std::string StringTable::lookup(string tablename, int value) {
    
    ListEntry entry = StringTable::lookup(tablename);
    ListEntry::iterator iter;
    //mexDebug("lookup: %s -> %i\n", tablename.c_str(), value);

    for(iter = entry.begin(); iter!=entry.end(); iter++) {
      //mexDebug("checking: %i=%s\n", iter->second, iter->first.c_str());
        if( iter->second == value)
            return iter->first;
	}

    return "unknown";
}

