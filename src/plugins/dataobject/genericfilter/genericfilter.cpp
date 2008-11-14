/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "genericfilter.h"
#include "objectstore.h"
#include "ui_genericfilterconfig.h"

#include "polynom.h"
#include "filter.h"

static const QString& VECTOR_IN = "Vector In";
static const QString& SCALAR_IN = "Scalar In";
static const QString& STRING_IN_NUMERATOR = "String In Numerator";
static const QString& STRING_IN_DENOMINATOR = "String In Denominator";

static const QString& VECTOR_OUT = "Filtered";

class ConfigGenericFilterPlugin : public Kst::DataObjectConfigWidget, public Ui_GenericFilterConfig {
  public:
    ConfigGenericFilterPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_GenericFilterConfig() {
      setupUi(this);
    }

    ~ConfigGenericFilterPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store); 
      _scalarInterval->setObjectStore(store);
      _stringNumerator->setObjectStore(store);
      _stringDenominator->setObjectStore(store);
      _scalarInterval->setDefaultValue(0);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarInterval, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_stringNumerator, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_stringDenominator, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    Kst::ScalarPtr selectedScalar() { return _scalarInterval->selectedScalar(); };
    void setSelectedScalar(Kst::ScalarPtr scalar) { return _scalarInterval->setSelectedScalar(scalar); };

    Kst::StringPtr selectedStringNumerator() { return _stringNumerator->selectedString(); };
    void setSelectedStringNumerator(Kst::StringPtr string) { return _stringNumerator->setSelectedString(string); };

    Kst::StringPtr selectedStringDenominator() { return _stringDenominator->selectedString(); };
    void setSelectedStringDenominator(Kst::StringPtr string) { return _stringDenominator->setSelectedString(string); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (GenericFilterSource* source = static_cast<GenericFilterSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedScalar(source->scalarInterval());
        setSelectedStringNumerator(source->stringNumerator());
        setSelectedStringDenominator(source->stringDenominator());
      }
    }

    virtual bool configurePropertiesFromXml(Kst::ObjectStore *store, QXmlStreamAttributes& attrs) {
      Q_UNUSED(store);
      Q_UNUSED(attrs);

      bool validTag = true;

//       QStringRef av;
//       av = attrs.value("value");
//       if (!av.isNull()) {
//         _configValue = QVariant(av.toString()).toBool();
//       }

      return validTag;
    }

  public slots:
    virtual void save() {
      if (_cfg) {
        _cfg->beginGroup("Generic Filter DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Input Scalar Interval", _scalarInterval->selectedScalar()->Name());
        if (_stringNumerator->selectedString()) {
          _cfg->setValue("Input String Numerator", _stringNumerator->selectedString()->Name());
        }
        if (_stringDenominator->selectedString()) {
          _cfg->setValue("Input String Denominator", _stringDenominator->selectedString()->Name());
        }
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Generic Filter DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Input Scalar Interval").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* binScalar = static_cast<Kst::Scalar*>(object);
        if (binScalar) {
          setSelectedScalar(binScalar);
        }
        QString stringName = _cfg->value("Input String Numerator").toString();
        object = _store->retrieveObject(stringName);
        Kst::String* stringNum = static_cast<Kst::String*>(object);
        if (stringNum) {
          setSelectedStringNumerator(stringNum);
        }
        stringName = _cfg->value("Input String Denominator").toString();
        object = _store->retrieveObject(stringName);
        Kst::String* stringDenom = static_cast<Kst::String*>(object);
        if (stringDenom) {
          setSelectedStringDenominator(stringDenom);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


GenericFilterSource::GenericFilterSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


GenericFilterSource::~GenericFilterSource() {
}


QString GenericFilterSource::_automaticDescriptiveName() const {
  return QString("Generic Filter Plugin Object");
}


void GenericFilterSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigGenericFilterPlugin* config = static_cast<ConfigGenericFilterPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_IN, config->selectedScalar());
    setInputString(STRING_IN_NUMERATOR, config->selectedStringNumerator());
    setInputString(STRING_IN_DENOMINATOR, config->selectedStringDenominator());
  }
}


void GenericFilterSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool GenericFilterSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr inputScalar = _inputScalars[SCALAR_IN];
  Kst::StringPtr inputStringNumerator = _inputStrings[STRING_IN_NUMERATOR];
  Kst::StringPtr inputStringDenominator = _inputStrings[STRING_IN_DENOMINATOR];
  Kst::VectorPtr outputVector = _outputVectors[VECTOR_OUT];

  //Validate the Numerator.
  if (!inputStringNumerator || inputStringNumerator->value().isEmpty()) {
    _errorString = "Error:  Input String Numerator is Empty";
    return false;
  }

  //Validate the Denominator.
  if (!inputStringDenominator || inputStringDenominator->value().isEmpty()) {
    _errorString = "Error:  Input String Denominator is Empty";
    return false;
  }

  int length = inputVector->length();

  // Extract polynom coefficients and instantiate polynoms
  QStringList numCoeffs = QStringList::split(QRegExp("\\s*(,|;|:)\\s*"), inputStringNumerator->value());
  QStringList denCoeffs = QStringList::split(QRegExp("\\s*(,|;|:)\\s*"), inputStringDenominator->value());
  int numDegree = numCoeffs.count() - 1, denDegree = denCoeffs.count() - 1;
  polynom<double> Num(numDegree), Den(denDegree);
  double tmpDouble = 0.0;
  bool ok = false;
  for (int i=0; i<=numDegree; i++) {
    tmpDouble = numCoeffs[i].toDouble(&ok);
    if (ok) Num[i]= tmpDouble;
    else Num[i] = 0.0;
  }
  for (int i=0; i<=denDegree; i++) {
    tmpDouble = denCoeffs[i].toDouble(&ok);
    if (ok) Den[i] = tmpDouble;
    else Den[i] = 0.0;
  }

  // Time step
  double DeltaT = inputScalar->value();

  // Allocate storage for output vectors
  outputVector->resize(length, true);

  // Create filter
  filter<double> theFilter(Num,Den,DeltaT);
  double in = 0.0;
  theFilter.ConnectTo(in);
  theFilter.Reset();
  for (int i=0; i<length; i++) {
    in = inputVector->value()[i];
    theFilter.NextTimeStep();
    outputVector->value()[i] = theFilter.out;
  }

  return true;
}


Kst::VectorPtr GenericFilterSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr GenericFilterSource::scalarInterval() const {
  return _inputScalars[SCALAR_IN];
}


Kst::StringPtr GenericFilterSource::stringNumerator() const {
  return _inputStrings[STRING_IN_NUMERATOR];
}


Kst::StringPtr GenericFilterSource::stringDenominator() const {
  return _inputStrings[STRING_IN_DENOMINATOR];
}


QStringList GenericFilterSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList GenericFilterSource::inputScalarList() const {
  return QStringList( SCALAR_IN );
}


QStringList GenericFilterSource::inputStringList() const {
  QStringList strings(STRING_IN_NUMERATOR);
  strings += STRING_IN_DENOMINATOR;
  return strings;
}


QStringList GenericFilterSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList GenericFilterSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList GenericFilterSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void GenericFilterSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString GenericFilterPlugin::pluginName() const { return "Generic Filter"; }
QString GenericFilterPlugin::pluginDescription() const { return "Generates a discrete filter from a continuous-time definition."; }


Kst::DataObject *GenericFilterPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigGenericFilterPlugin* config = static_cast<ConfigGenericFilterPlugin*>(configWidget)) {

    GenericFilterSource* object = store->createObject<GenericFilterSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN, config->selectedVector());
      object->setInputScalar(SCALAR_IN, config->selectedScalar());
      object->setInputString(STRING_IN_NUMERATOR, config->selectedStringNumerator());
      object->setInputString(STRING_IN_DENOMINATOR, config->selectedStringDenominator());
      object->setupOutputs();
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->update();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *GenericFilterPlugin::configWidget(QSettings *settingsObject) const {
  ConfigGenericFilterPlugin *widget = new ConfigGenericFilterPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, GenericFilterPlugin)

// vim: ts=2 sw=2 et
