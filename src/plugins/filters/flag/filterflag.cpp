/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2016 C. Barth Netterfield
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2005  University of British Columbia                  *
 *                   dscott@phas.ubc.ca                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "filterflag.h"
#include "objectstore.h"
#include "ui_filterflagconfig.h"
#include "math_kst.h"


static const QString& VECTOR_IN = "Y Vector";
static const QString& VECTOR_FLAG_IN = "Flag Vector";
static const QString& VECTOR_OUT = "Y";

class ConfigWidgetFilterFlagPlugin : public Kst::DataObjectConfigWidget, public Ui_FilterFlagConfig {
  public:
    ConfigWidgetFilterFlagPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FilterFlagConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigWidgetFilterFlagPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) {
      _store = store;
      _vector->setObjectStore(store);
      _flag->setObjectStore(store);
      _mask->setText("0xffff");
      _validIsZero->setChecked(true);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_flag, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_mask, SIGNAL(textChanged(QString)), dialog, SIGNAL(modified()));
        connect(_validIsZero, SIGNAL(clicked(bool)), dialog, SIGNAL(modified()));
      }
    }

    void setVectorX(Kst::VectorPtr vector) {
      setSelectedVector(vector);
    }

    void setVectorY(Kst::VectorPtr vector) {
      setSelectedVector(vector);
    }

    void setVectorsLocked(bool locked = true) {
      _vector->setEnabled(!locked);
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    Kst::VectorPtr selectedFlag() { return _flag->selectedVector(); };
    void setSelectedFlag(Kst::VectorPtr vector) { return _flag->setSelectedVector(vector); };

    unsigned long long mask() {bool ok; return _mask->text().toULongLong(&ok, 0);}
    void setMask(unsigned long long mask_in) {_mask->setText("0x" + QString::number( mask_in, 16 ));}

    bool validIsZero() {return _validIsZero->isChecked();}
    void setValidIsZero(bool valid_is_zero) { _validIsZero->setChecked(valid_is_zero);}

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FilterFlagSource* source = static_cast<FilterFlagSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedFlag(source->flagVector());
        setMask(source->mask());
        setValidIsZero(source->validIsZero());
      }
    }

    virtual bool configurePropertiesFromXml(Kst::ObjectStore *store, QXmlStreamAttributes& attrs) {
      Q_UNUSED(store);

      bool validTag = true;

      QStringRef av;
      av = attrs.value("Mask");
      if (!av.isNull()) {
        _mask->setText(av.toString());
      } else {
        _mask->setText("0xffff");
      }

      av = attrs.value("ValidIsZero");
      if (!av.isNull()) {
        setValidIsZero(QVariant(av.toString()).toBool());
      } else {
        setValidIsZero(true);
      }

      return validTag;
    }

  public slots:
    virtual void save() {
      if (_cfg) {
        _cfg->beginGroup("Filter Flag Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Flag Vector", _flag->selectedVector()->Name());
        _cfg->setValue("Mask", QString::number( mask(), 16 ));
        _cfg->setValue("ValidIsZero", validIsZero());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Filter Flag Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }

        vectorName = _cfg->value("Flag Vector").toString();
        object = _store->retrieveObject(vectorName);
        vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          _flag->setSelectedVector(vector);
        }

        bool ok;
        setMask(_cfg->value("Mask", "0xffff").toString().toULongLong(&ok, 0));

        setValidIsZero(_cfg->value("ValidIsZero", true).toBool());

        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FilterFlagSource::FilterFlagSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FilterFlagSource::~FilterFlagSource() {
}


QString FilterFlagSource::_automaticDescriptiveName() const {
  if (vector()) {
    return tr("%1 Flagged", "arg 1 is the name of the vector which has been Flagged").arg(vector()->descriptiveName());
  } else {
    return tr("Flagged");
  }
}


QString FilterFlagSource::descriptionTip() const {
  QString tip;

  tip = tr("Flag Filter: %1\n  Flag: %2").arg(Name()).arg(flagVector()->Name());

  tip += tr("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}

void FilterFlagSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFilterFlagPlugin* config = static_cast<ConfigWidgetFilterFlagPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputVector(VECTOR_FLAG_IN, config->selectedFlag());
    _mask = config->mask();
    _validIsZero = config->validIsZero();
  }
}


void FilterFlagSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool FilterFlagSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::VectorPtr flagVector = _inputVectors[VECTOR_FLAG_IN];
  Kst::VectorPtr outputVector;
  // maintain kst file compatibility if the output vector name is changed.
  if (_outputVectors.contains(VECTOR_OUT)) {
    outputVector = _outputVectors[VECTOR_OUT];
  } else {
    outputVector = _outputVectors.values().at(0);
  }

  int N = inputVector->length();
  int i;
  if (N < 1) {
    return false;
  }

  // resize the output array
  outputVector->resize(inputVector->length(), false);

  if (_validIsZero) {
    for (i=0; i<N; ++i) {
      if ((unsigned long)flagVector->value(i) & _mask) { // invalid if flag & mask != 0
        outputVector->raw_V_ptr()[i] = Kst::NOPOINT;
      } else {
        outputVector->raw_V_ptr()[i] = inputVector->value(i);
      }
    }
  } else { // valid is nonzero
    for (i=0; i<N; ++i) {
      if ((unsigned long)flagVector->value(i) & _mask) { // valid if flag & mask != 0
        outputVector->raw_V_ptr()[i] = inputVector->value(i);
      } else {
        outputVector->raw_V_ptr()[i] = Kst::NOPOINT;
      }
    }
  }


  Kst::LabelInfo label_info = inputVector->labelInfo();
  label_info.name = tr("Flaged %1").arg(label_info.name);
  outputVector->setLabelInfo(label_info);

  return true;
}


Kst::VectorPtr FilterFlagSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::VectorPtr FilterFlagSource::flagVector() const {
  return _inputVectors[VECTOR_FLAG_IN];
}


QStringList FilterFlagSource::inputVectorList() const {
  QStringList input_vectors (VECTOR_IN);
  input_vectors += VECTOR_FLAG_IN;

  return input_vectors;
}


QStringList FilterFlagSource::inputScalarList() const {
  return QStringList(  );
}


QStringList FilterFlagSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FilterFlagSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList FilterFlagSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList FilterFlagSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}

void FilterFlagSource::setProperty(const QString &key, const QString &val) {
  if (key == "Mask") {
    bool ok;
    setMask(val.toLongLong(&ok, 0));
  } else if (key == "ValidIsZero") {
    if (val.toLower() == "true") {
      setValidIsZero(true);
    } else if (val.toLower() == "false") {
      setValidIsZero(false);
    }
  }
}


void FilterFlagSource::saveProperties(QXmlStreamWriter &s) {
  s.writeAttribute("Mask", "0x"+QString::number( mask(), 16 ));
  s.writeAttribute("ValidIsZero", QString::number(validIsZero()));
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FilterFlagPlugin::pluginName() const { return tr("Flag Filter"); }
QString FilterFlagPlugin::pluginDescription() const { return tr("Outputs the input vector flagged NaN when flag is non-zero."); }


Kst::DataObject *FilterFlagPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFilterFlagPlugin* config = static_cast<ConfigWidgetFilterFlagPlugin*>(configWidget)) {

    FilterFlagSource* object = store->createObject<FilterFlagSource>();

    if (setupInputsOutputs) {
      object->setupOutputs();
      object->setInputVector(VECTOR_IN, config->selectedVector());
      object->setInputVector(VECTOR_FLAG_IN, config->selectedFlag());
    }

    object->setMask(config->mask());
    object->setValidIsZero(config->validIsZero());

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *FilterFlagPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFilterFlagPlugin *widget = new ConfigWidgetFilterFlagPlugin(settingsObject);
  return widget;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstplugin_FilterFlagPlugin, FilterFlagPlugin)
#endif

// vim: ts=2 sw=2 et
