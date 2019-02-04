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

#include "hdf5.h"

#include <QXmlStreamWriter>
#include <QImageReader>
#include <qcolor.h>

using namespace Kst;

/**********************
HDF5Source::Config - This class defines the config widget that will be added to the 
Dialog Config Button for configuring the plugin.  This is only needed for special handling required
by the plugin.  Many plugins will not require configuration.  See plugins/sampleplugin for additional
details.

***********************/
class HDF5Source::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString()) {
      Q_UNUSED(fileName);
      cfg->beginGroup("HDF5");
      cfg->endGroup();
    }

    void save(QXmlStreamWriter& s) {
      Q_UNUSED(s);
    }

    void load(const QDomElement& e) {
      Q_UNUSED(e);
    }
};

// 
// Vector Interface
//

class DataInterfaceHDF5Vector : public DataSource::DataInterface < DataVector > {

public:
  DataInterfaceHDF5Vector(HDF5Source& s) : hdf(s) {}

  //read one element
  int read(const QString&, DataVector::ReadInfo&);

  //Named Elements
  QStringList list() const { return hdf._vectorList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString& field) const { return hdf._fieldList.contains( field ); }

  const DataVector::DataInfo dataInfo(const QString&, int frame = 0) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }

  HDF5Source& hdf;
};

const DataVector::DataInfo DataInterfaceHDF5Vector::dataInfo(const QString &field, int frame) const {
  Q_UNUSED(frame)

  if(!isValid(field)){
    return DataVector::DataInfo();
  }else{
    return DataVector::DataInfo(hdf.frameCount(field), hdf.samplesPerFrame(field));
  }
}

int DataInterfaceHDF5Vector::read(const QString& field, DataVector::ReadInfo& p){
  return hdf.readField(p.data, field, p.startingFrame, p.numberOfFrames);
}


//
// Matrix Interface
//

class DataInterfaceHDF5Matrix : public DataSource::DataInterface<DataMatrix>{
public:
  DataInterfaceHDF5Matrix(HDF5Source& s) : hdf(s) {}

  //read one element 
  int read(const QString&, DataMatrix::ReadInfo&);

  //named elements
  QStringList list() const { return hdf._matrixList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString& field) const { return hdf._matrixList.contains( field ); }

  // T Specific: still don't know what that means
  const DataMatrix::DataInfo dataInfo(const QString&, int frame = 0) const;
  void setDataInfo(const QString&, const DataMatrix::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }

  HDF5Source& hdf;
};

int DataInterfaceHDF5Matrix::read(const QString& field, DataMatrix::ReadInfo& p){
  //TODO: figure this out
  return hdf.readMatrix(p, field);
}

const DataMatrix::DataInfo DataInterfaceHDF5Matrix::dataInfo(const QString &field, int frame) const {

  Q_UNUSED(frame)
  if (!isValid(field)){
    return DataMatrix::DataInfo();
  }else{//get x and y dimentions of the matrix
    //Debug::self()->log(QString("Getting data info for matrix ") + field);
    DataMatrix::DataInfo info;
    if(field.contains("->")){
      QStringList list = field.split("->");

      QString fieldName = list[0];
      QString attrName = list[list.size() -1];
      H5::Attribute attr = hdf._hdfFile->openDataSet(qPrintable(fieldName)).openAttribute(qPrintable(attrName));

      hsize_t* sizes = new hsize_t[2];
      attr.getSpace().getSimpleExtentDims(sizes, NULL);
      info.xSize = sizes[0];
      info.ySize = sizes[1];
      //Debug::self()->log(QString::number(sizes[0]) + QString(" is xsize in datainfo, y is ") + QString::number(sizes[1]));
      delete[] sizes;
      return info;

    }else{

      H5::DataSet dataset = hdf._hdfFile->openDataSet(qPrintable(field));
      hsize_t* sizes = new hsize_t[2];
      dataset.getSpace().getSimpleExtentDims(sizes, NULL);
      info.xSize = sizes[0];
      info.ySize = sizes[1];
      //Debug::self()->log(QString::number(sizes[0]) + QString(" is xsize in datainfo, y is ") + QString::number(sizes[1]));
  
      delete[] sizes;
      return info;
    }
  }
}

//
// Scalar Interface
//

class DataInterfaceHDF5Scalar : public DataSource::DataInterface<DataScalar>{
public:
  DataInterfaceHDF5Scalar(HDF5Source & s) : hdf(s) {}

  //read one element
  int read(const QString&, DataScalar::ReadInfo&);

  //Named Elements 
  QStringList list() const { return hdf._scalarList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString& field) const { return hdf._scalarList.contains( field ); }

 //T specific: not used for scalars
 const DataScalar::DataInfo dataInfo(const QString&, int frame=0) const {Q_UNUSED(frame) return DataScalar::DataInfo(); }
 void setDataInfo(const QString&, const DataScalar::DataInfo&) {}

  //meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  HDF5Source& hdf;
};

int DataInterfaceHDF5Scalar::read(const QString& field, DataScalar::ReadInfo& p){
  return hdf.readScalar(*p.value, field);
}

//
// String Interface
//
class DataInterfaceHDF5String : public DataSource::DataInterface<DataString>
{
public:
  DataInterfaceHDF5String(HDF5Source& s) : hdf(s) {}

  // read one element
  int read(const QString&, DataString::ReadInfo&);

  // named elements
  QStringList list() const { return hdf._stringList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString& field) const { return hdf._stringList.contains( field ); }

  // T specific: not used for Strings
  virtual const DataString::DataInfo dataInfo(const QString&name, int frame=0) const;
  void setDataInfo(const QString&, const DataString::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  HDF5Source& hdf;
};

int DataInterfaceHDF5String::read(const QString& field, DataString::ReadInfo& p)
{
    return hdf.readString(*p.value, field);
}

const DataString::DataInfo DataInterfaceHDF5String::dataInfo(const QString & name, int frame) const
{
  Q_UNUSED(frame)
  //Debug::self()->log(QString("Getting data info for string ") + name);

  DataString::DataInfo info;
  info.frameCount = 1;

  return info;
}


/**********************
HDF5Source - This class defines the main DataSource which derives from DataSource.
The key functions that this class must provide is the ability to create the source, provide details about the source
be able to process the data.

***********************/
HDF5Source::HDF5Source(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type), _config(0L), 
  iv(new DataInterfaceHDF5Vector(*this)),
  ix(new DataInterfaceHDF5Scalar(*this)),
  im(new DataInterfaceHDF5Matrix(*this)),
  is(new DataInterfaceHDF5String(*this)), _resetNeeded(false)
{
  setInterface(iv);
  setInterface(ix);
  setInterface(im);
  setInterface(is);

  startUpdating(None);

  _valid = false;
  if (!type.isEmpty() && type != "HDF5") {
    return;
  }

  _config = new HDF5Source::Config;
  _config->read(cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  _directoryName = filename;

  if (init()) {
    _valid = true;
  }

  registerChange();
}

HDF5Source::~HDF5Source() {
  delete _config;
  _config = NULL;
  if(_hdfFile != NULL){
    delete _hdfFile;
  }
  _hdfFile = NULL;
}


void HDF5Source::reset() {
  if(_hdfFile){
    delete _hdfFile;
  }
  if(_config){
    delete _config;
  }

  init();
  Object::reset();
}

herr_t HDF5Source::visitFunc(hid_t id, const char* name, const H5L_info_t* info, void* opData){

    //Debug::self()->log(QString("Visiting ") + name);
    HDF5Source* h5Source = static_cast<HDF5Source*> (opData);

    herr_t status;
    H5O_info_t infobuf;

    if(QString(name).contains("->")){
      Debug::self()->log(QString("Skipping dataset ") + name + QString(" because it contains reserved string ->"));
      return 0;
    }

    status = H5Oget_info_by_name(id, name, &infobuf, H5P_DEFAULT);
    if(status == 0){
        if(infobuf.type == H5O_TYPE_DATASET){
            H5::DataSet dataset = h5Source->_hdfFile->openDataSet(name);
            H5::DataSpace space = dataset.getSpace();
            //try{
            H5::DataType type = dataset.getDataType();
            /*}catch (H5::Exception e){
              Debug::self()->log(QString("Caught exception ") + QString(e.getCDetailMsg()));

            }*/

            //stick scalar, vector and matrix data into the respective lists
            h5Source->_fieldList.append(name);

            int n_dims = space.getSimpleExtentNdims();
            //Debug::self()->log(QString::number(n_dims) + QString(" is dataspace n dims"));
            if(n_dims == 0){
                if(type.getClass() == H5T_STRING){//field is a string
                  h5Source->_stringList.append(name);
                  //Debug::self()->log(QString("Storing as string ") + name);
                }else if(type.getClass() == H5T_INTEGER || type.getClass() == H5T_FLOAT){
                  h5Source->_scalarList.append(name);
                }else{
                  Debug::self()->log(QString("Unknown scalar data type for field ") + name);
                  h5Source->_fieldList.removeLast();
                }
            }else if(n_dims == 1){
                h5Source->_vectorList.append(name);

                //get count of frames to see what index lengths we need
                unsigned len = h5Source->frameCount(name);
                //Debug::self()->log(QString("Length found to be ") + QString::number(len));

                if(!h5Source->lengths.contains(len) && len > 1){
                    h5Source->lengths.append(len);
                }
                if(len == 1){//len 1 vector ie. scalar
                    h5Source->_vectorList.removeLast();
                    h5Source->_scalarList.append(name);
                }
            }else if(n_dims == 2){
                //static matrix
                h5Source->_matrixList.append(name);
            }else if(n_dims == 3){
                //matrix per frame
                h5Source->_mpfList.append(name);
                unsigned len = h5Source->frameCount(name);
                if(!h5Source->lengths.contains(len)){
                    h5Source->lengths.append(len);
                }
            }else{
                //array is or too many dimentions to make sense, I guess do nothing?
                h5Source->_fieldList.removeLast();
                Debug::self()->log(QString("4+ dimention array found with name ") + name);
            }
            //Debug::self()->log(QString("Looking for attributes of dataset ") + name);

            //get attributes of the dataset as scalars
            hsize_t numAttr = 0;
            H5Aiterate(dataset.getId(), H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, &numAttr, (H5A_operator2_t) HDF5Source::attrIterFunc, opData);
            //Debug::self()->log(QString::number(numAttr) + QString(" attributes found"));
        }
    }
    return 0;
}

//this function should be called once per attribute
herr_t HDF5Source::attrIterFunc(hid_t id, const char* name, const H5A_info_t* info, void* opData){

  HDF5Source* h5Source = static_cast<HDF5Source*> (opData);
  //Debug::self()->log(QString("iterating through attribute: ") + QString(name));

  size_t len = H5Iget_name(id, NULL, 0);
  char* buffer = new char[len+ 1];
  H5Iget_name(id, buffer, len+1);

  QString attrName = QString(buffer) + QString("->") + QString(name);

  hid_t attrId = H5Aopen_name(id, name);
  hid_t type = H5Aget_type(attrId);
  H5S_class_t space = H5Sget_simple_extent_type(H5Aget_space(attrId));

  if(space == H5S_NULL || space == H5S_NO_CLASS){
    Debug::self()->log(QString("No type found for: ") + QString(attrName));
  }else{
    h5Source->_fieldList.append(attrName);
    int rank = H5Sget_simple_extent_ndims(H5Aget_space(attrId));

    //Debug::self()->log(QString("Ndims is: ") + QString::number(rank) + QString(" type is ") + QString::number(type));

    if(space == H5S_SCALAR){//string or scalar
      if(H5Tget_class(type) == H5T_STRING){
        h5Source->_stringList.append(attrName);
        //Debug::self()->log(QString("Storing attribute as string: ") + QString(attrName));
      }else if(H5Tget_class(type) == H5T_FLOAT || H5Tget_class(type) == H5T_INTEGER){
        //Debug::self()->log(QString("Storing attribute as scalar: ") + QString(attrName) + QString(" scalar list size ") + QString::number(h5Source->_scalarList.size()));
        h5Source->_fieldList.append(attrName);
        h5Source->_scalarList.append(attrName);
      }else{
        Debug::self()->log(QString("Datatype does not match for attribute: ") + QString(attrName) + QString(" type " ) + QString::number(H5Tget_class(type)));
      }
    }else if(space == H5S_SIMPLE){//matrix or vector
      //determine rank 
      if(rank == 1){
        //Debug::self()->log(QString("Storing attribute as vector: ") + QString(attrName));
        h5Source->_vectorList.append(attrName);

        //get count of frames to see what index lengths we need
        unsigned len = h5Source->frameCount(attrName);
        //Debug::self()->log(QString("Length found to be ") + QString::number(len));

        if(!h5Source->lengths.contains(len) && len > 1){
            h5Source->lengths.append(len);
        }
        if(len == 1){//len 1 vector ie. scalar
            h5Source->_vectorList.removeLast();
            h5Source->_scalarList.append(attrName);
        }
      }else if(rank == 2){
        //Debug::self()->log(QString("Storing attribute as matrix: ") + QString(attrName));
        h5Source->_matrixList.append(attrName);
      }else{
        Debug::self()->log(QString("Attribute ") + attrName +QString(" has unhandled data dimension: ") + QString::number(rank));
        h5Source->_fieldList.removeLast();
      }
    }else{
      Debug::self()->log(QString("Attribute ") + attrName + QString(" has unknown dataspace: ") + QString::number(space));
      h5Source->_fieldList.removeLast();
    }
  }
  delete[] buffer;
  return 0;
}

// If the datasource has any predefined fields they should be populated here.
bool HDF5Source::init() {

  _fieldList.clear();
  _scalarList.clear();
  _indexList.clear();
  _matrixList.clear();
  _stringList.clear();
  _mpfList.clear();
  _vectorList.clear();

  H5::Exception::dontPrint();
  try{
     _hdfFile = new H5::H5File(qPrintable(_directoryName), H5F_ACC_RDONLY);
  }catch(H5::Exception e){
    //dump to debug log somewhere
    Debug::self()->log(QString("Failed to open HDF5 file with name ") + _directoryName +QString(" ") + QString(e.getCDetailMsg()));
    _hdfFile = NULL;
    return false;
  }
  
  H5Lvisit(_hdfFile->getId(), H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, HDF5Source::visitFunc, (void*)this);

  //Add one index field per vector length
  for(int i =0; i<lengths.size(); i++){
    _fieldList.append(QString("INDEX-") + QString::number(lengths[i]));
    _vectorList.append(QString("INDEX-") + QString::number(lengths[i]));
    _indexList.append(QString("INDEX-") + QString::number(lengths[i]));
    //Debug::self()->log(QString("Adding to indexList ") + QString("INDEX-") + QString::number(lengths[i]));
  }

  registerChange();
  return true; // false if something went wrong
}

//TODO: make this do something
unsigned HDF5Source::samplesPerFrame(const QString& field){
  return 1;
}

unsigned HDF5Source::frameCount(const QString& field){
  //Debug::self()->log(QString("Getting frame count of ") + field);
  if(_indexList.contains(field)){ 
    //Debug::self()->log(QString("Returning index len ") + QString::number(lengths[_indexList.indexOf(field)]));
    return lengths[_indexList.indexOf(field)];
  }else if(_vectorList.contains(field)){
    if(field.contains("->")){//attribute vector
      QStringList list = field.split("->");

      QString fieldName = list[0];
      QString attrName = list[list.size() -1];
      try{
        H5::Attribute attr = _hdfFile->openDataSet(qPrintable(fieldName)).openAttribute(qPrintable(attrName));
        H5::DataSpace space = attr.getSpace();
        hsize_t size = 0;
        space.getSimpleExtentDims(&size, NULL);
        //Debug::self()->log(QString("Returning attr vec len ") + QString::number((int)size));

        return size;
      }catch(H5::Exception e){
        Debug::self()->log(QString("Problem reading dataset ") + field + QString(" ") + QString(e.getCDetailMsg()));
        return 0;
      }
    }else{
      try{
        H5::DataSet dataset = _hdfFile->openDataSet(qPrintable(field));
        H5::DataSpace space = dataset.getSpace();
    
        hsize_t size = 0;
        space.getSimpleExtentDims(&size, NULL);
        //Debug::self()->log(QString("Returning vec len ") + QString::number((int)size));
        return size;
      }catch(H5::Exception e){
        Debug::self()->log(QString("Failed to get frame count of ") + field);

      }
    }
  }else if(_mpfList.contains(field)){
    H5::DataSet dataset = _hdfFile->openDataSet(qPrintable(field));
    int nDims = dataset.getArrayType().getArrayNDims();
    hsize_t * sizes = new hsize_t[nDims];
    dataset.getArrayType().getArrayDims(sizes);
    int size = sizes[0];
    delete[] sizes;
    //Debug::self()->log(QString("Returning mpf len") + QString::number(size));
    return size;
  }else{
    //Debug::self()->log(QString("Returning 0"));
    return 0;
  }
  return 0;
}

int HDF5Source::readField(double* dataVec, const QString& name, int start, int numFrames){

  //Debug::self()->log(QString("Into readField, name: ") + name + QString(" start: ") + QString::number(start) + QString(" numFrames: ") + QString::number(numFrames));
  
  //Populate the index field provided by the plugin 
  if(_indexList.contains(name)){
    for(int i = start; i<numFrames + start; i++){
      dataVec[i-start] = i;
    }
    return numFrames;
  }else if(name.contains("->")){//attribute vector
    QStringList list = name.split("->");

    QString fieldName = list[0];
    QString attrName = list[list.size() -1];
    try{

      H5::DataSet dataset = _hdfFile->openDataSet(qPrintable(fieldName));
      H5::Attribute attr = dataset.openAttribute(qPrintable(attrName));

      double* temp = new double[frameCount(name)];
      attr.read(attr.getDataType(), temp);//can only read the whole attribute
      memcpy(dataVec, &temp[start], numFrames*samplesPerFrame(name)*sizeof(double));

      delete[] temp;
      return numFrames;
    }catch (H5::Exception e){
      Debug::self()->log(QString("Problem reading dataset ") + name + QString(" ") + QString(e.getCDetailMsg()));
    }

    return 0;
  }else{

    H5::DataSet dataset = _hdfFile->openDataSet(qPrintable(name)); 

    H5::DataSpace dataspace = dataset.getSpace();

    hsize_t dataSize = numFrames*samplesPerFrame(name);

    hsize_t offsetOut = 0;
    hsize_t memsize = (numFrames + 1)*samplesPerFrame(name);//this better not work
    //ugh this worked. No idea why, but memsize must be strictly larger than numframes
    //or else data offsets don't work. Really feels like an off-by-one in the hdf library

    H5::DataSpace memspace(1, &memsize);

    memspace.selectHyperslab( H5S_SELECT_SET, &dataSize, &offsetOut);

    hsize_t startSize = start*samplesPerFrame(name);
    hsize_t framesSize = numFrames*samplesPerFrame(name);

    //Debug::self()->log(QString("Going to read, datasize = ") + QString::number(dataSize));

    dataspace.selectHyperslab(H5S_SELECT_SET, &framesSize, &startSize);
  
    try{ 
      dataset.read((void*)dataVec, H5::PredType::NATIVE_DOUBLE, memspace, dataspace);
    }catch(H5::Exception e){
      Debug::self()->log(QString("Problem reading dataset ") + name + QString(" ") + QString(e.getCDetailMsg()));
      numFrames = 0;
    }catch(const std::exception& e1){
      Debug::self()->log(QString("Problem reading dataset ") + name + QString(" " ) + QString(e1.what()));
      numFrames = 0;
    }catch(...){
      Debug::self()->log(QString("Unknown problem reading dataset ") + name);
      numFrames = 0;
    }
  }
  return numFrames;
}

int HDF5Source::readScalar(double& scalar, const QString& field){

  scalar = 0;  
  if(_scalarList.contains(field)){

    if(field.contains(QString("->"))){//attribute scalar
      QStringList list = field.split("->");

      QString fieldName = list[0];
      QString attrName = list[list.size() -1];
      try{
        H5::DataSet data = _hdfFile->openDataSet(qPrintable(fieldName));
        H5::Attribute attr = data.openAttribute(qPrintable(attrName));
        H5::DataType type = H5::DataType(attr.getDataType());
        if(type.getClass() == H5T_FLOAT){
          attr.read(type, &scalar);
        }else if(type.getClass() == H5T_INTEGER){
          int i;
          attr.read(type, &i);
          scalar = i;
        }
      }catch(H5::FileIException e){
        Debug::self()->log(QString("Trouble opening group ") + fieldName + QString(" in readScalar, attribute: ") + attrName);
      }
    }else{
      H5::DataSet data = _hdfFile->openDataSet(qPrintable(field));
      if(data.getSpace().getSimpleExtentNdims() == 0){//real scalar
        H5::DataType type(data.getDataType());
        if(type.getClass() == H5T_FLOAT){
          data.read((void*)&scalar, type);
        }else if(type.getClass() == H5T_INTEGER){
          int i;
          data.read((void*)&i, type);
          scalar = i;
        }
      }else{//dataset of length one
        return this->readField(&scalar, field, 0, 1);
      }
    }
  }
  return 0;
}

int HDF5Source::readString(QString &data, const QString& field){

  //Debug::self()->log(QString("Reading string ") + field);
  H5std_string h5data;

  if(field.contains("->")){//Attribute string
    QStringList list = field.split("->");

    QString fieldName = list[0];
    QString attrName = list[list.size() -1];

    try{
      H5::DataSet dataset = _hdfFile->openDataSet(qPrintable(fieldName));
      H5::Attribute attr = dataset.openAttribute(qPrintable(attrName));
      attr.read(attr.getDataType(), h5data);
      data = QString(h5data.c_str());
      return 1;
    }catch(H5::Exception e){
      Debug::self()->log(QString("Problem reading attribute ") + field + QString(" ") + QString(e.getCDetailMsg()));
      return 0;
    }
  }else{//dataset string
    try{
      H5::DataSet dataset = _hdfFile->openDataSet(qPrintable(field));
      H5::StrType datatype(dataset);
      dataset.read(h5data, datatype);
      data=QString(h5data.c_str());
    }catch(H5::Exception e){
      Debug::self()->log(QString("Problem reading dataset ") + field + QString(" ") + QString(e.getCDetailMsg()));
      return 0;
    }
  }

  return 0;
}

int HDF5Source::readMatrix(DataMatrix::ReadInfo& data, const QString& name){
  //Debug::self()->log(QString("Reading matrix ") + name + QString(" xsize is ") + QString::number(data.xNumSteps) + QString(" ysize is ") + QString::number(data.yNumSteps) + QString(" xstart is ") + QString::number(data.xStart) + QString(" ystart is ") + QString::number(data.yStart));
  hsize_t dataSize = data.xNumSteps*data.yNumSteps;

  if(name.contains("->")){//attribute matrix
    QStringList list = name.split("->");

    QString fieldName = list[0];
    QString attrName = list[list.size() -1];
    try{
      H5::DataSet dataset = _hdfFile->openDataSet(qPrintable(fieldName));
      H5::Attribute attr = dataset.openAttribute(qPrintable(attrName));
      H5::DataSpace space = attr.getSpace();
      hsize_t* dims = new hsize_t[2];
      space.getSimpleExtentDims(dims, NULL);

      double* temp = new double[dims[0] * dims[1]];
      attr.read(attr.getDataType(), temp);//can only read the whole attribute
      for(int i=0; i<data.xNumSteps; i++){
        for(int j = 0; j<data.yNumSteps; j++){
          data.data->z[i + dims[0]*j] = temp[((int)data.xStart + i) + dims[0]*((int)data.yStart + j)];
        }
      }

      delete[] dims;
      delete[] temp;
      }catch (H5::Exception e){
        Debug::self()->log(QString("Problem reading dataset ") + name + QString(" ") + QString(e.getCDetailMsg()));
        dataSize = 0;
      }
  }else{
    H5::DataSet dataset = _hdfFile->openDataSet(qPrintable(name));

    H5::DataSpace dataspace = dataset.getSpace();

    hsize_t* memsize = new hsize_t[2];
    hsize_t* offset = new hsize_t[2];

    memsize[0] = data.xNumSteps;
    memsize[1] = data.yNumSteps;

    H5::DataSpace memspace(2, memsize);

    offset[0] = 0;
    offset[1] = 0;

    memspace.selectHyperslab(H5S_SELECT_SET, memsize, offset);

    hsize_t* startSize = new hsize_t[2];
    hsize_t* frameSize = new hsize_t[2];

    startSize[0] = data.xStart;
    startSize[1] = data.yStart;

    frameSize[0] = data.xNumSteps;
    frameSize[1] = data.yNumSteps;

    dataspace.selectHyperslab(H5S_SELECT_SET, frameSize, startSize);
  
    try{
      dataset.read((void*)data.data->z, H5::PredType::NATIVE_DOUBLE, memspace, dataspace);
    }catch(H5::Exception e){
      Debug::self()->log(QString("Problem reading dataset ") + name + QString(" ") + QString(e.getCDetailMsg()));
      dataSize = 0;
    }catch(const std::exception& e1){
      Debug::self()->log(QString("Problem reading dataset ") + name + QString(" " ) + QString(e1.what()));
      dataSize = 0;
    }catch(...){
      Debug::self()->log(QString("Unknown problem reading dataset ") + name);
      dataSize = 0;
    }
    delete[] memsize;
    delete[] offset;
    delete[] startSize;
    delete[] frameSize;
  }
  //set some matrix viewing parameters
  data.data->xMin = data.xStart;
  data.data->yMin = data.yStart;
  data.data->xStepSize = 1;
  data.data->yStepSize = 1;

  return dataSize;
}

// Check if the data in the from the source has updated.  Typically done by checking the frame count of the datasource for 
// changes.
Kst::Object::UpdateType HDF5Source::internalDataSourceUpdate() {
  return Kst::Object::NoChange;
}


// TODO a DataSource::DataInterface implementation as example






QString HDF5Source::fileType() const {
  return "HDF5";
}


void HDF5Source::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}





// Name used to identify the plugin.  Used when loading the plugin.
QString HDF5Plugin::pluginName() const { return tr("HDF5 Reader"); }
QString HDF5Plugin::pluginDescription() const { return tr("HDF5 Reader"); }

/**********************
HDF5Plugin - This class defines the plugin interface to the DataSource defined by the plugin.
The primary requirements of this class are to provide the necessary connections to create the object
which includes providing access to the configuration widget.

***********************/

Kst::DataSource *HDF5Plugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new HDF5Source(store, cfg, filename, type, element);
}


// Provides the matrix list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of matrices.
QStringList HDF5Plugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {


  if (typeSuggestion) {
    *typeSuggestion = "HDF5";
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
QStringList HDF5Plugin::scalarList(QSettings *cfg,
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
    *typeSuggestion = "HDF5";
  }

  //scalarList.append("FRAMES");
  return scalarList;

}


// Provides the string list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of strings if necessary.
QStringList HDF5Plugin::stringList(QSettings *cfg,
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
    *typeSuggestion = "HDF5";
  }

  //stringList.append("FILENAME");
  return stringList;

}


// Provides the field list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of fields if necessary.
QStringList HDF5Plugin::fieldList(QSettings *cfg,
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
    *typeSuggestion = "HDF5";
  }

  QStringList fieldList;

  if(understands(cfg, filename)){

    //TODO: open hdf file
    //get list of fields
    //I am confused about how field list is returned if this doesn't do anything
  }
  return fieldList;
}


// The main function used to determine if this plugin knows how to process the provided file.
// Each datasource plugin should check the file and return a number between 0 and 100 based 
// on the likelyhood of the file being this type.  100 should only be returned if there is no way
// that the file could be any datasource other than this one.
int HDF5Plugin::understands(QSettings *cfg, const QString& filename) const {
  
  Q_UNUSED(cfg)
  QFileInfo fi(filename);

  int returnVal = 0;
  if (fi.suffix() == "h5" || fi.suffix() == "hdf5"){
    returnVal += 40;
    if (H5::H5File::isHdf5(qPrintable(filename))){
      returnVal += 40;
    }
  }
  return returnVal;
}


bool HDF5Plugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList HDF5Plugin::provides() const {
  QStringList rc;
  rc += "HDF5";
  return rc;
}


// Request for this plugins configuration widget.  
Kst::DataSourceConfigWidget *HDF5Plugin::configWidget(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstdata_hdf5, HDF5Plugin)
#endif

// vim: ts=2 sw=2 et
