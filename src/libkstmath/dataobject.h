/***************************************************************************
                   dataobject.h: base class for data objects
                             -------------------
    begin                : May 20, 2003
    copyright            : (C) 2003 by C. Barth Netterfield
                           (C) 2003 The University of Toronto
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

#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include "curvehint.h"
#include "string_kst.h"
#include "object.h"
#include "objectlist.h"
#include "vector.h"
#include "matrix.h"
#include "kst_export.h"

#include <QWidget>
#include <QSettings>

class QXmlStreamReader;
class QXmlStreamWriter;

namespace Kst {

class DataObject;
class Relation;
class DataObjectConfigWidget;

typedef SharedPtr<DataObject> DataObjectPtr;
typedef ObjectList<DataObject> DataObjectList;
typedef QMap<DataObjectPtr, DataObjectPtr> DataObjectDataObjectMap;

class KST_EXPORT DataObject : public Object {
  Q_OBJECT

  public:
    enum Kind { Generic, Primitive, Fit, Filter };

    virtual void attach();

    // These static methods are not for plugins to use
    static void setupOnStartup(QSettings*);
    static void cleanupForExit();
    /** Returns a list of object plugins found on the system. */
    static QStringList pluginList();

    //     static QWidget* pluginWidget(const QString& name, ObjectPtr objectPtr = 0, VectorPtr vector = 0);
    static DataObjectPtr createPlugin(const QString& name, ObjectStore *store, VectorPtr vector);
    static DataObjectConfigWidget* pluginWidget(const QString& name);
    static DataObjectPtr createPlugin(const QString& name, ObjectStore *store, DataObjectConfigWidget *configWidget, bool setupInputsOutputs = true);

    virtual UpdateType update() = 0;
    virtual const QString& typeString() const { return _typeString; }
    virtual QString propertyString() const = 0;
    virtual const QString& type() const { return _type; }
    virtual Kind kind() const { return Generic; }

    virtual int sampleCount() const { return 0; }

    // If you use these, you must lock() and unlock() the object as long as you
    // hold the reference
    const VectorMap& inputVectors()  const { return _inputVectors;  }
    const VectorMap& outputVectors() const { return _outputVectors; }
    VectorMap& inputVectors() { return _inputVectors;  }
    VectorMap& outputVectors() { return _outputVectors; }

    const ScalarMap& inputScalars()  const { return _inputScalars;  }
    const ScalarMap& outputScalars() const { return _outputScalars; }
    ScalarMap& inputScalars() { return _inputScalars;  }
    ScalarMap& outputScalars() { return _outputScalars; }

    const StringMap& inputStrings()  const { return _inputStrings;  }
    const StringMap& outputStrings() const { return _outputStrings; }
    StringMap& inputStrings() { return _inputStrings;  }
    StringMap& outputStrings() { return _outputStrings; }

    const MatrixMap& inputMatrices() const { return _inputMatrices; }
    const MatrixMap& outputMatrices() const { return _outputMatrices; }
    MatrixMap& inputMatrices() { return _inputMatrices; }
    MatrixMap& outputMatrices() { return _outputMatrices; }

    virtual void load(const QXmlStreamReader& s);
    virtual void save(QXmlStreamWriter& s);

    virtual bool loadInputs();

    virtual int getUsage() const;

    virtual void readLock() const;
    virtual void writeLock() const;
    virtual void unlock() const;

    virtual bool isValid() const;

    virtual const CurveHintList* curveHints() const;

    virtual void deleteDependents();

    bool duplicateDependents(DataObjectPtr newObject, QMap< SharedPtr<Relation>, SharedPtr<Relation> > &duplicatedRelations);

    virtual DataObjectPtr makeDuplicate() = 0;

    virtual void replaceDependency(DataObjectPtr oldObject, DataObjectPtr newObject);
    virtual void replaceDependency(VectorPtr oldVector, VectorPtr newVector);
    virtual void replaceDependency(MatrixPtr oldMatrix, MatrixPtr newMatrix);

    virtual bool uses(ObjectPtr p) const;

    //These are generally only valid for plugins...
    const QString& name() const { return _name; }
    const QString& author() const { return _author; }
    const QString& description() const { return _description; }
    const QString& version() const { return _version; }
    const QString& library() const { return _library; }

    void showDialog(bool isNew = true);

  public Q_SLOTS:
    void inputObjectUpdated(ObjectPtr object);

  protected slots:
    virtual void showNewDialog() = 0;
    virtual void showEditDialog() = 0;

  protected:
    DataObject(ObjectStore *store);
    virtual ~DataObject();

    double *vectorRealloced(VectorPtr v, double *memptr, int newSize) const;

    //The plugin infrastructure will read the desktop file and set these
    //Other objects that inherit can set the ones that apply if desired...
    void setName(const QString& str) { _name = str; }
    void setAuthor(const QString& str) { _author = str; }
    void setDescription(const QString& str) { _description = str; }
    void setVersion(const QString& str) { _version = str; }
    void setLibrary(const QString& str) { _library = str; }

    virtual void writeLockInputsAndOutputs() const;
    virtual void unlockInputsAndOutputs() const;

    VectorMap _inputVectors;
    VectorMap _outputVectors;
    ScalarMap _inputScalars;
    ScalarMap _outputScalars;
    StringMap _inputStrings;
    StringMap _outputStrings;
    MatrixMap _inputMatrices;
    MatrixMap _outputMatrices;

    QString _typeString, _type;

    bool _isInputLoaded;
    QList<QPair<QString,QString> > _inputVectorLoadQueue;
    QList<QPair<QString,QString> > _inputScalarLoadQueue;
    QList<QPair<QString,QString> > _inputStringLoadQueue;
    QList<QPair<QString,QString> > _inputMatrixLoadQueue;
    CurveHintList *_curveHints;

  private:
    QString _name;
    QString _author;
    QString _description;
    QString _version;
    QString _library;

  private:
    static void scanPlugins();
};


class DataObjectConfigWidget : public QWidget {
  Q_OBJECT
  friend class DataObject;
  public:
    DataObjectConfigWidget(QSettings*); // will be reparented later
    virtual ~DataObjectConfigWidget();

    virtual void setupFromObject(Object* dataObject);
    virtual void setupSlots(QWidget* dialog);

    virtual bool configurePropertiesFromXml(ObjectStore *store, QXmlStreamAttributes& attrs);

    KST_EXPORT void setInstance(DataObjectPtr inst);
    KST_EXPORT DataObjectPtr instance() const;

    virtual void setObjectStore(ObjectStore* store);

  public slots:
    virtual void load();
    virtual void save();

  protected:
    QSettings *_cfg;
} KST_EXPORT;

}

Q_DECLARE_METATYPE(Kst::DataObject*)

#endif
// vim: ts=2 sw=2 et
