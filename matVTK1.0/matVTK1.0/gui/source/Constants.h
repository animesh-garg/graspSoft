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

#ifndef CONSTANTS_H
#define CONSTANTS_H


#ifdef QT
#include <qdebug.h>
#endif
// orig: gcc only: #define mexDebug(fmt, args...) mexPrintf(fmt, ##args)

// debugging enabled, set debug macro
#ifdef DEBUG

#ifdef MEX
#define Debug( ... ) mexPrintf( __VA_ARGS__ )
#endif

#ifdef QT
#define Debug( ... ) qDebug( __VA_ARGS__ )
#endif

// not debugging
#else
#define Debug(...)
#endif


#ifdef MEX
#define Warning( ... ) mexWarnMsgTxt( __VA_ARGS__ )
#endif

#ifdef QT
#define Warning( ... ) qWarning( __VA_ARGS__ )
#endif

#endif
// CONSTANTS_H
