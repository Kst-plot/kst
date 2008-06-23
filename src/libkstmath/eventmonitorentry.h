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
    static const QString staticTypeString;
    const QString& typeString() const { return staticTypeString; }
    static const QString staticTypeTag;

    UpdateType update();
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
    bool logDebug() const { return _logDebug; }
    bool logEMail() const { return _logEMail; }
    bool logELOG() const { return _logELOG; }
    const QString& eMailRecipients() const { return _eMailRecipients; }
    const QString& scriptCode() const;

    void setScriptCode(const QString& script);
    void setEvent(const QString& str);
    void setDescription(const QString& str);
    void setLevel(Debug::LogLevel level);
    void setExpression(Equations::Node* pExpression);
    void setLogDebug(bool logDebug);
    void setLogEMail(bool logEMail);
    void setLogELOG(bool logELOG);
    void setEMailRecipients(const QString& str);

    bool reparse();

    DataObjectPtr makeDuplicate();

    void replaceDependency(DataObjectPtr oldObject, DataObjectPtr newObject);

    void replaceDependency(VectorPtr oldVector, VectorPtr newVector);
    void replaceDependency(MatrixPtr oldMatrix, MatrixPtr newMatrix);

    bool uses(ObjectPtr p) const;

  protected:
    EventMonitorEntry(ObjectStore *store);
    ~EventMonitorEntry();

    friend class ObjectStore;

    QString _automaticDescriptiveName() const;

    bool event(QEvent *e);

  private slots:
    void slotUpdate();
    void doLog(const QString& logMessage) const;

  private:
    void logImmediately(bool sendEvent = true);

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
    bool _logDebug;
    bool _logEMail;
    bool _logELOG;
    bool _isValid;
    int _numDone;
    QString _script;
};

typedef SharedPtr<EventMonitorEntry> EventMonitorEntryPtr;
typedef ObjectList<EventMonitorEntry> EventMonitorEntryList;

}

#endif
// vim: ts=2 sw=2 et
