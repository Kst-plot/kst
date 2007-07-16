/***************************************************************************
                   kstdataobject.h: base class for data objects
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

#ifndef KSTDATAOBJECT_H
#define KSTDATAOBJECT_H

#include "kstcurvehint.h"
#include "kststring.h"
#include "kstvector.h"
#include "kstmatrix.h"
#include "kst_export.h"

typedef KstSharedPtr<KstDataObject> KstDataObjectPtr;
typedef KstObjectList<KstDataObjectPtr> KstDataObjectList;
typedef QMap<KstDataObjectPtr, KstDataObjectPtr> KstDataObjectDataObjectMap;
typedef QMap<QString, int> KstPluginInfoList;

class QXmlStreamReader;
class QXmlStreamWriter;

class KST_EXPORT KstDataObject : public KstObject {
  Q_OBJECT

  public:
    enum Kind { Generic, Primitive, Fit, Filter };

    KstDataObject();
    KstDataObject(const QDomElement& e);
    KstDataObject(const KstDataObject& object);
    virtual ~KstDataObject();

    virtual void attach();

    // These static methods are not for plugins to use
    static void cleanupForExit();
    /** Returns a list of object plugins found on the system. */
    static KstPluginInfoList pluginInfoList();
    static KstDataObjectPtr plugin(const QString& name);
    static KstDataObjectPtr createPlugin(const QString& name);

    virtual UpdateType update(int updateCounter = -1) = 0;
    virtual const QString& typeString() const { return _typeString; }
    virtual QString propertyString() const = 0;
    virtual const QString& type() const { return _type; }
    virtual Kind kind() const { return Generic; }

    virtual int sampleCount() const { return 0; }

    // If you use these, you must lock() and unlock() the object as long as you
    // hold the reference
    const KstVectorMap& inputVectors()  const { return _inputVectors;  }
    const KstVectorMap& outputVectors() const { return _outputVectors; }
    KstVectorMap& inputVectors() { return _inputVectors;  }
    KstVectorMap& outputVectors() { return _outputVectors; }

    const KstScalarMap& inputScalars()  const { return _inputScalars;  }
    const KstScalarMap& outputScalars() const { return _outputScalars; }
    KstScalarMap& inputScalars() { return _inputScalars;  }
    KstScalarMap& outputScalars() { return _outputScalars; }

    const KstStringMap& inputStrings()  const { return _inputStrings;  }
    const KstStringMap& outputStrings() const { return _outputStrings; }
    KstStringMap& inputStrings() { return _inputStrings;  }
    KstStringMap& outputStrings() { return _outputStrings; }
    
    const KstMatrixMap& inputMatrices() const { return _inputMatrices; }
    const KstMatrixMap& outputMatrices() const { return _outputMatrices; }
    KstMatrixMap& inputMatrices() { return _inputMatrices; }
    KstMatrixMap& outputMatrices() { return _outputMatrices; }

    virtual void load(const QXmlStreamReader& s);
    virtual void save(QXmlStreamWriter& s);

    virtual bool loadInputs();

    virtual int getUsage() const;

    virtual void readLock() const;
    virtual void writeLock() const;
    virtual void unlock() const;

    virtual bool isValid() const;

    virtual const KstCurveHintList* curveHints() const;
    
    virtual bool deleteDependents();
    
    bool duplicateDependents(KstDataObjectDataObjectMap& duplicatedMap);
    
    virtual KstDataObjectPtr makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap) = 0;
    
    virtual void replaceDependency(KstDataObjectPtr oldObject, KstDataObjectPtr newObject);
    virtual void replaceDependency(KstVectorPtr oldVector, KstVectorPtr newVector);
    virtual void replaceDependency(KstMatrixPtr oldMatrix, KstMatrixPtr newMatrix);

    virtual bool uses(KstObjectPtr p) const;

    //These are generally only valid for plugins...
    const QString& name() const { return _name; }
    const QString& author() const { return _author; }
    const QString& description() const { return _description; }
    const QString& version() const { return _version; }
    const QString& library() const { return _library; }

    void showDialog(bool isNew = true);

  protected slots:
    virtual void showNewDialog() = 0;
    virtual void showEditDialog() = 0;

  protected:
    
    double *vectorRealloced(KstVectorPtr v, double *memptr, int newSize) const;

    //The plugin infrastructure will read the desktop file and set these
    //Other objects that inherit can set the ones that apply if desired...
    void setName(const QString& str) { _name = str; }
    void setAuthor(const QString& str) { _author = str; }
    void setDescription(const QString& str) { _description = str; }
    void setVersion(const QString& str) { _version = str; }
    void setLibrary(const QString& str) { _library = str; }

    virtual void writeLockInputsAndOutputs() const;
    virtual void unlockInputsAndOutputs() const;

    KstVectorMap _inputVectors;
    KstVectorMap _outputVectors;
    KstScalarMap _inputScalars;
    KstScalarMap _outputScalars;
    KstStringMap _inputStrings;
    KstStringMap _outputStrings;
    KstMatrixMap _inputMatrices;
    KstMatrixMap _outputMatrices;

    QString _typeString, _type;

    bool _isInputLoaded;
    QList<QPair<QString,QString> > _inputVectorLoadQueue;
    QList<QPair<QString,QString> > _inputScalarLoadQueue;
    QList<QPair<QString,QString> > _inputStringLoadQueue;
    QList<QPair<QString,QString> > _inputMatrixLoadQueue;
    KstCurveHintList *_curveHints;

  private:
    QString _name;
    QString _author;
    QString _description;
    QString _version;
    QString _library;

  private:
    static void scanPlugins();
    static KstDataObjectPtr createPlugin();
};


#endif
// vim: ts=2 sw=2 et
