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

#ifndef PROPERTY_ENTRY_H
#define PROPERTY_ENTRY_H



#include "Constants.h"
#include "StringTable.h"
#include <ConfigurationValue.h>



template<typename P>
class PropertyEntryBase {
  
  public: 
        
        virtual ~PropertyEntryBase() { }

        virtual void SetValue(P* prop, const ConfigurationValue *data) = 0;
        virtual void GetValue(P* prop, ConfigurationValue *config) = 0;
        //virtual void ConfigValueType GetType() { return this->type; }
    
        void SetName(std::string name) { this->name = name; }
        std::string GetName() { return this->name; }
    

    
  protected:
	std::string name; 
    ConfigValueType type;
};


template<typename P, typename T>
class PropertyEntry : public PropertyEntryBase<P> {
public:
    PropertyEntry(std::string entryName, ConfigValueType aType, void (P::*setMethod)(T parm), T (P::*getMethod)(void)=NULL) : setter(setMethod), getter(getMethod) { 
        this->type = aType;
        this->name = entryName;
    }

    void SetValue(P* prop, const ConfigurationValue *value) {
        const_cast<ConfigurationValue*>(value)->SetType(this->type);
        const_cast<ConfigurationValue*>(value)->SetValueSpace(this->name);
        value->SetPropertyValue<P>(prop, setter);
        
    }
    
    void GetValue(P* prop, ConfigurationValue *value) {
        value->SetType(this->type);
        value->SetValueSpace(this->name);
        value->GetPropertyValue<P>(prop, getter);
    }
		
protected:
    void (P::*setter)(T parm);
    T (P::*getter)(void);
};

/*
template<typename P>
class StringPropertyEntry : public PropertyEntry<P,int> {
public:
    StringPropertyEntry(std::string entryName, void (P::*setMethod)(int parm), int (P::*getMethod)(void)=NULL) {
        
        this->setter = setMethod;
        this->getter = getMethod;
        this->name = entryName;
    }
    
    void SetValue(P* prop, const ConfigurationValue *value) {
        
        value->SetPropertyValue<P>(prop, this->setter);
    }
    
    void GetValue(P* prop, ConfigurationValue *value) {
        value->GetPropertyValue<P>(prop, this->getter);
    }
};
*/

#endif
