



#include "ConfigDialog.h"

#include <vtkActor.h>

#include <PropertyEntry.h>
#include <SceneConfiguration.h>

ConfigDialog::ConfigDialog(QWidget *parent) : QDialog(parent) {
	
	this->resize(640, 480);
	
	QVBoxLayout *verticalLayout = new QVBoxLayout();
	this->setLayout(verticalLayout);

	
	tabWidget = new QTabWidget(this);
	
	verticalLayout->addWidget(tabWidget);
	
    
	QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    //buttonBox->addButton(QDialogButtonBox::Close);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    
	verticalLayout->addWidget(buttonBox);
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

}

void ConfigDialog::reject() {
    
    qDebug() << "REJECTING SETTINGS, RESTORE OLD CONFIG";
    emit(configChanged(&(this->originalConfig)));
    
    this->config.Copy(&(this->originalConfig));
    /*
    double *col;
    
    col = this->originalConfig.GetField("color")->GetMatrix();
    qDebug() << "orig value: " << this->originalConfig.GetField("color");
    qDebug() << "orig color: " << col[0] << "," << col[1] << "," << col[2] << " ";

    col = this->config.GetField("color")->GetMatrix();
    qDebug() << "config value: " << this->config.GetField("color");
    qDebug() << "config color: " << col[0] << "," << col[1] << "," << col[2] << " ";
     */
    
    QDialog::reject();
}

void ConfigDialog::clear() {
    tabWidget->clear();
}


void ConfigDialog::buildConfig(ConfigAdapterBase *adapter) {
    
    QWidget *baseWidget = new QWidget(tabWidget);
    QFormLayout *formLayout = new QFormLayout();
    
    adapter->Write(&(this->config));
    this->originalConfig.Copy(&(this->config));

    
    std::list<string> names = adapter->names();
    
    std::list<string>::iterator it;
    
    qDebug() << "adapter: " << adapter->adapterName().c_str();
    for(it=names.begin(); it!=names.end(); ++it) {
        string name = (*it);
        qDebug() << " config entry: "  << name.c_str();
        
        formLayout->addRow(name.c_str(), configWidget(this->config, name, new QWidget(baseWidget)));
    }
    
    baseWidget->setLayout(formLayout);
    tabWidget->addTab(baseWidget, adapter->adapterName().c_str());
    
    // copy original config
    //adapter->Write(&(this->originalConfig));
    //qDebug() << "orig value: " << this->originalConfig.GetField("color");
    //qDebug() << "config value: " << this->config.GetField("color");
    this->originalConfig.Copy(&(this->config));

}

QWidget* ConfigDialog::configWidget(SceneConfiguration& config, string name, QWidget *baseWidget) {
    
    const ConfigurationValue *value = config.GetField(name);
    
    QWidget *dataWidget = NULL;

    switch(value->GetType()) {
        case SCALAR:
            //qDebug() << "scalar";
            dataWidget = buildNumberWidget(value, -99999999, 99999999, 2);
            break;
            
        case INTEGER:
            //qDebug() << "integer";
            dataWidget = buildNumberWidget(value, -99999999, 99999999, 0);
            break;
            
        case FLAG:
            //qDebug() << "flag";
            dataWidget = buildFlagWidget(value);
            break;
            
        case RANGE:
            //qDebug() << "range 0-1";
            dataWidget = buildSliderWidget(value);
            break;
            
        case MATRIX:
            //qDebug() << "MATRIX";
            dataWidget = new QLabel("UNSUPPORTED MATRIX", baseWidget);
            break;
            
        case COLOR:
            //qDebug() << "color";
            dataWidget = buildColorWidget(value);
            break;
            
        case TRIPLET:
            //qDebug() << "triplet";
            dataWidget = buildTripletWidget(value);
            break;
            
        case STRING:
            //qDebug() << "string";
            dataWidget = buildSelectWidget(value);

            break;
            
        default:
            //qDebug() << "dunno";
            dataWidget = new QLabel("UNKNOWN VALUE TYPE", baseWidget);
            break;
    }
    
    dataWidget->setParent(baseWidget);
    
    return dataWidget;
    
    //return baseWidget;
}

QWidget* ConfigDialog::buildFlagWidget(const ConfigurationValue *value) {
    
    QCheckBox *checkBox = new QCheckBox();

    if(value->GetScalar() == 0.0) checkBox->setCheckState(Qt::Checked);
    else checkBox->setCheckState(Qt::Unchecked);
    

    QObject::connect(checkBox, SIGNAL(stateChanged(int)), value, SLOT(SetBoolean(int)));
    QObject::connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(valueChanged()));
    
    return checkBox;
}

QWidget* ConfigDialog::buildColorWidget(const ConfigurationValue *value) {
    
    double* colors = value->GetMatrix();
    int red = colors[0]*255.0;
    int green = colors[1]*255.0;
    int blue = colors[2]*255.0;
    QColor col(red, green, blue);
    
    
    ColorWidget *w = new ColorWidget(col);
    
    QObject::connect(w, SIGNAL(valueChanged(double*)), value, SLOT(SetTriplet(double*)));
    QObject::connect(w, SIGNAL(valueChanged(double*)), this, SLOT(valueChanged()));
    
    return w;
}

QWidget* ConfigDialog::buildSelectWidget(const ConfigurationValue *value) {
    
    QComboBox *combo = new QComboBox();
    combo->setEditable(false);
    
    ListEntry entry = StringTable::lookup(value->GetValueSpace());
    
    //qDebug() << "value space: " << value->GetValueSpace().c_str();

    ListEntry::iterator iter;    
    for(iter = entry.begin(); iter!=entry.end(); iter++) {
        qDebug() << iter->second << " = " << iter->first.c_str();
        combo->insertItem(iter->second, iter->first.c_str(), QVariant(iter->second));
	}

    //int scalar = value->GetScalar();
    int scalar = StringTable::find(value->GetValueSpace(), value->GetString());
    
    combo->setCurrentIndex(scalar);
    
    QObject::connect(combo, SIGNAL(activated(int)), value, SLOT(SetScalar(int)));
    QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(valueChanged()));
    
    return combo;
}

QWidget* ConfigDialog::buildNumberWidget(const ConfigurationValue *value, int minVal, int maxVal, int decimals) {
    
    QDoubleSpinBox *spinner = doubleSpinner(value->GetScalar(), minVal, maxVal, decimals);
    

    QObject::connect(spinner, SIGNAL(valueChanged(double)), value, SLOT(SetScalar(double)));
    QObject::connect(spinner, SIGNAL(valueChanged(double)), this, SLOT(valueChanged()));
    
    return spinner;
}


QWidget* ConfigDialog::buildSliderWidget(const ConfigurationValue *value, int min, int max) {
    
    QSlider *slider = new QSlider(Qt::Horizontal);
    
    slider->setMinimum(min);
    slider->setMaximum(max);
    
    slider->setValue(value->GetScalar()*1000.0);
    slider->setMinimumWidth(200);
    //slider->setTickPosition(QSlider::TicksBelow);
    
    QObject::connect(slider, SIGNAL(valueChanged(int)), value, SLOT(SetScalarPromille(int)));
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged()));
    
    return slider;
}

QWidget* ConfigDialog::buildTripletWidget(const ConfigurationValue *value, int min, int max) {
    
    
    TripletWidget *triplet = new TripletWidget();

    
    triplet->configureSpinners(min, max);    
    triplet->SetData(value->GetMatrix());

    
	QObject::connect(triplet, SIGNAL(valueChanged(double*)), value, SLOT(SetTriplet(double*)));
    QObject::connect(triplet, SIGNAL(valueChanged(double*)), this, SLOT(valueChanged()));
    
    return triplet;
    
}

QDoubleSpinBox* ConfigDialog::doubleSpinner(double value, double minVal, double maxVal, int decimals) {
    QDoubleSpinBox *spinBox = new QDoubleSpinBox();
    
    spinBox->setMinimum(minVal);
    spinBox->setMaximum(maxVal);
    spinBox->setDecimals(decimals);
    
    if(decimals>0) {
        spinBox->setSingleStep(0.1);
    }
    else spinBox->setSingleStep(1.0);
    
    spinBox->setValue(value);
    
    return spinBox;
}

void ConfigDialog::valueChanged() {
    
    qDebug() << "CONFIG CHANGED, MUST NOTIFY MAIN WINDOW!";
       
    emit(configChanged(&(this->config)));
    
    const ConfigurationValue *value = this->config.GetField("color");
    if(value != NULL) {
        qDebug() << "color point:" << value;
        qDebug() << "color value:" << value->GetMatrix()[0] <<  value->GetMatrix()[1] <<  value->GetMatrix()[2];
    }


    value = this->config.GetField("ambientColor");
    if(value != NULL) {
        qDebug() << "ambient point:" << value;
        qDebug() << "ambient value:" << value->GetMatrix()[0] <<  value->GetMatrix()[1] <<  value->GetMatrix()[2];
    }
    

    value = this->config.GetField("diffuseColor");
    if(value != NULL) {
        qDebug() << "diffuse point:" << value;
        qDebug() << "diffuse value:" << value->GetMatrix()[0] <<  value->GetMatrix()[1] <<  value->GetMatrix()[2];
    }
    

    value = this->config.GetField("specularColor");
    if(value != NULL) {
        qDebug() << "specular point:" << value;
        qDebug() << "specular value:" << value->GetMatrix()[0] <<  value->GetMatrix()[1] <<  value->GetMatrix()[2];
    }

}

