/***************************************************************************
                     datasource.h  -  abstract data source
                             -------------------
    begin                : Thu Oct 16 2003
    copyright            : (C) 2003 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <qhash.h>
#include <qdom.h>
#include <qpointer.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qwidget.h>
#include <QRunnable>

#include <qsettings.h>

#include "kst_export.h"
#include "object.h"
#include "dateparser.h"
#include "objectlist.h"
//#include "scalar.h"
//#include "string_kst.h"

class QXmlStreamWriter;
class QXmlStreamAttributes;

namespace KST {
  class DataSourcePlugin;
}

class QFileSystemWatcher;

namespace Kst {

struct MatrixData {
  double xMin;
  double yMin;
  double xStepSize;
  double yStepSize;
  double *z; // the data
};

class String;
class Scalar;
class DataSourceConfigWidget;

class KST_EXPORT DataSource : public Object {
  Q_OBJECT

  public:
    enum UpdateCheckType { Timer, File, None };

    static void init();
    static void cleanupForExit();
    static void initPlugins();

    /** Returns a list of plugins found on the system. */
    static QStringList pluginList();

    static SharedPtr<DataSource> loadSource(ObjectStore *store, const QString& filename, const QString& type = QString::null);
    static SharedPtr<DataSource> loadSource(ObjectStore *store, QDomElement& e);
    static SharedPtr<DataSource> findOrLoadSource(ObjectStore *store, const QString& filename);
    static bool validSource(const QString& filename);

    static bool sourceHasConfigWidget(const QString& filename, const QString& type = QString());
    static DataSourceConfigWidget *configWidgetForSource(const QString& filename, const QString& type = QString());

    static bool pluginHasConfigWidget(const QString& plugin);
    static DataSourceConfigWidget *configWidgetForPlugin(const QString& plugin);

    DataSource(ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const UpdateCheckType updateType = File);
    virtual ~DataSource();

    virtual UpdateType objectUpdate(qint64 newSerial);

    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    bool hasConfigWidget() const;
    DataSourceConfigWidget *configWidget();
    virtual void parseProperties(QXmlStreamAttributes &properties);

    bool reusable() const;
    void disableReuse();

    virtual bool isValid() const; // generally you don't need to change this
    // returns _valid;

    // these are not used by datasources
    void internalUpdate() {return;}
    qint64 minInputSerial() const {return 0;}
    qint64 minInputSerialOfLastChange() const {return 0;}

   /** Updates number of samples.
      For ascii files, it also reads and writes to a temporary binary file.
      It must be implemented by the datasource. */

    virtual UpdateType internalDataSourceUpdate() = 0;

    virtual QString fileName() const;

    /** Returns the file type or an error message in a static string
      The string is stored in a separate static variable, so changes
      to this are ignored.  It is updated each time the fn is called */
    virtual QString fileType() const;

    void saveSource(QXmlStreamWriter &s);

    /** Save file description info into stream s. */
    virtual void save(QXmlStreamWriter &s);

    const QString& sourceName() const { return _source; }

    // These malloc calls do not appear to be used.
    virtual void *bufferMalloc(size_t size);
    virtual void bufferFree(void *ptr);
    virtual void *bufferRealloc(void *ptr, size_t size);

    /** Returns true if this file is empty */
    virtual bool isEmpty() const;

    /** Reset to initial state of the source, just as though no data had been
     *  read and the file had just been opened.  Return true on success.
     */
    virtual void reset();

    virtual void deleteDependents();

    virtual QString descriptionTip() const;

    /************************************************************/
    /* Methods for writing                                      */
    /* only used by d2d - may be reworked (remove this note     */
    /* if you use it)                                           */
    /************************************************************/
    virtual bool isWritable() const;

    /** Write a field to the file.
     * Data is in the double Array v[]
     * s is the starting frame
     * n is the number of frames to write
     *
     * return the number of -samples- written or -1 if writing fails or is not supported.
     */
    virtual int writeField(const double *v, const QString& field, int s, int n);
    /** Returns the file name. It is updated each time the fn is called. */

    /************************************************************/
    /* Methods for handling time in vectors.                    */
    /* not currently used - may be reworked (remove this note   */
    /* if you use it)                                           */
    /************************************************************/
    static bool supportsTime(const QString& plugin, const QString& type = QString::null);

    /** Does it support time conversion of sample numbers, in general? */
    virtual bool supportsTimeConversions() const;

    virtual int sampleForTime(const QDateTime& time, bool *ok = 0L);

    virtual int sampleForTime(double milliseconds, bool *ok = 0L);

    virtual QDateTime timeForSample(int sample, bool *ok = 0L);

    // in (ms)
    virtual double relativeTimeForSample(int sample, bool *ok = 0L);

    /************************************************************/
    /* Methods for Vectors (which, for historical reasons, are  */
    /* referred to as fields in data sources                    */
    /************************************************************/
    static QStringList fieldListForSource(const QString& filename, const QString& type = QString(), QString *outType = 0L, bool *complete = 0L);

    /** Returns true if the field list is complete, therefore the user should
      not be able to edit the field combobox.  Default is true. */
    virtual bool fieldListIsComplete() const;

    virtual QStringList fieldList() const;

    /** Reads a field from the file.  Data is returned in the
      double Array v[]
      s is the starting frame
      n is the number of frames to read
      if n is -1, it means to read 1 -sample- from frame s.
      return the number of -samples- read.
     */
    virtual int readField(double *v, const QString& field, int s, int n);

    /** Reads a field from the file.  Data is returned in the
      double Array v[].  Will skip according to the parameter, but it may not
      be implemented.  If it returns -9999, use the non-skip version instead. */
    virtual int readField(double *v, const QString& field, int s, int n, int skip, int *lastFrameRead = 0L);

    /** Returns true if the field is valid, or false if it is not */
    virtual bool isValidField(const QString& field) const;

    /** Returns samples per frame for field <field>.  For ascii column data,
      this is always 1.  For frame data this could greater than 1. */
    virtual int samplesPerFrame(const QString& field);

    /** Returns the size of the file (in frames) as of last update.
      Field is optional, but you might not get back what you expect if you
      don't provide it as the data source may have different frame counts
      for different fields.  When implementing this, field may be ignored. */
    virtual int frameCount(const QString& field = QString::null) const;

    /** Returns a list of scalars associated with a field by the data source.
        These could be sample rate, calibrations, etc.  This list must be
        complete the very first time it is called and must never change its
        order or size, because readFieldScalars counts on its order and size. 
        */
    virtual QStringList fieldScalars(const QString& field);

    /** Read the values of the field scalars.  This is called for every field scalar
        every time the vector is updated, so it needs to be kept very cheap.
        Returns the number of scalars returned, and 0 on failure. V must
        be pre allocated to the right size as given by the length of the list
        returned by fieldScalars(). 
        Most data sources will never change the the field scalars once they have 
        been initialized.  In order to keep this case as fast as possible, the data 
        source can chose to only update v[] if init is true.  
        Note: datavector currently assumes these never change, and only gets them once! */ 
    virtual int readFieldScalars(QList<double> &v, const QString& field, bool init);

    /** Returns a list of strings associated with a field by the data source.
        These could be units, notes, etc.  This list must be
        complete the very first time it is called and must never change its
        order or size, because readFieldScalars counts on its order and size. 
        In order to remain fast, the data source can additionally assume that
        readField() has already been called on the field, so the updating
        of string values can be done there instead and cached for read by this
        call.  */
    virtual QStringList fieldStrings(const QString& field);

    /** Read the values of the field strings.  This is called 
        every time the vector is updated, so it needs to be kept very cheap.
        Returns the number of strings returned, and 0 on failure. 
        Most data sources will never change the the field strings once they have 
        been initialized.  In order to keep this case as fast as possible, the data 
        source can chose to only update v if init is true.  */ 
    virtual int readFieldStrings(QStringList &v, const QString& field, bool init);

    /************************************************************/
    /* Methods for Matrixes                                     */
    /************************************************************/
    //static QStringList matrixListForSource(const QString& filename, const QString& type = QString(), QString *outType = 0L, bool *complete = 0L);

    /** Returns the list of fields that support readMatrix **/
    virtual QStringList matrixList() const;

    /** Read the specified sub-range of the matrix, flat-packed in z in row-major order
        xStart - starting x *frame*
        yStart - starting y *frame*
        xNumSteps - number of *frames* to read in x direction; -1 to read 1 *sample* from xStart
        yNumSteps - number of *frames* to read in y direction; -1 to read 1 *sample* from yStart
        Will skip according to the parameter, but it may not be implemented.  If return value is -9999, 
        use the non-skip version instead.
        The suggested scaling and translation is returned in xMin, yMin, xStepSize, and yStepSize
        Returns the number of *samples* read **/
    virtual int readMatrix(MatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps, int skip);

    /** Read the specified sub-range of the matrix, flat-packed in z in row-major order (non-skipping)
        xStart - starting x *frame*
        yStart - starting y *frame*
        xNumSteps - number of *frames* to read in x direction; -1 to read 1 *sample* from xStart
        yNumSteps - number of *frames* to read in y direction; -1 to read 1 *sample* from yStart
        The suggested scaling and translation is returned in xMin, yMin, xStepSize, and yStepSize
        Returns the number of *samples* read **/
    virtual int readMatrix(MatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps);

    /** Returns true if the matrix is valid, or false if it is not */
    virtual bool isValidMatrix(const QString& field) const;

    /** Return the current dimensions of the matrix: xDim*yDim <= total frames **/
    virtual bool matrixDimensions(const QString& matrix, int* xDim, int* yDim);

    /************************************************************/
    /* Methods for Scalars                                      */
    /* see also fieldScalars                                    */
    /************************************************************/
    static QStringList scalarListForSource(const QString& filename, const QString& type = QString(), QString *outType = 0L, bool *complete = 0L);

    /** Returns true if the scalar list is complete, therefore the user should
        not be able to edit the scalar combobox.  Default is true. */
    virtual bool scalarListIsComplete() const;

    /** Returns the list of fields that support readScalar **/
    virtual QStringList scalarList() const;

    /** Read the specified scalar **/
    virtual int readScalar(double &S, const QString& scalar);

    /** Returns true if the scalar is valid, or false if it is not */
    virtual bool isValidScalar(const QString& field) const;

    /************************************************************/
    /* Methods for Strings                                      */
    /************************************************************/
    static QStringList stringListForSource(const QString& filename, const QString& type = QString(), QString *outType = 0L, bool *complete = 0L);

    /** Returns true if the string list is complete, therefore the user should
        not be able to edit the scalar combobox.  Default is true. */
    virtual bool stringListIsComplete() const;

    /** Returns the list of fields that support readString **/
    virtual QStringList stringList() const;

    /** Read the specified string **/
    virtual int readString(QString &S, const QString& string);

    /** Returns true if the string is valid, or false if it is not */
    virtual bool isValidString(const QString& field) const;


  public Q_SLOTS:
    virtual void checkUpdate();

  Q_SIGNALS:
    void sourceUpdated(ObjectPtr sourceObject);

  protected:

    /** Is the object valid? */
    bool _valid;

    bool _reusable;

    bool _writable;

    /** Place to store the list of fields.  Base implementation returns this. */
    QStringList _fieldList;

    /** Place to store the list of matrices.  Base implementation returns this. */
    QStringList _matrixList;

    /** Place to store the list of scalars.  Base implementation returns this. */
    QStringList _scalarList;

    /** Place to store the list of strings.  Base implementation returns this. */
    QStringList _stringList;

    /** The filename.  Populated by the base class constructor.  */
    QString _filename;

    friend class DataSourcePlugin;

    /** The source type name. */
    QString _source;

    QSettings *_cfg;

    UpdateCheckType _updateCheckType;

    virtual QString _automaticDescriptiveName() const;
    void _initializeShortName();

    QFileSystemWatcher *_watcher;

    // NOTE: You must bump the version key if you add new member variables
    //       or change or add virtual functions.
};


typedef SharedPtr<DataSource> DataSourcePtr;

class DataSourceList : public QList<DataSourcePtr> {
  public:
    DataSourceList() : QList<DataSourcePtr>() {}
    DataSourceList(const DataSourceList& x) : QList<DataSourcePtr>(x) {}
    virtual ~DataSourceList() {}

    virtual DataSourcePtr findName(const QString name) {
      for (DataSourceList::Iterator it = begin(); it != end(); ++it) {
        if ((*it)->Name() == name) {
          return *it;
        }
      }
      return 0;
    }

    virtual DataSourcePtr findFileName(const QString& x) {
      for (DataSourceList::Iterator it = begin(); it != end(); ++it) {
        if ((*it)->fileName() == x) {
          return *it;
        }
      }
      return 0;
    }

    // @since 1.1.0
    DataSourcePtr findReusableFileName(const QString& x) {
      for (DataSourceList::Iterator it = begin(); it != end(); ++it) {
        if ((*it)->reusable() && (*it)->fileName() == x) {
          return *it;
        }
      }
      return 0;
    }

    // @since 1.1.0
    QStringList fileNames() const {
      QStringList rc;
      for (DataSourceList::ConstIterator it = begin(); it != end(); ++it) {
        rc << (*it)->fileName();
      }
      return rc;
    }

};


// @since 1.1.0
class DataSourceConfigWidget : public QWidget {
  Q_OBJECT
  friend class DataSource;
  public:
    DataSourceConfigWidget(); // will be reparented later
    virtual ~DataSourceConfigWidget();

    virtual void setConfig(QSettings*);

    KST_EXPORT void setInstance(DataSourcePtr inst);
    KST_EXPORT DataSourcePtr instance() const;

  public slots:
    virtual void load();
    virtual void save();

  protected:
    QSettings *_cfg;
    // If _instance is nonzero, then your settings are to be saved for this
    // particular instance of the source, as opposed to globally.
    DataSourcePtr _instance;
} KST_EXPORT;

class KST_EXPORT ValidateDataSourceThread : public QObject, public QRunnable
{
  Q_OBJECT

  public:
    ValidateDataSourceThread(const QString& file, const int requestID);
    void run();

  Q_SIGNALS:
    void dataSourceValid(QString filename, int requestID);
    void dataSourceInvalid(int requestID);

  private:
    ObjectStore *_store;
    QString _file;
    int _requestID;
};

}
#endif
// vim: ts=2 sw=2 et
