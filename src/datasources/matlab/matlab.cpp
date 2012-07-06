/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "matlab.h"

#include <QXmlStreamWriter>
#include <QFileInfo>

using namespace Kst;

// Define enum to handle the various classes of variables with readable code
// (taken from matio 1.3.4, file mat5.c static const char *class_type_desc at line 33
// and turned into an enum). Some overlap with the data type, see below. I don't exactly
// understand why...
enum matio_class_type {UNDEFINED_CT,
                       CELL_ARRAY_CT,
                       STRUCTURE_CT,
                       OBJECT_CT,
                       CHARACTER_ARRAY_CT,
                       SPARSE_ARRAY_CT,
                       DOUBLE_PRECISION_ARRAY_CT,
                       SINGLE_PRECISION_ARRAY_CT,
                       EIGHT_BIT_SIGNED_INT_ARRAY_CT,
                       EIGHT_BIT_UNSIGNED_INT_ARRAY_CT,
                       SIXTEEN_BIT_SIGNED_INT_ARRAY_CT,
                       SIXTEEN_BIT_UNSIGNED_INT_ARRAY_CT,
                       THIRTYTWO_BIT_SIGNED_INT_ARRAY_CT,
                       THIRTYTWO_BIT_UNSIGNED_INT_ARRAY_CT,
                       MATLAB_ARRAY_CT,
                       COMPRESSED_DATA_CT};

// Define enum to handle the various data types with readable code
// (taken from matio 1.3.4, file mat5.c static const char *data_type_desc at line 39)
enum matio_data_type {UNKNOWN_DT,
                      EIGHT_BIT_SIGNED_INT_DT,
                      EIGHT_BIT_UNSIGNED_INT_DT,
                      SIXTEEN_BIT_SIGNED_INT_DT,
                      SIXTEEN_BIT_UNSIGNED_INT_DT,
                      THIRTYTWO_BIT_SIGNED_INT_DT,
                      THIRTYTWO_BIT_UNSIGNED_INT_DT,
                      IEEE_754_SINGLE_PRECISION_DT,
                      RESERVED_1_DT,
                      IEEE_754_DOUBLE_PRECISION_DT,
                      RESERVED_2_DT,
                      RESERVED_3_DT,
                      SIXTYFOUR_BIT_SIGNED_INT_DT,
                      SIXTYFOUR_BIT_UNSIGNED_INT_DT,
                      MATLAB_ARRAY_DT,
                      COMPRESSED_DATA_DT,
                      UTF8_CHARACTER_DATA_DT,
                      UTF16_CHARACTER_DATA_DT,
                      UTF32_CHARACTER_DATA_DT,
                      STRING_DT,
                      CELL_ARRAY_DT,
                      STRUCTURE_DT};


//
// Scalar interface
//

class DataInterfaceMatlabScalar : public DataSource::DataInterface<DataScalar>
{
public:
  DataInterfaceMatlabScalar(MatlabSource& s) : matlab(s) {}

  // read one element
  int read(const QString&, DataScalar::ReadInfo&);

  // named elements
  QStringList list() const { return matlab._scalarList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataScalar::DataInfo dataInfo(const QString&) const { return DataScalar::DataInfo(); }
  void setDataInfo(const QString&, const DataScalar::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  MatlabSource& matlab;
};


int DataInterfaceMatlabScalar::read(const QString& scalar, DataScalar::ReadInfo& p)
{
  return matlab.readScalar(p.value, scalar);
}


bool DataInterfaceMatlabScalar::isValid(const QString& scalar) const
{
  return  matlab._scalarList.contains( scalar );
}


//
// String interface
//

class DataInterfaceMatlabString : public DataSource::DataInterface<DataString>
{
public:
  DataInterfaceMatlabString(MatlabSource& s) : matlab(s) {}

  // read one element
  int read(const QString&, DataString::ReadInfo&);

  // named elements
  QStringList list() const { return matlab._strings.keys(); }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataString::DataInfo dataInfo(const QString&) const { return DataString::DataInfo(); }
  void setDataInfo(const QString&, const DataString::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  MatlabSource& matlab;
};


int DataInterfaceMatlabString::read(const QString& string, DataString::ReadInfo& p)
{
  if (isValid(string) && p.value) {
    *p.value = matlab._strings[string];
    return 1;
  }
  return 0;
}


bool DataInterfaceMatlabString::isValid(const QString& string) const
{
  return matlab._strings.contains( string );
}




/**********************
Vector interface
***********************/

class DataInterfaceMatlabVector : public DataSource::DataInterface<DataVector>
{
public:
  DataInterfaceMatlabVector(MatlabSource& s) : matlab(s) {}

  // read one element
  int read(const QString&, DataVector::ReadInfo&);

  // named elements
  QStringList list() const { return matlab._fieldList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataVector::DataInfo dataInfo(const QString&) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&);
  QMap<QString, QString> metaStrings(const QString&);


private:
  MatlabSource& matlab;
};


const DataVector::DataInfo DataInterfaceMatlabVector::dataInfo(const QString &field) const
{
  if (!matlab._fieldList.contains(field))
    return DataVector::DataInfo();

  return DataVector::DataInfo(matlab.frameCount(field), matlab.samplesPerFrame(field));
}



int DataInterfaceMatlabVector::read(const QString& field, DataVector::ReadInfo& p)
{
  return matlab.readField(p.data, field, p.startingFrame, p.numberOfFrames);
}


bool DataInterfaceMatlabVector::isValid(const QString& field) const
{
  return  matlab._fieldList.contains( field );
}

QMap<QString, double> DataInterfaceMatlabVector::metaScalars(const QString& field)
{
  Q_UNUSED(field);
  QMap<QString, double> fieldScalars;
  return fieldScalars;
}

QMap<QString, QString> DataInterfaceMatlabVector::metaStrings(const QString& field)
{
  QMap<QString, QString> fieldStrings;
  return fieldStrings;
}



//
// Matrix interface
//

class DataInterfaceMatlabMatrix : public DataSource::DataInterface<DataMatrix>
{
public:

  DataInterfaceMatlabMatrix(MatlabSource& s) : matlab(s) {}

  // read one element
  int read(const QString&, DataMatrix::ReadInfo&);

  // named elements
  QStringList list() const { return matlab._matrixList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataMatrix::DataInfo dataInfo	(const QString&) const;
  void setDataInfo(const QString&, const DataMatrix::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  MatlabSource& matlab;
};


const DataMatrix::DataInfo DataInterfaceMatlabMatrix::dataInfo(const QString& matrix) const
{
  if (!matlab._matrixList.contains( matrix ) ) {
    return DataMatrix::DataInfo();
  }

  QByteArray bytes = matrix.toLatin1();
  matvar_t *matvar = Mat_VarRead(matlab._matfile, bytes.data());
  if (!matvar) {
    return DataMatrix::DataInfo();
  }

  if (matvar->rank != 2) {
    return DataMatrix::DataInfo();
  }

  DataMatrix::DataInfo info;
  info.samplesPerFrame = 1;
  info.xSize = matvar->dims[0];
  info.ySize = matvar->dims[1];

  Mat_VarFree(matvar);

  return info;
}


int DataInterfaceMatlabMatrix::read(const QString& field, DataMatrix::ReadInfo& p)
{
  int count = matlab.readMatrix(p.data->z, field);

  p.data->xMin = 0;
  p.data->yMin = 0;
  p.data->xStepSize = 1;
  p.data->yStepSize = 1;

  return count;
}


bool DataInterfaceMatlabMatrix::isValid(const QString& field) const {
  return matlab._matrixList.contains( field );
}



/**********************
MatlabDatasourceSource - This class defines the main DataSource which derives from DataSource.
The key functions that this class must provide is the ability to create the source, provide details about the source
be able to process the data.

***********************/
MatlabSource::MatlabSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type),
  _matfile(0L),
  _config(0L),
  is(new DataInterfaceMatlabScalar(*this)),
  it(new DataInterfaceMatlabString(*this)),
  iv(new DataInterfaceMatlabVector(*this)),
  im(new DataInterfaceMatlabMatrix(*this))
  {
  setInterface(is);
  setInterface(it);
  setInterface(iv);
  setInterface(im);

  setUpdateType(None);

  if (!type.isEmpty() && type != "Matlab") {
    return;
  }

  _valid = false;
  _maxFrameCount = 0;

  _filename = filename;

  if (init()) {
    _valid = true;
  }

  registerChange();
}



MatlabSource::~MatlabSource() {
  Mat_Close(_matfile);
  _matfile = 0L;
}


void MatlabSource::reset() {
  Mat_Close(_matfile);
  _matfile = 0L;
  _maxFrameCount = 0;
  _valid = init();
}


// If the datasource has any predefined fields they should be populated here.
bool MatlabSource::init() {
  // First, try to open the file
  _matfile = Mat_Open(_filename.toStdString().c_str(),MAT_ACC_RDONLY);
  if (!_matfile) {
    _valid = false;
    return false;
  }

  _scalarList.clear();
  _fieldList.clear();
  _matrixList.clear();
  _strings.clear();

  // Some standard stuff
  _fieldList += "INDEX";
  _strings = fileMetas();

  _maxFrameCount = 0;

  // Now iterate over the variables and keep the usable ones plus store some interesting data like number of samples
  matvar_t *matvar = Mat_VarReadNextInfo(_matfile);
  while (matvar) {
    switch (matvar->class_type) {
    // All array types = matrix, scalar or vector - check rank and sizes to determine which
    case DOUBLE_PRECISION_ARRAY_CT:
    case SINGLE_PRECISION_ARRAY_CT:
    case EIGHT_BIT_SIGNED_INT_ARRAY_CT:
    case EIGHT_BIT_UNSIGNED_INT_ARRAY_CT:
    case SIXTEEN_BIT_SIGNED_INT_ARRAY_CT:
    case SIXTEEN_BIT_UNSIGNED_INT_ARRAY_CT:
    case THIRTYTWO_BIT_SIGNED_INT_ARRAY_CT:
    case THIRTYTWO_BIT_UNSIGNED_INT_ARRAY_CT:
    case MATLAB_ARRAY_CT:
    case COMPRESSED_DATA_CT:
      // Scalar
      if ( (matvar->rank == 1 && matvar->dims[0] == 1) ||
           (matvar->rank == 2 && matvar->dims[0] == 1 && matvar->dims[1] == 1) ) {
        _scalarList << QString(matvar->name);
        // qDebug() << "Found a scalar: " << matvar->name;
      }
      // Vector
      if ( (matvar->rank == 1 && matvar->dims[0] > 1) ||
           (matvar->rank == 2 && matvar->dims[0] == 1 && matvar->dims[1] > 1) || // matrix with one "flat" dim and at least 2 samples in the other direction
           (matvar->rank == 2 && matvar->dims[1] == 1 && matvar->dims[0] > 1) ) { // symmetrical case to the previous one
        _fieldList << QString(matvar->name);
        int fc = (matvar->rank == 1) ? matvar->dims[0] : qMax(matvar->dims[0], matvar->dims[1]);
        _maxFrameCount = qMax(_maxFrameCount, fc);
        _frameCounts[matvar->name] = fc;
        // qDebug() << "Found a vector: " << matvar->name << ", size: [" << matvar->dims[0] << "x" << matvar->dims[1] << "]";
      }
      // Dimension 2 matrix
      if ( matvar->rank == 2 && matvar->dims[0] > 1 && matvar->dims[1] > 1 )  {
        _matrixList << QString(matvar->name);
        // qDebug() << "Found a matrix: " << matvar->name << ", size: [" << matvar->dims[0] << "x" << matvar->dims[1] << "]";
      }
      break;

    case CHARACTER_ARRAY_CT: { // String
      matvar_t *string = Mat_VarRead(_matfile, matvar->name);
      _strings[QString(matvar->name)] = QString((char*)string->data);
      Mat_VarFree(string);
      // qDebug() << "Found a string: " << matvar->name << ", size: [" << matvar->dims[0] << "x" << matvar->dims[1] << "]";
      break;
    }

    default:
      // qDebug() << "Variable " << matvar->name << ", type not supported (" << matvar->class_type << ")";
      break;
    }


    matvar = Mat_VarReadNextInfo(_matfile);
  }
  Mat_VarFree(matvar);


  registerChange();
  return true; // false if something went wrong
}


// Check if the data in the from the source has updated.
// Considering how Matlab files are built up we can consider that they are always fixed
Kst::Object::UpdateType MatlabSource::internalDataSourceUpdate() {
  return Kst::Object::NoChange;
}


int MatlabSource::readScalar(double *v, const QString& field)
{
  matvar_t *matvar = Mat_VarRead(_matfile, field.toLatin1().data());
  if (matvar) { // TODO: add data type check. Or is the cast enough?
    *v = (double)*(double *)matvar->data;
    Mat_VarFree(matvar);
    return 1;
  }
  qDebug() << "Error reading scalar " << field;
  return 0;
}

int MatlabSource::readString(QString *stringValue, const QString& stringName)
{
  // TODO more error handling? Especially data type
  matvar_t *matvar = Mat_VarRead(_matfile, stringName.toLatin1().data());
  if (matvar) {
    *stringValue = QString((const char*)matvar->data);
    Mat_VarFree(matvar);
    return 1;
  }
  return 0;
}

int MatlabSource::readField(double *v, const QString& field, int s, int n) {

  KST_DBG qDebug() << "Entering MatlabSource::readField with params: " << field << ", from " << s << " for " << n << " frames" << endl;

  /* For INDEX field */
  if (field.toLower() == "index") {
    if (n < 0) {
      v[0] = double(s);
      return 1;
    }
    for (int i = 0; i < n; ++i) {
      v[i] = double(s + i);
    }
    return n;
  }

  /* For a variable from the Matlab file */
  matio_data_type dataType;
  matvar_t *matvar = Mat_VarRead(_matfile, field.toLatin1().data());
  if (!matvar) {
    KST_DBG qDebug() << "MatlabSource: queried field " << field << " which can't be read" << endl;
    return -1;
  }

  if (s >= _frameCounts[field]) {
    return 0;
  }

  dataType = (matio_data_type) matvar->data_type;
  switch (dataType) { // We have to be careful with the dimension of data elements
  case EIGHT_BIT_SIGNED_INT_DT:
  {
    int8_t *dataPointer = (int8_t*)matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i+s];
    }
  }
    break;

  case EIGHT_BIT_UNSIGNED_INT_DT:
  {
    uint8_t *dataPointer = (uint8_t*)matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i+s];
    }
  }
    break;

  case SIXTEEN_BIT_SIGNED_INT_DT:
  {
    int16_t *dataPointer = (int16_t*)matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i+s];
    }
  }
    break;

  case SIXTEEN_BIT_UNSIGNED_INT_DT:
  {
    uint16_t *dataPointer = (uint16_t*)matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i+s];
    }
  }
    break;

  case THIRTYTWO_BIT_SIGNED_INT_DT:
  {
    int32_t *dataPointer = (int32_t*)matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i+s];
    }
  }
    break;

  case THIRTYTWO_BIT_UNSIGNED_INT_DT:
  {
    uint32_t *dataPointer = (uint32_t*)matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i+s];
    }
  }
    break;

  case IEEE_754_SINGLE_PRECISION_DT:
  {
    float *dataPointer = (float*)matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i+s];
    }
  }
    break;

  case IEEE_754_DOUBLE_PRECISION_DT:
  {
    double *dataPointer = (double*)matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i+s];
    }
  }
    break;

  case SIXTYFOUR_BIT_SIGNED_INT_DT:
  {
    int64_t *dataPointer = (int64_t*)matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i+s];
    }
  }
    break;

  case SIXTYFOUR_BIT_UNSIGNED_INT_DT:
  {
    uint64_t *dataPointer = (uint64_t*)matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i+s];
    }
  }
    break;

  default:
      KST_DBG qDebug() << "MatlabSource, field " << field << ": wrong datatype for kst, no values read" << endl;
      return -1;
      break;
  }

  KST_DBG qDebug() << "Finished reading " << field << endl;
  Mat_VarFree(matvar);
  return n;
}


int MatlabSource::readMatrix(double *v, const QString& field)
{
  /* For a variable from the Matlab file */
  matvar_t *matvar = Mat_VarRead(_matfile, field.toLatin1().data());
  if (!matvar) {
    KST_DBG qDebug() << "MatlabSource: queried matrix " << field << " which can't be read" << endl;
    return -1;
  }

  // Matrices are always read from the beginning to the end
  // But we have to first store the data in a buffer of the right type, then copy it to v
  int n = matvar->dims[0] * matvar->dims[1];

  matio_data_type dataType = (matio_data_type) matvar->data_type;
  switch (dataType) {
  case EIGHT_BIT_SIGNED_INT_DT:
  {
    int8_t *dataPointer = (int8_t *) matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i];
    }
  }
    break;

  case EIGHT_BIT_UNSIGNED_INT_DT:
  {
    uint8_t *dataPointer = (uint8_t *) matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i];
    }
    free(dataPointer);
  }
    break;

  case SIXTEEN_BIT_SIGNED_INT_DT:
  {
    int16_t *dataPointer = (int16_t *) matvar->data;;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i];
    }
  }
    break;

  case SIXTEEN_BIT_UNSIGNED_INT_DT:
  {
    uint16_t *dataPointer = (uint16_t *) matvar->data;;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i];
    }
  }
    break;

  case THIRTYTWO_BIT_SIGNED_INT_DT:
  {
    int32_t *dataPointer = (int32_t *) matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i];
    }
  }
    break;

  case THIRTYTWO_BIT_UNSIGNED_INT_DT:
  {
    uint32_t *dataPointer = (uint32_t *) matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i];
    }
  }
    break;

  case IEEE_754_SINGLE_PRECISION_DT:
  {
    float *dataPointer = (float *) matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i];
    }
  }
    break;

  case IEEE_754_DOUBLE_PRECISION_DT:
  {
    double *dataPointer = (double *) matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i];
    }
  }
    break;

  case SIXTYFOUR_BIT_SIGNED_INT_DT:
  {
    int64_t *dataPointer = (int64_t *) matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i];
    }
  }
    break;

  case SIXTYFOUR_BIT_UNSIGNED_INT_DT:
  {
    uint64_t *dataPointer = (uint64_t *) matvar->data;
    for (int i = 0; i < n; ++i) {
      v[i] = (double)dataPointer[i];
    }
  }
    break;

  default:
      KST_DBG qDebug() << "MatlabSource, field " << field << ": wrong datatype for kst, no values read" << endl;
      return -1;
      break;
  }

  Mat_VarFree(matvar);

  return n;
}


int MatlabSource::frameCount(const QString& field) const {
  if (field.isEmpty() || field.toLower() == "index") {
    return _maxFrameCount;
  } else {
    return _frameCounts[field];
  }
}


QString MatlabSource::fileType() const {
  return "Matlab Datasource";
}


void MatlabSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}

int MatlabSource::samplesPerFrame(const QString& field) {
  return 1;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString MatlabSourcePlugin::pluginName() const { return "Matlab Datasource Reader"; }
QString MatlabSourcePlugin::pluginDescription() const { return "Matlab's .mat Datasource Reader"; }

/**********************
MatlabDatasourcePlugin - This class defines the plugin interface to the DataSource defined by the plugin.
The primary requirements of this class are to provide the necessary connections to create the object
which includes providing access to the configuration widget.

***********************/

Kst::DataSource *MatlabSourcePlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new MatlabSource(store, cfg, filename, type, element);
}


// Provides the matrix list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the
// list of matrices.
QStringList MatlabSourcePlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {


  if (typeSuggestion) {
    *typeSuggestion = "Matlab Datasource";
  }
  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }
  QStringList matrixList;

  return matrixList;

}


// Provides the scalar list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the
// list of scalars if necessary.
QStringList MatlabSourcePlugin::scalarList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {

  QStringList scalarList;

  if ((!type.isEmpty() && !provides().contains(type)) || 0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = "Matlab Datasource";
  }

  scalarList.append("FRAMES");
  return scalarList;

}


// Provides the string list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the
// list of strings if necessary.
QStringList MatlabSourcePlugin::stringList(QSettings *cfg,
                                      const QString& filename,
                                      const QString& type,
                                      QString *typeSuggestion,
                                      bool *complete) const {

  QStringList stringList;

  if ((!type.isEmpty() && !provides().contains(type)) || 0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = "Matlab Datasource";
  }

  stringList.append("FILENAME");
  return stringList;

}


// Provides the field list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the
// list of fields if necessary.
QStringList MatlabSourcePlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  Q_UNUSED(type)

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = "Matlab Datasource";
  }

  QStringList fieldList;
  return fieldList;
}


// The main function used to determine if this plugin knows how to process the provided file.
// Each datasource plugin should check the file and return a number between 0 and 100 based
// on the likelyhood of the file being this type.  100 should only be returned if there is no way
// that the file could be any datasource other than this one.
int MatlabSourcePlugin::understands(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)
  QFileInfo fi(filename);
  if (fi.suffix() == "mat") {
    return 80;
  } else return 0;
}



bool MatlabSourcePlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return false;
}


QStringList MatlabSourcePlugin::provides() const {
  QStringList rc;
  rc += "Matlab Datasource";
  return rc;
}


// Request for this plugins configuration widget.
Kst::DataSourceConfigWidget *MatlabSourcePlugin::configWidget(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}


Q_EXPORT_PLUGIN2(kstdata_matlab, MatlabSourcePlugin)


// vim: ts=2 sw=2 et
