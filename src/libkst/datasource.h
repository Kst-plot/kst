/***************************************************************************
                     datasource.h  -  abstract data source
                             -------------------
    begin                : Thu Oct 16 2003
    copyright            : (C) 2003 The University of Toronto
    email                : netterfield@astro.utoronto.ca
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

#include "kst_export.h"
#include "object.h"
#include "dateparser.h"
#include "objectlist.h"

#include "datascalar.h"
#include "datastring.h"
#include "datavector.h"
#include "datamatrix.h"

#include <QRunnable>
#include <QDialog>
#include <QMap>

class QSettings;
class QXmlStreamWriter;
class QXmlStreamAttributes;
class QFileSystemWatcher;

namespace Kst {

class DataSourceConfigWidget;
//class DataSourcePlugin;


class KSTCORE_EXPORT DataSource : public Object
{
  Q_OBJECT

  public:
    DataSource(ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type);
    virtual ~DataSource();

    /************************************************************/
    /* Same interface for all supported Primitives              */
    /************************************************************/

    template<class T>
    struct DataInterface
    {

      virtual ~DataInterface() {}
      // read data.  The buffer and range info are in ReadInfo
      virtual int read(const QString& name, typename T::ReadInfo&) = 0;

      // named elements
      virtual QStringList list() const = 0;
      virtual bool isListComplete() const = 0;
      virtual bool isValid(const QString& name) const = 0;

      // T specific
      virtual const typename T::DataInfo dataInfo(const QString& name) const = 0;
      virtual void setDataInfo(const QString& name, const typename T::DataInfo&) = 0;

      // meta data
      virtual QMap<QString, double> metaScalars(const QString& name) = 0;
      virtual QMap<QString, QString> metaStrings(const QString& name) = 0;
    };


    DataInterface<DataScalar>& scalar() {Q_ASSERT(interf_scalar); return *interf_scalar; }
    DataInterface<DataString>& string() {Q_ASSERT(interf_string); return *interf_string; }
    DataInterface<DataVector>& vector() {Q_ASSERT(interf_vector); return *interf_vector; }
    DataInterface<DataMatrix>& matrix() {Q_ASSERT(interf_matrix); return *interf_matrix; }

    const DataInterface<DataScalar>& scalar() const {Q_ASSERT(interf_scalar); return *interf_scalar; }
    const DataInterface<DataString>& string() const {Q_ASSERT(interf_string); return *interf_string; }
    const DataInterface<DataVector>& vector() const {Q_ASSERT(interf_vector); return *interf_vector; }
    const DataInterface<DataMatrix>& matrix() const {Q_ASSERT(interf_matrix); return *interf_matrix; }



    /************************************************************/
    /* Dynamic type system                                      */
    /************************************************************/
    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;


    /************************************************************/
    /* Methods for update system                                */
    /************************************************************/

    enum UpdateCheckType { Timer, File, None };
    void setUpdateType(UpdateCheckType updateType, const QString& file = QString());
    UpdateCheckType updateType() const;

    virtual UpdateType objectUpdate(qint64 newSerial);

    void internalUpdate() {return;}
    qint64 minInputSerial() const {return 0;}
    qint64 maxInputSerialOfLastChange() const {return 0;}

   /** Updates number of samples.
      For ascii files, it also reads and writes to a temporary binary file.
      It must be implemented by the datasource. */
    virtual UpdateType internalDataSourceUpdate() = 0;


    /************************************************************/
    /* Methods for handling time in vectors.                    */
    /* not currently used - may be reworked (remove this note   */
    /* if you use it)                                           */
    /************************************************************/
    static bool supportsTime(const QString& plugin, const QString& type = QString());
    virtual QString timeFormat() const; // Used by the data wizard to set X axis display format

    /** Does it support time conversion of sample numbers, in general? */
    virtual bool supportsTimeConversions() const;

    virtual int sampleForTime(const QDateTime& time, bool *ok = 0L);

    virtual int sampleForTime(double milliseconds, bool *ok = 0L);

    virtual QDateTime timeForSample(int sample, bool *ok = 0L);

    // in (ms)
    virtual double relativeTimeForSample(int sample, bool *ok = 0L);

    /************************************************************/
    /* Methods for handling time in vectors.                    */
    /************************************************************/
    virtual bool isTime(const QString &field) const; // now used by ascii


    /************************************************************/
    /* Methods for using custom lookup vectors, like TIME.      */
    /* "Index" refers to custom lookup vector.                  */
    /* For all of these, a default implementation is provided   */
    /* in the base class.                                       */
    /************************************************************/
    /** returns the frame number corresponding to an index value from a frame */
    virtual int indexToFrame(double index, const QString &field);
    virtual double frameToIndex(int frame, const QString &field);
    virtual double readDespikedIndex(int frame, const QString &field);
    virtual double framePerIndex(const QString &field);
    virtual QStringList &timeFields();
    virtual QStringList &indexFields();


    /************************************************************/
    /* UI TODO leave here?                                      */
    /************************************************************/
    bool hasConfigWidget() const;
    DataSourceConfigWidget *configWidget();
    virtual void parseProperties(QXmlStreamAttributes &properties);

    bool reusable() const;
    void disableReuse();

    /************************************************************/
    /* Color for the "assign curve color per file" tool         */
    /************************************************************/
    QColor color() const;
    void setColor(const QColor& color);

    /************************************************************/
    /* File/data specific                                      */
    /************************************************************/


    virtual bool isValid() const; // generally you don't need to change this

    virtual QString fileName() const;
    QString alternateFilename() const;
    void setAlternateFilename(const QString &file);

    QMap<QString, QString> fileMetas() const;

    /** Returns the file type or an error message in a static string
      The string is stored in a separate static variable, so changes
      to this are ignored.  It is updated each time the fn is called */
    virtual QString fileType() const;

    void saveSource(QXmlStreamWriter &s);

    /** Save file description info into stream s. */
    virtual void save(QXmlStreamWriter &s);

    const QString& sourceName() const { return _source; }

    /** Returns true if this file is empty */
    virtual bool isEmpty() const;

    /** Reset to initial state of the source, just as though no data had been
     *  read and the file had just been opened.
     */
    virtual void reset();

    virtual void deleteDependents();

    virtual QString descriptionTip() const;

    /** Creates a list of curves without user interaction
    */
    virtual ObjectList<Object> autoCurves(ObjectStore&) { return ObjectList<Object>(); }


  public Q_SLOTS:
    virtual void checkUpdate();

  Q_SIGNALS:
    void sourceUpdated(ObjectPtr sourceObject);


  protected:

    /** Is the object valid? */
    bool _valid;

    bool _reusable;

    bool _writable;



    /** The filename.  Populated by the base class constructor.  */
    QString _filename;

    /** an alias for the file: for example if the file were replaced at load time */
    QString _alternateFilename;

    //friend class DataSourcePlugin;

    /** The source type name. */
    QString _source;

    QSettings *_cfg;

    UpdateCheckType _updateCheckType;
    void resetFileWatcher();

    virtual QString _automaticDescriptiveName() const;
    void _initializeShortName();

    void setInterface(DataInterface<DataScalar>*);
    void setInterface(DataInterface<DataString>*);
    void setInterface(DataInterface<DataVector>*);
    void setInterface(DataInterface<DataMatrix>*);

    QStringList _frameFields;
    QStringList _timeFields;
  private:
    DataSource();

    DataInterface<DataScalar>* interf_scalar;
    DataInterface<DataString>* interf_string;
    DataInterface<DataVector>* interf_vector;
    DataInterface<DataMatrix>* interf_matrix;

    QFileSystemWatcher *_watcher;

    QColor _color;
    // NOTE: You must bump the version key if you add new member variables
    //       or change or add virtual functions.
};




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
        if ((*it)->reusable()) {
          if ((*it)->fileName() == x) {
            return *it;
          } else if ((*it)->alternateFilename() == x) {
            return *it;
          }
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

#if QT_VERSION < 0x040500
    void append(const DataSourcePtr& ptr) {
      QList<DataSourcePtr>::append(ptr);
    }
    void append(const DataSourceList& list) {
      foreach(const DataSourcePtr& ptr, list) {
        QList<DataSourcePtr>::append(ptr);
      }
    }
#endif

};


// @since 1.1.0
class KSTCORE_EXPORT DataSourceConfigWidget : public QWidget
{
  Q_OBJECT

  public:
    DataSourceConfigWidget(QSettings&); // will be reparented later
    virtual ~DataSourceConfigWidget();

    QSettings& settings() const;

    // If _instance is nonzero, then your settings are to be saved for this
    // particular instance of the source, as opposed to globally.
    void setInstance(DataSourcePtr inst);
    DataSourcePtr instance() const;
    bool hasInstance() const;

    // Check if the widget could be closed,
    // and the user has not entered invalid parameters.
    virtual bool isOkAcceptabe() const;

    virtual void setDialogParent(QDialog* parent) { setParent(parent); }

  public slots:
    virtual void load() = 0;
    virtual void save() = 0;
    virtual void cancel() {return;}

  private:
    DataSourcePtr _instance;
    QSettings& _cfg;
    friend class DataSource;
};


class KSTCORE_EXPORT ValidateDataSourceThread : public QObject, public QRunnable
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
