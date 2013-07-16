//
//  ConfigurationValue.h
//  matvtk_guiserver
//
//  Created by Erich Birngruber on 07.01.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef CONFIGURATION_VALUE_H
#define CONFIGURATION_VALUE_H

#include <limits>
#include <math.h>

#include "Constants.h"
#include "matvtkProtocol.h"
#include <StringTable.h>

#include <qobject.h>

typedef enum ConfigValueType {
    SCALAR,
    INTEGER,
    FLAG,
    RANGE,
    MATRIX,
    TRIPLET,
    COLOR,
    STRING
} ConfigValueType;


class ConfigurationValue : public QObject {
    
    Q_OBJECT;
    
public:
	
	ConfigurationValue() : value_scalar(0.0), value_matrix(NULL), len(0) { }
	
	ConfigurationValue(double value, ConfigValueType t=SCALAR) : value_scalar(value), value_matrix(NULL), len(0), type(t) {
	}
	
	ConfigurationValue(double *matrix, DataSize len) : type(MATRIX) {
		this->value_scalar = 0; //numeric_limits<double>::quiet_NaN();
		
		this->len = len;
        this->value_matrix = new double[len];
		
		memcpy(this->value_matrix, matrix, len);
	}
	
	ConfigurationValue(DataSize matrixLen) : value_scalar(0.0), type(MATRIX) {
		this->len = matrixLen;
        this->value_matrix = new double[len];
	}
	
	ConfigurationValue(std::string strValue) : value_string(strValue), value_scalar(0), value_matrix(NULL), len(0), type(STRING) {
	}

    ConfigurationValue(std::string valuespace, std::string strValue) : value_string(strValue), value_space(valuespace), value_scalar(0), value_matrix(NULL), len(0), type(STRING) {
	}

    
	~ConfigurationValue() {
		if(this->value_matrix) delete[] value_matrix;
	}
	
    
	bool IsString() const {
		return type == STRING;
	}
	
	bool IsScalar() const {
        return type == SCALAR || type==RANGE || type==FLAG || type==INTEGER;
	}
	
	bool IsMatrix() const {
        return type == MATRIX || type==COLOR || type==TRIPLET;
	}
	
	bool IsMatrix(DataSize len) const {
		return type==MATRIX && (this->len == len);
	}
	
	/*
     template<typename T>
     void SetPropertyValue(T* property, void (T::*stringSetter)(const char*)) const {
     (property->*stringSetter)(value_string.c_str());
     }
     */
    
	template<typename T>
	void SetPropertyValue(T* prop, void (T::*pointerSetter)(double*)) const {
		(prop->*pointerSetter)(value_matrix);
	}
	
    template<typename T>
	void SetPropertyValue(T* prop, void (T::*pointerSetter)(double)) const {
		(prop->*pointerSetter)(value_scalar);
	}
	template<typename T>
	void SetPropertyValue(T* prop, void (T::*pointerSetter)(float)) const {
		(prop->*pointerSetter)(value_scalar);
	}
	template<typename T>
	void SetPropertyValue(T* prop, void (T::*pointerSetter)(int)) const {
        
        int set_value = value_scalar;
        
        if(this->type == STRING) {
            set_value = StringTable::find(this->value_space, this->value_string);
            //qDebug() << " value space: '" << this->value_space.c_str() << "', looking for'" << this->value_string.c_str() << "', found value: '"<< set_value << "'";
            //Debug("CONVERTING STRING->INT");
            const_cast<ConfigurationValue*>(this)->value_scalar = set_value;
        }
        
            
        (prop->*pointerSetter)(set_value);
	}
    
	template<typename T>
	void GetPropertyValue(T* prop, double (T::*scalarGetter)()) {
		this->value_scalar = (prop->*scalarGetter)();
	}
    
	template<typename T>
	void GetPropertyValue(T* prop, float (T::*scalarGetter)()) {
		this->value_scalar = (prop->*scalarGetter)();
	}
	
	template<typename T>
	void GetPropertyValue(T* prop, int (T::*scalarGetter)()) {
		
        
        int prop_scalar = (prop->*scalarGetter)();

        
        if(this->GetType() == STRING) {
            //qDebug() << " value space: '" << this->value_space.c_str() << "' using value: '"<< prop_scalar << "'";
            this->value_string = StringTable::lookup(this->value_space, prop_scalar);
            //Debug("CONVERTING INT->STRING");
        }
        else this->value_scalar = prop_scalar;
        
	}
    
	template<typename T>
	void GetPropertyValue(T* prop, double* (T::*scalarGetter)()) {
		
		double *buf = (prop->*scalarGetter)();
        if(this->len==0) {
            this->len = 3;
            this->value_matrix = new double[len];
        }
        
        for(DataSize i=0; i<len; i++) this->value_matrix[i] = buf[i];
        
	}
    
    double GetScalar() const { 
        return this->value_scalar;
    }
    
    
    
    ConfigValueType GetType() const {
        return type;
    }
    
    void SetType(ConfigValueType t) {
        this->type = t;
    }
    
    double* GetMatrix() const {
        return value_matrix;
    }
    
    string GetValueSpace() const {
        return value_space;
    }
    
    string GetString() const {
        return value_string;
    }
    
    void SetValueSpace(string s) {
        this->value_space = s;
    }
    
    void SetValue(const ConfigurationValue* value) {
        this->value_string = value->value_string;
        this->value_space = value->value_space;

        this->value_scalar = value->value_scalar;

        this->value_matrix = new double[value->len];
        this->SetVector(value->value_matrix, value->len);
        
        this->type = value->type;
    }
    
    
    public slots:
    void SetScalar(double s) { this->value_scalar = s;}
    void SetScalar(int s) { 
        
        this->value_scalar = (double)s; 
        if(this->type==STRING) {
            this->value_string = StringTable::lookup(this->value_space, this->value_scalar);
        }
    }
    void SetScalarPromille(int p) { this->value_scalar = (double) p / 1000.0; };
    void SetBoolean(int b) { b == 0 ? this->value_scalar=0.0 : this->value_scalar=1.0; }
    void SetTriplet(double *d) {
        this->SetVector(d, 3);
    }
    void SetVector(double *vec, int len) {
        //if(this->value_matrix) delete this->value_matrix;
        
        this->len = len;
        
        for(int i=0; i<len; i++) {
            this->value_matrix[i] = vec[i];
        }
        

    }
    
protected:
	
	std::string value_string;
    std::string value_space;
    
	double value_scalar;
	

	double *value_matrix;
	DataSize len;
	
    ConfigValueType type;
    
	friend class SceneConfiguration;
};

/*
class StringConfigurationValue : public ConfigurationValue {
    
    
    template<typename T>
	void GetPropertyValue(T* prop, int (T::*scalarGetter)()) {
		int str_val = (prop->*scalarGetter)();
        value_string = table
        
	}
    
    template<typename T>
	void SetPropertyValue(T* prop, void (T::*pointerSetter)(int)) const {
		(prop->*pointerSetter)(value_scalar);
	}
    
    protected:
    StringTable table;
    
};
*/

#endif // CONFIGURATION_VALUE_H
