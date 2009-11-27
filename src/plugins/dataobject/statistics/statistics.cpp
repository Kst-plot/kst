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


#include "statistics.h"
#include "objectstore.h"
#include "ui_statisticsconfig.h"

static const QString& VECTOR_IN = "Vector In";
static const QString& SCALAR_OUT_MEAN = "Mean";
static const QString& SCALAR_OUT_MINIMUM = "Minimum";
static const QString& SCALAR_OUT_MAXIMUM = "Maximum";
static const QString& SCALAR_OUT_VARIANCE = "Variance";
static const QString& SCALAR_OUT_STANDARD_DEVIATION = "Standard deviation";
static const QString& SCALAR_OUT_MEDIAN = "Median";
static const QString& SCALAR_OUT_ABSOLUTE_DEVIATION = "Absolute deviation";
static const QString& SCALAR_OUT_SKEWNESS = "Skewness";
static const QString& SCALAR_OUT_KURTOSIS = "Kurtosis";

class ConfigWidgetStatisticsPlugin : public Kst::DataObjectConfigWidget, public Ui_StatisticsConfig {
  public:
    ConfigWidgetStatisticsPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_StatisticsConfig() {
      setupUi(this);
    }

    ~ConfigWidgetStatisticsPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store); 
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (StatisticsSource* source = static_cast<StatisticsSource*>(dataObject)) {
        setSelectedVector(source->vector());
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
        _cfg->beginGroup("Statistics DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Statistics DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


StatisticsSource::StatisticsSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


StatisticsSource::~StatisticsSource() {
}


QString StatisticsSource::_automaticDescriptiveName() const {
  return QString("Statistics Plugin Object");
}


void StatisticsSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetStatisticsPlugin* config = static_cast<ConfigWidgetStatisticsPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
  }
}


void StatisticsSource::setupOutputs() {
  setOutputScalar(SCALAR_OUT_MEAN, "");
  setOutputScalar(SCALAR_OUT_MINIMUM, "");
  setOutputScalar(SCALAR_OUT_MAXIMUM, "");
  setOutputScalar(SCALAR_OUT_VARIANCE, "");
  setOutputScalar(SCALAR_OUT_STANDARD_DEVIATION, "");
  setOutputScalar(SCALAR_OUT_MEDIAN, "");
  setOutputScalar(SCALAR_OUT_ABSOLUTE_DEVIATION, "");
  setOutputScalar(SCALAR_OUT_SKEWNESS, "");
  setOutputScalar(SCALAR_OUT_KURTOSIS, "");
}


bool StatisticsSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr outputScalarMean = _outputScalars[SCALAR_OUT_MEAN];
  Kst::ScalarPtr outputScalarMin = _outputScalars[SCALAR_OUT_MINIMUM];
  Kst::ScalarPtr outputScalarMax = _outputScalars[SCALAR_OUT_MAXIMUM];
  Kst::ScalarPtr outputScalarVariance = _outputScalars[SCALAR_OUT_VARIANCE];
  Kst::ScalarPtr outputScalarSD = _outputScalars[SCALAR_OUT_STANDARD_DEVIATION];
  Kst::ScalarPtr outputScalarMedian = _outputScalars[SCALAR_OUT_MEDIAN];
  Kst::ScalarPtr outputScalarAD = _outputScalars[SCALAR_OUT_ABSOLUTE_DEVIATION];
  Kst::ScalarPtr outputScalarSkewness = _outputScalars[SCALAR_OUT_SKEWNESS];
  Kst::ScalarPtr outputScalarKurtosis = _outputScalars[SCALAR_OUT_KURTOSIS];

  //Make sure there is at least 1 element in the input vector
  if (inputVector->length() < 1) {
    _errorString = "Error:  Input Vector invalid size";
    return false;
  }

  double* pCopy;
  double dMean = 0.0;
  double dMedian = 0.0;
  double dStandardDeviation = 0.0;
  double dTotal = 0.0;
  double dSquaredTotal = 0.0;
  double dMinimum = 0.0;
  double dMaximum = 0.0;
  double dVariance = 0.0;
  double dAbsoluteDeviation = 0.0;
  double dSkewness = 0.0;
  double dKurtosis = 0.0;
  int iLength = inputVector->length();

  for (int i=0; i<iLength; i++) {
    if (i == 0 || inputVector->value()[i] < dMinimum) {
      dMinimum = inputVector->value()[i];
    }
    if (i == 0 || inputVector->value()[i] > dMaximum) {
      dMaximum = inputVector->value()[i];
    }
    dTotal += inputVector->value()[i];
    dSquaredTotal += inputVector->value()[i] * inputVector->value()[i];
  }

  dMean = dTotal / (double)iLength;
  if (iLength > 1) {
    dVariance  = 1.0 / ( (double)iLength - 1.0 );
    dVariance *= dSquaredTotal - ( dTotal * dTotal / (double)iLength ); 
    if (dVariance > 0.0) {
      dStandardDeviation = sqrt( dVariance );
    } else {
      dVariance = 0.0;
      dStandardDeviation = 0.0;
    }
  }

  for (int i=0; i<iLength; i++) {
    dAbsoluteDeviation += fabs( inputVector->value()[i] - dMean );
    dSkewness               += pow( inputVector->value()[i] - dMean, 3.0 );
    dKurtosis               += pow( inputVector->value()[i] - dMean, 4.0 );
  }
  dAbsoluteDeviation /= (double)iLength;
  dSkewness                 /= (double)iLength * pow( dStandardDeviation, 3.0 );
  dKurtosis                 /= (double)iLength * pow( dStandardDeviation, 4.0 );
  dKurtosis                 -= 3.0;

  /*
  sort by phase...
  */
  pCopy = (double*)calloc( iLength, sizeof( double ) );
  if (pCopy != NULL) {
    memcpy( pCopy, inputVector->value(), iLength * sizeof( double ) );
    quicksort( pCopy, 0, iLength-1 );
    dMedian = pCopy[ iLength / 2 ];

    free( pCopy );
  }

  outputScalarMean->setValue(dMean);
  outputScalarMin->setValue(dMinimum);
  outputScalarMax->setValue(dMaximum);
  outputScalarVariance->setValue(dVariance);
  outputScalarSD->setValue(dStandardDeviation);
  outputScalarMedian->setValue(dMedian);
  outputScalarAD->setValue(dAbsoluteDeviation);
  outputScalarSkewness->setValue(dSkewness);
  outputScalarKurtosis->setValue(dKurtosis);

  return true;
}


void StatisticsSource::swap( double* pData, int iOne, int iTwo) {
  double dTemp;

  dTemp = pData[iOne];
  pData[iOne] = pData[iTwo];
  pData[iTwo] = dTemp;
}


void StatisticsSource::quicksort( double* pData, int iLeft, int iRight) {

  double dVal = pData[iRight];
  int i = iLeft - 1;
  int j = iRight;

  if (iRight <= iLeft) {
    return;
  }

  while (1) {
    while (pData[++i] < dVal) {
    }

    while(dVal < pData[--j]) {
      if (j == iLeft) {
        break;
      }
    }
    if (i >= j) {
      break;
    }
    swap( pData, i, j );
  }
  swap( pData, i, iRight );
  quicksort( pData, iLeft, i-1 );
  quicksort( pData, i+1, iRight );
}


Kst::VectorPtr StatisticsSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


QStringList StatisticsSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList StatisticsSource::inputScalarList() const {
  return QStringList( /*SCALAR_IN*/ );
}


QStringList StatisticsSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList StatisticsSource::outputVectorList() const {
  return QStringList( /*VECTOR_OUT*/ );
}


QStringList StatisticsSource::outputScalarList() const {
  QStringList scalars(SCALAR_OUT_MEAN);
  scalars += SCALAR_OUT_MINIMUM;
  scalars += SCALAR_OUT_MAXIMUM;
  scalars += SCALAR_OUT_VARIANCE;
  scalars += SCALAR_OUT_STANDARD_DEVIATION;
  scalars += SCALAR_OUT_MEDIAN;
  scalars += SCALAR_OUT_ABSOLUTE_DEVIATION;
  scalars += SCALAR_OUT_SKEWNESS;
  scalars += SCALAR_OUT_KURTOSIS;
  return scalars;
}


QStringList StatisticsSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void StatisticsSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString StatisticsPlugin::pluginName() const { return "Statistics"; }
QString StatisticsPlugin::pluginDescription() const { return "Determines statistics for a given inputVector set."; }


Kst::DataObject *StatisticsPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetStatisticsPlugin* config = static_cast<ConfigWidgetStatisticsPlugin*>(configWidget)) {

    StatisticsSource* object = store->createObject<StatisticsSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN, config->selectedVector());
      object->setupOutputs();
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *StatisticsPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetStatisticsPlugin *widget = new ConfigWidgetStatisticsPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_ChopPlugin, StatisticsPlugin)

// vim: ts=2 sw=2 et
