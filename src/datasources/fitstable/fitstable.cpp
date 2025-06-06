/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2014 Northwestern University                          *
 *                   nchapman@u.northwestern.edu                           *
 *                   g-novak@northwestern.edu                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "fitstable.h"

#include <QXmlStreamWriter>
#include <QFileInfo>

#define DBG if(0)

using namespace Kst;

/* Scalar interface */

class DataInterfaceFitsTableScalar : public DataSource::DataInterface<DataScalar>{
   public:
      DataInterfaceFitsTableScalar(FitsTableSource& s) : source(s) {}

      // read one element
      int read(const QString&, DataScalar::ReadInfo&);

      // named elements
      QStringList list() const { return source._scalars.keys(); }
      bool isListComplete() const { return true; }
      bool isValid(const QString&) const;

      // T specific
      const DataScalar::DataInfo dataInfo(const QString&) const { return DataScalar::DataInfo(); }
      void setDataInfo(const QString&, const DataScalar::DataInfo&) {}

      // meta data
      QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
      QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }

   private:
      FitsTableSource& source;
};


int DataInterfaceFitsTableScalar::read(const QString& scalar, DataScalar::ReadInfo& p){

   DBG qDebug() << "Entering DataInterfaceFitsTableScalar::read() with scalar: " << scalar << Qt::endl;
   return source.readScalar(p.value, scalar);
}


bool DataInterfaceFitsTableScalar::isValid(const QString& scalar) const{

   DBG qDebug() << "Entering DataInterfaceFitsTableScalar::isValid() with scalar: " << scalar << Qt::endl;
   return  source._scalars.contains( scalar );
}

/* String interface */

class DataInterfaceFitsTableString : public DataSource::DataInterface<DataString>{
   public:
      DataInterfaceFitsTableString(FitsTableSource& s) : source(s) {}

      // read one element
      int read(const QString&, DataString::ReadInfo&);

      // named elements
      QStringList list() const { return source._strings.keys(); }
      bool isListComplete() const { return true; }
      bool isValid(const QString&) const;

      // T specific
      const DataString::DataInfo dataInfo(const QString&) const { return DataString::DataInfo(); }
      void setDataInfo(const QString&, const DataString::DataInfo&) {}

      // meta data
      QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
      QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }

   private:
      FitsTableSource& source;
};


int DataInterfaceFitsTableString::read(const QString& string, DataString::ReadInfo& p){

   DBG qDebug() << "Entering DataInterfaceFitsTableString::read() with string: " << string << Qt::endl;
   if (isValid(string) && p.value) {
      *p.value = source._strings[string];
      return 1;
   }
   return 0;
}


bool DataInterfaceFitsTableString::isValid(const QString& string) const{

   DBG qDebug() << "Entering DataInterfaceFitsTableString::isValid() with string: " << string << Qt::endl;
   return source._strings.contains( string );
}

/* Vector interface */

class DataInterfaceFitsTableVector : public DataSource::DataInterface<DataVector>{
   public:
      DataInterfaceFitsTableVector(FitsTableSource& s) : source(s) {}

      // read one element
      int read(const QString&, DataVector::ReadInfo&);

      // named elements
      QStringList list() const { return source._fieldList; }
      bool isListComplete() const { return true; }
      bool isValid(const QString&) const;

      // T specific
      const DataVector::DataInfo dataInfo(const QString&) const;
      void setDataInfo(const QString&, const DataVector::DataInfo&) {}

      // meta data
      QMap<QString, double> metaScalars(const QString&);
      QMap<QString, QString> metaStrings(const QString&);

   private:
      FitsTableSource& source;
};


const DataVector::DataInfo DataInterfaceFitsTableVector::dataInfo(const QString &field) const{

   DBG qDebug() << "Entering DataInterfaceFitsTableVector::dataInfo() with field: " << field << Qt::endl;
   if (!source._fieldList.contains(field))
      return DataVector::DataInfo();

   return DataVector::DataInfo(source.frameCount(field), source.samplesPerFrame(field));
}

int DataInterfaceFitsTableVector::read(const QString& field, DataVector::ReadInfo& p){

   DBG qDebug() << "Entering DataInterfaceFitsTableVector::read() with field: " << field << Qt::endl;
   return source.readField(p.data, field, p.startingFrame, p.numberOfFrames);
}


bool DataInterfaceFitsTableVector::isValid(const QString& field) const{

   DBG qDebug() << "Entering DataInterfaceFitsTableVector::isValid() with field: " << field << Qt::endl;
   return  source._fieldList.contains(field);
}

QMap<QString, double> DataInterfaceFitsTableVector::metaScalars(const QString& field){
   Q_UNUSED(field);
   QMap<QString, double> fieldScalars;

   DBG qDebug() << "Entering DataInterfaceFitsTableVector::metaScalars() with field: " << field << Qt::endl;
   return fieldScalars;
}

QMap<QString, QString> DataInterfaceFitsTableVector::metaStrings(const QString& field){

   QMap<QString, QString> fieldStrings;
   DBG qDebug() << "Entering DataInterfaceFitsTableVector::metaStrings() with field: " << field << Qt::endl;
   return fieldStrings;
}

/* Matrix interface */

class DataInterfaceFitsTableMatrix : public DataSource::DataInterface<DataMatrix>{
   public:

      DataInterfaceFitsTableMatrix(FitsTableSource& s) : source(s) {}

      // read one element
      int read(const QString&, DataMatrix::ReadInfo&);

      // named elements
      QStringList list() const { return source._matrixList; }
      bool isListComplete() const { return true; }
      bool isValid(const QString&) const;

      // T specific
      const DataMatrix::DataInfo dataInfo	(const QString&) const;
      void setDataInfo(const QString&, const DataMatrix::DataInfo&) {}

      // meta data
      QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
      QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }

   private:
      FitsTableSource& source;
};


const DataMatrix::DataInfo DataInterfaceFitsTableMatrix::dataInfo(const QString& matrix) const{

   DBG qDebug() << "Entering DataInterfaceFitsTableMatrix::dataInfo() with matrix: " << matrix << Qt::endl;
   if (!source._matrixList.contains( matrix ) ) {
      return DataMatrix::DataInfo();
   }

   DataMatrix::DataInfo info;
   info.samplesPerFrame = 1;
   info.xSize = 32;
   info.ySize = 12;

   return info;
}


int DataInterfaceFitsTableMatrix::read(const QString& field, DataMatrix::ReadInfo& p){

   DBG qDebug() << "Entering DataInterfaceFitsTableMatrix::read() with field: " << field << Qt::endl;
   int count = source.readMatrix(p.data->z, field);

   p.data->xMin = 0;
   p.data->yMin = 0;
   p.data->xStepSize = 1;
   p.data->yStepSize = 1;

   return count;
}


bool DataInterfaceFitsTableMatrix::isValid(const QString& field) const {

   DBG qDebug() << "Entering DataInterfaceFitsTableMatrix::isValid() with field: " << field << Qt::endl;
   return source._matrixList.contains(field);
}

/**********************
FitsTableSource - This class defines the main DataSource which derives
from DataSource. The key functions that this class must provide is the ability
to create the source, provide details about the source be able to process the
data.
***********************/
FitsTableSource::FitsTableSource(Kst::ObjectStore *store, QSettings *cfg,
   const QString& filename, const QString& type, const QDomElement& e)
   : Kst::DataSource(store, cfg, filename, type), _fptr(0L), _config(0L),
   is(new DataInterfaceFitsTableScalar(*this)),
   it(new DataInterfaceFitsTableString(*this)),
   iv(new DataInterfaceFitsTableVector(*this)),
   im(new DataInterfaceFitsTableMatrix(*this)){

   DBG qDebug() << "Entering FitsTableSource::FitsTableSource() with filename: " << filename << Qt::endl;
   setInterface(is);
   setInterface(it);
   setInterface(iv);
   setInterface(im);

   setUpdateType(None);

   if (!type.isEmpty() && type != "FITS Table") {
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



FitsTableSource::~FitsTableSource() {

   DBG qDebug() << "Entering FitsTableSource::~FitsTableSource()\n";
   int status = 0;
   if (_fptr) {
      fits_close_file( _fptr, &status );
      _fptr = 0L;
   }
}


void FitsTableSource::reset() {

   DBG qDebug() << "Entering FitsTableSource::reset()\n";

   int status = 0;

   if (_fptr){
      fits_close_file(_fptr, &status);
      _fptr = 0L;
   }
   _maxFrameCount = 0;
   _valid = init();
}

int FitsTableSource::validField(int typecode){
   /* check to see if typcode is one of the supported ones.  If yes, return 1,
      else return 0 */

   int tmp;

   switch(typecode){
      case TINT:
      case TLONG: /* also covers TINT32BIT */
      case TLONGLONG:
      case TFLOAT:
      case TDOUBLE:
      case TLOGICAL:
         tmp = 1;
         break;
      default:
         tmp = 0;
   }
   return tmp;
}

// If the datasource has any predefined fields they should be populated here.
bool FitsTableSource::init() {
   int  status = 0;   /* cfitsio status flag */
   int  colnum;       /* column number in table */
   int  ncol;         /* number of columns in table */
   long nrow;         /* number of rows in table */
   char colname[512]; /* Name for column */
   int  typecode;     /* data type of column in FITS table */
   long width;        /* width codes from fits_get_coltype(). not used */
   int  hdutype;      /* FITS HDU type */
   long repeat;       /* used to determine if matrix or vector */
   long i,j,k;        /* loop variables */
   char coltemplate[512]; /* template for reading column names */
   QString tmp;       /* format string for bolometer array */
   int numHDU;        /* total number of HDUs in FITS file */
   int idx;           /* used to get index of elements in lists, so we don't
                         add the same scalar name, vector name, etc to the
                         relevant list twice */
   int naxis;           /* length of naxes variable.  Used for reading TDIM */
   long *naxes;         /* array to contain dimensionality of FITS columns */
   int maxdim;          /* maximum allowed array size of naxes */
   QStringList allcols; /* list of all column names */
   int nkeys;           /* Number of header keywords in current HDU */
   int keylength;       /* length of keyword in characters (not used) */
   int keyclass;        /* class of FITS header keyword, e.g., TYP_USER_KEY */
   char keyname[10];    /* array to hold FITS keyword name */
   char keyvalue[128];   /* array to hold FITS keyword value */
   char keycomment[128]; /* array to hold FITS keyword comment */
   char fitsrecord[128]; /* array to hold one line of FITS header */
   char keytype;        /* FITS key type (string, float, int, etc) */

   maxdim = 2;
   naxes = (long *) malloc(maxdim*sizeof(long));

   DBG qDebug() << "Entering FitsTableSource::init() with filename: " << _filename.toLatin1() << Qt::endl;
   // First, try to open the file
   if(fits_open_file( &_fptr, _filename.toLatin1(), READONLY, &status )){
      fits_close_file( _fptr, &status );
      _fptr = 0L;
      _valid = false;
      free(naxes);
      return false;
   }
   _scalars.clear();
   _fieldList.clear();
   _matrixList.clear();
   _strings.clear();
   _colName.clear();
   _colRepeat.clear();
   _colType.clear();
   _colOffset.clear();
   tableHDU.clear();
   tableRow.clear();

   // Some standard stuff
   _fieldList += "INDEX";
   _colName += "INDEX";
   _colRepeat << 1;
   _colType << 1;
   _colOffset << 0;

   _strings = fileMetas();
   _maxFrameCount = 0;

   /* get total number of HDUs */
   if (fits_get_num_hdus(_fptr, &numHDU, &status)){
      fits_report_error(stderr,status);
      _fptr = 0L;
      _valid = false;
      free(naxes);
      return false;
   } /* can't read number of HDUs, so quit */

   nrow = 0;
   for (i=1; i<= numHDU; i++){ /* loop over all HDUs */
      if (fits_movabs_hdu(_fptr, i, &hdutype, &status)){
         fits_report_error(stderr,status);
         status = 0;
         continue;
      } /* failed moving to an HDU, so try to skip and go to next one */
      /* read header to assign string and scalar values.  For now only
         read USER_KEYs.  May want to add UNIT_KEYs eventually, to get the
         units for every vector */
      if(fits_get_hdrspace(_fptr, &nkeys, NULL, &status)){ /* total # of keys */
         fits_report_error(stderr,status);
         status = 0;
         continue;
      }
      for (j=1; j<= nkeys; j++){ /* loop over keys */
         if(fits_read_record(_fptr, j, fitsrecord, &status)){
            fprintf(stderr,"Failed to read record number %ld\n",j);
            fits_report_error(stderr,status);
            status = 0;
            continue;
         } /* failed to read a record, so skip and continue */
         if (fits_get_keyname(fitsrecord, keyname, &keylength, &status)){
            fprintf(stderr,"Failed to read keyword from record = %s\n",fitsrecord);
            fits_report_error(stderr,status);
            status = 0;
            continue;
         } /* failed to read keyword name, so skip and continue */
         if (strcmp(keyname,"HISTORY") == 0) /* skip HISTORY keywords */
            continue;
         if (fits_parse_value(fitsrecord, keyvalue, keycomment, &status)){
            fprintf(stderr,"Failed to read value and comment from key = %s\n",keyname);
            fits_report_error(stderr,status);
            status = 0;
            continue;
         } /* failed to read keyword value or comment, so skip and continue */
         if (fits_get_keytype(keyvalue, &keytype, &status)){
            fprintf(stderr,"Failed to read type (string, int, float) for key = %s, value = %s\n",keyname,keyvalue);
            fits_report_error(stderr,status);
            status = 0;
            continue;
         } /* failed to read keytype, so skip and continue */
         keyclass = fits_get_keyclass(fitsrecord);
         if (keyclass == TYP_USER_KEY){
            if (keytype == 'C' || keytype == 'L'){ /* string or logical */
               _strings[QString(keyname)] = QString(keyvalue);
            } else if (keytype == 'I' || keytype == 'F'){ /* int or float */
               _scalars[QString(keyname)] = atof(keyvalue);
            }
         }
      }

      /* check if table HDU, and skip if not */
      if (hdutype == ASCII_TBL || hdutype == BINARY_TBL){
         if(fits_get_num_cols(_fptr, &ncol, &status)){ /* read # of columns */
            fprintf(stderr,"Failed to read # of columns in HDU = %ld\n",i);
            fits_report_error(stderr,status);
            status = 0;
            continue;
         } /* failed to read number of columns, so skip and continue */
         if(fits_get_num_rows(_fptr, &nrow, &status)){ /* read # of rows */
            fprintf(stderr,"Failed to read # of rows in HDU = %ld\n",i);
            fits_report_error(stderr,status);
            status = 0;
            continue;
         } /* failed to read number of rows, so skip and continue */
         tableRow << nrow;
         tableHDU << i;
      } else
         continue; /* skip non-table HDUs for reading table columns */
      for (j=1; j<= ncol; j++){
         sprintf(coltemplate,"%ld",j);
         if(fits_get_colname(_fptr, CASEINSEN, coltemplate, colname, &colnum, &status)){
            fprintf(stderr,"Failed to read column name for column = %ld\n",j);
            fits_report_error(stderr,status);
            status = 0;
            continue;
         } /* failed to read column name, so skip and continue */
         if(fits_get_coltype(_fptr, colnum, &typecode,&repeat,&width, &status)){
            fprintf(stderr,"Failed to read column type for column = %s\n",colname);
            fits_report_error(stderr,status);
            status = 0;
            continue;
         } /* failed to read column type, so skip and continue */
         if (validField(typecode)){
            if (repeat == 1){ /* not an array of values */
               idx = _fieldList.indexOf(QString(colname));
               if (idx == -1){ /* not present in list already */
                  _fieldList << QString(colname);
                  _colName << QString(colname);
                  _colRepeat << repeat;
                  _colType << typecode;
                  _colOffset << 0;
               }
               _frameCounts[QString(colname)] += nrow;
               if (_frameCounts[QString(colname)] > _maxFrameCount)
                  _maxFrameCount = _frameCounts[QString(colname)];
            } else{ /* is an array of values */
               /* TODO: should these be matrices instead (or perhaps as well?)
                  I don't understand how matrices should work with KST */
               if(fits_read_tdim(_fptr,colnum,maxdim, &naxis, naxes, &status)){
                  fprintf(stderr,"Failed to read dimensions of column = %s\n",colname);
                  fits_report_error(stderr,status);
                  status = 0;
                  continue;
               }
               for (k=0; k < repeat; k++){
                  if (naxis == 1)
                     tmp.sprintf("%s_%02ld",colname,k%naxes[0]+1);
                  else if (naxis == 2)
                     tmp.sprintf("%s_%02ld_%02ld",colname,k/naxes[0]+1,k%naxes[0]+1);
                  else /* 3-D arrays not supported right now */
                     break;
                  idx = _fieldList.indexOf(tmp);
                  if (idx == -1){ /* not present already */
                     _fieldList << tmp;
                     _colName << QString(colname);
                     _colRepeat << repeat;
                     _colType << typecode;
                     _colOffset << k;
                  }
                  _frameCounts[tmp] += nrow;
                  if (_frameCounts[QString(colname)] > _maxFrameCount)
                     _maxFrameCount = _frameCounts[QString(colname)];
               }
            }
         } else /* end if(validField) */
            fprintf(stderr,"Skipping unsupported type for column = %s\n",colname);
      } /* end loop over columns */
   } /* end loop over HDUs */

   /* set all fields to have _maxFrameCount size */
   allcols = _frameCounts.keys();
   for (i=0; i < allcols.size(); i++)
      _frameCounts[allcols[i]] = _maxFrameCount;
   registerChange();
   free(naxes);
   return true; // false if something went wrong
}

/* Check if the data in the from the source has updated. Considering how FITS
 files are built up we can consider that they are always fixed */

Kst::Object::UpdateType FitsTableSource::internalDataSourceUpdate() {

   DBG qDebug() << "Entering FitsTableSource::internalDataSourceUpdate()\n";
   return Kst::Object::NoChange;
}

int FitsTableSource::readScalar(double *v, const QString& field){
   DBG qDebug() << "Entering FitsTableSource::readScalar() with field: " << field << Qt::endl;

   *v = _scalars[field];
   return 1;
}

int FitsTableSource::readString(QString *stringValue, const QString& stringName){
   DBG qDebug() << "Entering FitsTableSource::readString() with field: " << stringName << Qt::endl;
   *stringValue = _strings[stringName];
   return 1;
}

int FitsTableSource::readField(double *v, const QString& field, int s, int n) {
   int status = 0; /* cfitsio status flag */
   int colnum;     /* column number in table */
   int anynul;     /* Number of null values read by fits_read_col() */
   int typecode;   /* data type of column in FITS table */
   long repeat;    /* used to determine if matrix or vector */
   void *data;     /* empty pointer for reading data */
   long nelements; /* size of data to read */
   long offset;    /* offset for data when repeat > 1 */
   long idx;       /* used when reading from Pixel Readout and tracking which
                      section of data array is being read by fits_read_col */
   long totalidx;  /* to keep track of our location in the v array */
   long i,j,k;     /* loop variables */
   long nrow;      /* number of rows read for each data chunk */
   long maxrow;    /* maximum number of rows to read at once.  Later re-used
                      to signify the number of rows to read for the current
                      HDU, which may not be tableRow[i], due to the fact
                      that the offset, s, may not be zero. */
   long currow;    /* current row index (when looping over data chunks */
   int hdutype;    /* FITS HDU type */
   char *colname;  /* Name for column */
   int firstHDU;   /* Flag to control whether we are reading the first HDU
                      containing data for a field.  Necessary to keep track of
                      s, the offset from the beginning of data */
   QByteArray ba;  /* needed to convert a QString to char array */

   DBG qDebug() << "Entering FitsTableSource::readField() with params: " << field << ", from " << s << " for " << n << " frames" << Qt::endl;

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

   idx       = _fieldList.indexOf(field); /* get index of field in list */
   repeat    = _colRepeat[idx];
   typecode  = _colType[idx];   /* FITS type code for field */
   ba        = _colName[idx].toLocal8Bit(); /* FITS column name of field */
   colname   = ba.data();
   offset    = _colOffset[idx]; /* data offset */

   firstHDU = 1;
   maxrow = 100000/repeat; /* cap on max rows to read at once */
   nelements = maxrow * repeat; /* so we read an integer number of 'repeat' at a time */
   if (typecode == TINT)
      data = (int *) malloc(nelements*sizeof(int));
   else if (typecode == TLONG || typecode == TINT32BIT)
      data = (long *) malloc(nelements*sizeof(long));
   else if (typecode == TLONGLONG)
      data = (long long *) malloc(nelements*sizeof(long long));
   else if (typecode == TFLOAT)
      data = (float *) malloc(nelements*sizeof(float));
   else if (typecode == TDOUBLE)
      data = (double *) malloc(nelements*sizeof(double));
   else if (typecode == TLOGICAL)
      data = (bool *) malloc(nelements*sizeof(bool));

   totalidx = 0;
   for (i=0; i < tableHDU.size(); i++){ /* loop over table HDUs */
      if (totalidx >= n) /* quit when we have read all data requested */
         break;
      if (fits_movabs_hdu(_fptr, tableHDU[i], &hdutype, &status)){
         fits_report_error(stderr,status);
         status = 0;
         continue;
      } /* failed moving to an HDU, so try to skip and go to next one */
      if(fits_get_colnum(_fptr, CASEINSEN, colname, &colnum, &status)){
         status = 0; /* column not found in this HDU, so skip. */
         continue;
      }
      if (firstHDU == 1){
         maxrow = tableRow[i] - s;
         firstHDU = 0;
         currow = s + 1; /* FITS starts counting from row 1 */
      } else{
         maxrow = tableRow[i];
         currow = 1; /* FITS starts counting from row 1 */
      }

      /* figure out how many chunks we need to read the rows in this HDU */
      idx = (maxrow*repeat)%nelements;
      if (idx == 0) /* divides evenly */
         idx = (maxrow*repeat)/nelements;
      else
         idx = (maxrow*repeat)/nelements + 1;
      for (j=0; j < idx; j++){ /* loop over row chunks */
         if (totalidx >= n) /* quit when we have read all data requested */
            break;
         if (j == (idx - 1)) /* last iteration */
            nrow = (maxrow*repeat - j*nelements)/repeat;
         else
            nrow = nelements/repeat;
         /* check to make sure we don't read more data than requested */
         if (totalidx + nrow > n)
            nrow = n - totalidx;
         if (typecode == TINT){
            if (fits_read_col(_fptr, typecode, colnum, currow, 1,
               nrow*repeat, NULL, &(((int *)data)[0]), &anynul, &status)){
               fprintf(stderr,"Failed to read column = %s, filling with NaNs\n",colname);
               fits_report_error(stderr,status);
               status = 0;
               for (k=0; k < nrow; k++)
                  v[totalidx+k] = sqrt(-1);
            } else {
               for (k=0; k < nrow; k++){
                  v[totalidx+k] = (double) ((int *)data)[k*repeat+offset];
               }
            }
         } else if (typecode == TLONG || typecode == TINT32BIT){
            if (fits_read_col(_fptr, typecode, colnum, currow, 1,
               nrow*repeat, NULL, &(((long *)data)[0]), &anynul, &status)){
               fprintf(stderr,"Failed to read column = %s, filling with NaNs\n",colname);
               fits_report_error(stderr,status);
               status = 0;
               for (k=0; k < nrow; k++)
                  v[totalidx+k] = sqrt(-1);
            } else {
               for (k=0; k < nrow; k++){
                  v[totalidx+k] = (double) ((long *)data)[k*repeat+offset];
               }
            }
         } else if (typecode == TLONGLONG){
            if (fits_read_col(_fptr, typecode, colnum, currow, 1,
               nrow*repeat, NULL, &(((long long *)data)[0]), &anynul, &status)){
               fprintf(stderr,"Failed to read column = %s, filling with NaNs\n",colname);
               fits_report_error(stderr,status);
               status = 0;
               for (k=0; k < nrow; k++)
                  v[totalidx+k] = sqrt(-1);
            } else {
               for (k=0; k < nrow; k++){
                  v[totalidx+k] = (double) ((long long *)data)[k*repeat+offset];
               }
            }
         }else if (typecode == TFLOAT){
            if (fits_read_col(_fptr, typecode, colnum, currow, 1,
               nrow*repeat, NULL, &(((float *)data)[0]), &anynul, &status)){
               fprintf(stderr,"Failed to read column = %s, filling with NaNs\n",colname);
               fits_report_error(stderr,status);
               status = 0;
               for (k=0; k < nrow; k++)
                  v[totalidx+k] = sqrt(-1);
            } else {
               for (k=0; k < nrow; k++)
                  v[totalidx+k] = (double) ((float *)data)[k*repeat+offset];
            }
         }else if (typecode == TDOUBLE){
            if (fits_read_col(_fptr, typecode, colnum, currow, 1,
               nrow*repeat, NULL, &(((double *)data)[0]), &anynul, &status)){
               fprintf(stderr,"Failed to read column = %s, filling with NaNs\n",colname);
               fits_report_error(stderr,status);
               status = 0;
               for (k=0; k < nrow; k++)
                  v[totalidx+k] = sqrt(-1);
            } else {
               for (k=0; k < nrow; k++)
                  v[totalidx+k] = (double) ((double *)data)[k*repeat+offset];
            }
         }else if (typecode == TLOGICAL){
            if (fits_read_col(_fptr, typecode, colnum, currow, 1,
               nrow*repeat, NULL, &(((bool *)data)[0]), &anynul, &status)){
               fprintf(stderr,"Failed to read column = %s, filling with NaNs\n",colname);
               fits_report_error(stderr,status);
               status = 0;
               for (k=0; k < nrow; k++)
                  v[totalidx+k] = sqrt(-1);
            } else {
               for (k=0; k < nrow; k++)
                  v[totalidx+k] = (double) ((bool *)data)[k*repeat+offset];
            }
         }
         totalidx += nrow;
         currow += nrow;
      } /* end loop over row chunks */
   } /* end loop over HDUs */

   for (i=totalidx; i< n; i++) /* fill remainder with NaNs */
      v[i] = sqrt(-1);
   free(data);
   return n;
}

int FitsTableSource::readMatrix(double *v, const QString& field){
   int status = 0; /* cfitsio status flag */
   int colnum;     /* column number in table */
   int anynul;     /* Number of null values read by fits_read_col() */
   int typecode;   /* data type of column in FITS table */
   long width;     /* width code from fits_get_coltype(). not used */
   long repeat;    /* used to determine if matrix or vector */
   long n;          /* total size */
   long nrow;      /* number of rows */
   void *data;     /* empty pointer for reading data */

   /* TODO: The code in this function is probably all wrong.  Since fitstable
      does not currently implement matrices, this function never gets called */
   DBG qDebug() << "Entering FitsTableSource::readMatrix() with field: " << field << Qt::endl;
   if (fits_get_colnum(_fptr, CASEINSEN, field.toLatin1().data(), &colnum, &status))
      fits_report_error(stderr,status);
   if (fits_get_coltype(_fptr, colnum, &typecode, &repeat, &width, &status))
      fits_report_error(stderr,status);
   if (fits_get_num_rows(_fptr, &nrow, &status))
      fits_report_error(stderr,status);

   n = repeat*nrow;
   if (typecode == TLONG || typecode == TINT32BIT)
      data = (int *) malloc(n*sizeof(int));
   else if (typecode == TLONGLONG)
      data = (long long *) malloc(n*sizeof(long long));
   else if (typecode == TFLOAT)
      data = (float *) malloc(n*sizeof(float));
   else if (typecode == TDOUBLE)
      data = (double *) malloc(n*sizeof(double));
   else if (typecode == TLOGICAL)
      data = (bool *) malloc(n*sizeof(bool));

   if (fits_read_col(_fptr, typecode, colnum, 1, 1, n, NULL, data, &anynul, &status))
      fits_report_error(stderr,status);
   for (int i=0; i < n; i++){
      if (typecode == TLONG || typecode == TINT32BIT)
         v[i] = (double) ((int *)data)[i];
      else if (typecode == TLONGLONG)
         v[i] = (double) ((long long *)data)[i];
      else if (typecode == TFLOAT)
         v[i] = (double) ((float *)data)[i];
      else if (typecode == TDOUBLE)
         v[i] = (double) ((double *)data)[i];
      else if (typecode == TLOGICAL)
         v[i] = (double) ((bool *)data)[i];
   }

   free(data);
   return n;
}

int FitsTableSource::frameCount(const QString& field) const {

   DBG qDebug() << "Entering FitsTableSource::frameCount() with field: " << field << Qt::endl;
   if (field.isEmpty() || field.toLower() == "index") {
      return _maxFrameCount;
   } else {
      return _frameCounts[field];
   }
}

QString FitsTableSource::fileType() const {
   return "FITS Table Datasource";
}


void FitsTableSource::save(QXmlStreamWriter &streamWriter) {
   Kst::DataSource::save(streamWriter);
}

int FitsTableSource::samplesPerFrame(const QString& field) {
   return 1;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitsTableSourcePlugin::pluginName() const { return "FITS Table Datasource Reader"; }
QString FitsTableSourcePlugin::pluginDescription() const { return "FITS Table Datasource Reader"; }

/**********************
FitsTablesourcePlugin - This class defines the plugin interface to the DataSource
defined by the plugin. The primary requirements of this class are to provide the
necessary connections to create the object which includes providing access to
the configuration widget.
***********************/

Kst::DataSource *FitsTableSourcePlugin::create(Kst::ObjectStore *store,
                                           QSettings *cfg,
                                           const QString &filename,
                                           const QString &type,
                                           const QDomElement &element) const {

   return new FitsTableSource(store, cfg, filename, type, element);
}


// Provides the matrix list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the
// list of matrices.
QStringList FitsTableSourcePlugin::matrixList(QSettings *cfg,
                                          const QString& filename,
                                          const QString& type,
                                          QString *typeSuggestion,
                                          bool *complete) const {

   DBG qDebug() << "Entering FitsTableSourcePlugin::matrixList()\n";
   if (typeSuggestion) {
      *typeSuggestion = "FITS Table Datasource";
   }
   if ((!type.isEmpty() && !provides().contains(type)) || 0 == understands(cfg, filename)) {
      if (complete) {
         *complete = false;
      }
      return QStringList();
   }
   QStringList matrixList;

   return matrixList;

}


/* Provides the scalar list that this dataSource can provide from the provided
filename. This function should use understands to validate the file and then
open and calculate the list of scalars if necessary.
*/
QStringList FitsTableSourcePlugin::scalarList(QSettings *cfg,
                                          const QString& filename,
                                          const QString& type,
                                          QString *typeSuggestion,
                                          bool *complete) const {

   QStringList scalarList;

   DBG qDebug() << "Entering FitsTableSourcePlugin::scalarList()\n";
   if ((!type.isEmpty() && !provides().contains(type)) || 0 == understands(cfg, filename)) {
      if (complete) {
         *complete = false;
      }
      return QStringList();
   }

   if (typeSuggestion) {
      *typeSuggestion = "FITS Table Datasource";
   }

   scalarList.append("FRAMES");
   return scalarList;
}


/* Provides the string list that this dataSource can provide from the provided
filename. This function should use understands to validate the file and then
open and calculate the list of strings if necessary.
*/
QStringList FitsTableSourcePlugin::stringList(QSettings *cfg,
                                          const QString& filename,
                                          const QString& type,
                                          QString *typeSuggestion,
                                          bool *complete) const {

   QStringList stringList;

   DBG qDebug() << "Entering FitsTableSourcePlugin::stringList()\n";
   if ((!type.isEmpty() && !provides().contains(type)) || 0 == understands(cfg, filename)) {
      if (complete) {
         *complete = false;
      }
      return QStringList();
   }

   if (typeSuggestion) {
      *typeSuggestion = "FITS Table Datasource";
   }

   stringList.append("FILENAME");
   return stringList;
}


/* Provides the field list that this dataSource can provide from the provided
filename. This function should use understands to validate the file and then
open and calculate the list of fields if necessary.
*/
QStringList FitsTableSourcePlugin::fieldList(QSettings *cfg,
                                         const QString& filename,
                                         const QString& type,
                                         QString *typeSuggestion,
                                         bool *complete) const {
   Q_UNUSED(cfg)
   Q_UNUSED(filename)
   Q_UNUSED(type)

   DBG qDebug() << "Entering FitsTableSourcePlugin::fieldList()\n";
   if (complete) {
      *complete = true;
   }

   if (typeSuggestion) {
      *typeSuggestion = "FITS Table Datasource";
   }

   QStringList fieldList;
   return fieldList;
}

/* The main function used to determine if this plugin knows how to process the
provided file. Each datasource plugin should check the file and return a number
between 0 and 100 based on the likelyhood of the file being this type.  100
should only be returned if there is no way that the file could be any datasource
other than this one.
*/
int FitsTableSourcePlugin::understands(QSettings *cfg, const QString& filename) const {
   /* This function will attempt to open the file as a FITS file, then loop
      through the HDUs to see if any are binary or ascii tables.  If so, and
      if any table has > 0 rows, then this is a FITS table that can be read
      by this plugin.  A value of 80 is returned, otherwise zero */
   Q_UNUSED(cfg)

   fitsfile *ff;
   int status = 0;    /* FITS error status */
   int i;             /* loop variable */
   int ret_val = 0;
   int numHDU;        /* total number of HDUs in FITS file */
   int hdutype;       /* FITS HDU type */
   long nrow;         /* number of rows in table */

   DBG qDebug() << "Entering FitsTableSourcePlugin::understands()\n";
   if(fits_open_file(&ff, filename.toLatin1(), READONLY, &status)){
      fits_close_file(ff,&status);
      return 0;
   }
   if (fits_get_num_hdus(ff, &numHDU, &status)){
      fits_close_file(ff,&status);
      return 0;
   }
   for(i=1; i<= numHDU; i++){
      if (fits_movabs_hdu(ff, i, &hdutype, &status)){
         fits_report_error(stderr,status);
         status = 0;
         continue;
      }
      if (hdutype == ASCII_TBL || hdutype == BINARY_TBL){
         if(fits_get_num_rows(ff, &nrow, &status)){
            fits_report_error(stderr,status);
            status = 0;
            continue;
         }
         if (nrow > 0){
            ret_val = 80;
            break;
         }
      }
   }
   status = 0;
   fits_close_file(ff,&status);
   return ret_val;
}

bool FitsTableSourcePlugin::supportsTime(QSettings *cfg, const QString& filename) const {
   //FIXME
   Q_UNUSED(cfg)
   Q_UNUSED(filename)
   return false;
}


QStringList FitsTableSourcePlugin::provides() const {
   QStringList rc;
   rc += "FITS Table Datasource";
   return rc;
}


// Request for this plugins configuration widget.
Kst::DataSourceConfigWidget *FitsTableSourcePlugin::configWidget(QSettings *cfg,
   const QString& filename) const {

   Q_UNUSED(cfg)
   Q_UNUSED(filename)
   return 0;
}


// vim: ts=2 sw=2 et
