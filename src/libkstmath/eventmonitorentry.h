/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2004 University of British Columbia                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef EVENTMONITORENTRY_H
#define EVENTMONITORENTRY_H

#include <qtimer.h>

#include "dataobject.h"
#include "debug.h"

namespace Equations {
  class Node;
  class Context;
}

namespace Kst {

class EventMonitorEntry : public DataObject {
  Q_OBJECT
  public:
    static const QString staticTypeTag;

    UpdateType update(int updateCounter = -1);
    void save(QXmlStreamWriter &s);
    QString propertyString() const;
    void showNewDialog();
    void showEditDialog();

    bool needToEvaluate();
    bool isValid() const { return _isValid; }

    void log(int idx);
    const QString& event() const { return _event; }
    const QString& description() const { return _description; }
    Debug::LogLevel level() const { return _level; }
    Equations::Node* expression() const { return _pExpression; }
    bool logKstDebug() const { return _logKstDebug; }
    bool logEMail() const { return _logEMail; }
    bool logELOG() const { return _logELOG; }
    const QString& eMailRecipients() const { return _eMailRecipients; }
    const QString& scriptCode() const;

    void setScriptCode(const QString& script);
    void setEvent(const QString& str);
    void setDescription(const QString& str);
    void setLevel(Debug::LogLevel level);
    void setExpression(Equations::Node* pExpression);
    void setLogKstDebug(bool logKstDebug);
    void setLogEMail(bool logEMail);
    void setLogELOG(bool logELOG);
    void setEMailRecipients(const QString& str);

    bool reparse();

    DataObjectPtr makeDuplicate(DataObjectDataObjectMap& duplicatedMap);

    void replaceDependency(DataObjectPtr oldObject, DataObjectPtr newObject);

    void replaceDependency(VectorPtr oldVector, VectorPtr newVector);
    void replaceDependency(MatrixPtr oldMatrix, MatrixPtr newMatrix);

    bool uses(ObjectPtr p) const;

  protected:
    EventMonitorEntry(ObjectStore *store, const ObjectTag &in_tag);
    EventMonitorEntry(ObjectStore *store, const ObjectTag &tag, const QString &script, const QString &event, const QString &description, const Debug::LogLevel level, const bool logKstDebug, const bool logEMail, const bool logELOG, const QString& emailRecipients);
    EventMonitorEntry(ObjectStore *store, const QDomElement &e);
    ~EventMonitorEntry();

    friend class ObjectStore;

    bool event(QEvent *e);

  private slots:
    void slotUpdate();
    void doLog(const QString& logMessage) const;

  private:
    void logImmediately(bool sendEvent = true);
    void commonConstructor(ObjectStore *store);

    static const QString OUTXVECTOR;
    static const QString OUTYVECTOR;

    VectorMap _vectorsUsed;
    QVector<int> _indexArray;
    QString _event;
    QString _description;
    QString _eMailRecipients;
    Debug::LogLevel _level;
    Equations::Node* _pExpression;
    VectorMap::Iterator _xVector;
    VectorMap::Iterator _yVector;
    bool _logKstDebug;
    bool _logEMail;
    bool _logELOG;
    bool _isValid;
    int _numDone;
    QString _script;
};

typedef SharedPtr<EventMonitorEntry> EventMonitorEntryPtr;
typedef ObjectList<EventMonitorEntryPtr> EventMonitorEntryList;

}

#endif
// vim: ts=2 sw=2 et
