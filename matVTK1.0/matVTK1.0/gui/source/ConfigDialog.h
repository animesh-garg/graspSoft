


#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QtGui>

#include <ConfigAdapter.h>

class SceneConfiguration;

class TripletWidget;

class ConfigDialog : public QDialog {
	
    Q_OBJECT;
    
public:
	ConfigDialog(QWidget *parent);

    //template<typename T>
	void buildConfig(ConfigAdapterBase *adapter);
    //void buildConfig(void *bla);

    // remove all tabs;
    void clear();
    
    signals:
    void configChanged(SceneConfiguration*);


private:
    QWidget *configWidget(SceneConfiguration &config, string name, QWidget *baseWidget);
    
    QWidget *buildFlagWidget(const ConfigurationValue *value);
    QWidget *buildColorWidget(const ConfigurationValue *value);
    QWidget *buildSelectWidget(const ConfigurationValue *value);
    QWidget *buildNumberWidget(const ConfigurationValue *value, int min=-999999999, int max=999999999, int decimals=0);

    QWidget *buildSliderWidget(const ConfigurationValue *value, int min=0, int max=1000);
    
    QWidget* buildTripletWidget(const ConfigurationValue *value, int min=-999999999, int max=999999999);
    
    
    QDoubleSpinBox* doubleSpinner(double value, double minVal=-999999999.0, double maxVal=999999999.0, int decimals=2);
    
    QTabWidget *tabWidget;

    SceneConfiguration config;
    SceneConfiguration originalConfig;

    
public slots:
    void valueChanged();
    void reject();
    
};

class TripletWidget : public QWidget {
    
    Q_OBJECT;
    
public:
    TripletWidget() {
        
        QHBoxLayout *hLayout = new QHBoxLayout();
        for(int i=0; i<3; i++) {
            spinner[i] = new QDoubleSpinBox(this);
            spinner[i]->setMaximumWidth(80);
            QObject::connect(spinner[i], SIGNAL(valueChanged(double)), this, SLOT(updated(double)));
            
            hLayout->addWidget(spinner[i]);
            
        }
        
        this->configureSpinners(0.0, 1.0);
        
        
        this->lockedBox = new QCheckBox("interlock values");
        hLayout->addWidget(lockedBox);
        
        this->setLayout(hLayout);
    }
  
    void configureSpinners(double min, double max, double step=0.1, int decimals=2) {
        for(int i=0; i<3; i++) {
            spinner[i]->setSingleStep(step);
            spinner[i]->setMinimum(min);
            spinner[i]->setMaximum(max);
            spinner[i]->setDecimals(decimals);
         }
   }
	
	void SetData(double *d) {
		for (int i=0; i<3; i++) {
			value[i] = d[i];
			spinner[i]->setValue(d[i]);
		}
	}

    
    ~TripletWidget() {
        for(int i=0; i<3; i++) {
            delete spinner[i];
        }
        delete lockedBox;
    }

signals:
    void valueChanged(double*);
    
protected slots:
	void updated(double d) {

        
        for(int i=0; i<3; i++) {
            if(lockedBox->checkState() == Qt::Checked){
                value[i] = d;
                spinner[i]->setValue(d);
            }
            else value[i] = spinner[i]->value();
        }
        
        emit(valueChanged(value));
    }
	
protected:
  
    double value[3];
    QDoubleSpinBox* spinner[3];
    QCheckBox *lockedBox;

};


class ColorWidget : public QWidget {
    
    Q_OBJECT;
    
public:
    ColorWidget(QColor c) : color(c) {
        
        QHBoxLayout *layout = new QHBoxLayout();
        
        button = new QPushButton();
      
        button->setDefault(false);
		button->setAutoDefault(false);
		
        this->SetColor(c);
        
        QObject::connect(button, SIGNAL(clicked()), this, SLOT(selectColor()));
        
        layout->addWidget(button);
        
        this->setLayout(layout);
        
        colorDialog = new QColorDialog(c, this);
        //colorDialog->setOption(QColorDialog::NoButtons, true);
        QObject::connect(colorDialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(updatedColor(QColor)));
        QObject::connect(colorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(selectedColor(QColor)));
        QObject::connect(colorDialog, SIGNAL(rejected()), this, SLOT(revertColor()));
    }
    
    void SetColor(QColor col) {
        QPalette p(button->palette()); // = new QPalette();
        
        p.setColor(QPalette::Window, col);
        p.setColor(QPalette::Base, col);    
        p.setColor(QPalette::Text, col);    
        p.setColor(QPalette::Button, col);    
        button->setPalette(p);
        button->setAutoFillBackground(true);
        this->color = col;
    }
    
    ~ColorWidget() {
        //delete colorDialog;
    }
    
    
    
signals:
    void valueChanged(double*);
    
private slots:
    void revertColor() {
        this->updatedColor(this->color);
    }
    
    void selectedColor(QColor col) {
        this->color = col;
        this->SetColor(col);
        this->updatedColor(col);
    }
    
    void updatedColor(QColor col) {
        //this->SetColor(col);
        //double values[3] = {col.redF(), col.greenF(), col.blueF()};
        this->values[0] = col.redF();
        this->values[1] = col.greenF();
        this->values[2] = col.blueF(); 
        emit(valueChanged(values));
    }
    
    void selectColor() {
        colorDialog->open();
        //colorDialog->raiseWindow();
    }
    
    
private:
    
    double values[3];
    QPushButton *button;
    QColor color;
    QColorDialog *colorDialog;
};

#endif

