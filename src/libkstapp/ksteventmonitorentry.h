/***************************************************************************
                          ksteventmonitorentry.h  -  description
                             -------------------
    begin                : Tue Apr 6 2004
    copyright            : (C) 2000 by The University of British Columbia
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

#ifndef KSTEVENTMONITORENTRY_H
#define KSTEVENTMONITORENTRY_H

#include <qtimer.h>

#include "kstdataobject.h"
#include "kstdebug.h"

namespace Equation {
  class Node;
  class Context;
}

class EventMonitorEntry : public KstDataObject {
  Q_OBJECT
  public:
    EventMonitorEntry(const QString &in_tag);
    EventMonitorEntry(const QDomElement &e);
    ~EventMonitorEntry();

    UpdateType update(int updateCounter = -1);
    void save(QTextStream &ts, const QString& indent = QString::null);
    QString propertyString() const;
    void showNewDialog();
    void showEditDialog();

    bool needToEvaluate();
    bool isValid() const { return _isValid; }

    void log(int idx);
    const QString& event() const { return _event; }
    const QString& description() const { return _description; }
    KstDebug::LogLevel level() const { return _level; }
    Equation::Node* expression() const { return _pExpression; }
    bool logKstDebug() const { return _logKstDebug; }
    bool logEMail() const { return _logEMail; }
    bool logELOG() const { return _logELOG; }
    const QString& eMailRecipients() const { return _eMailRecipients; }
    const QString& scriptCode() const;

    void setScriptCode(const QString& script);
    void setEvent(const QString& str);
    void setDescription(const QString& str);
    void setLevel(KstDebug::LogLevel level);
    void setExpression(Equation::Node* pExpression);
    void setLogKstDebug(bool logKstDebug);
    void setLogEMail(bool logEMail);
    void setLogELOG(bool logELOG);
    void setEMailRecipients(const QString& str);

    bool reparse();
    
    KstDataObjectPtr makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap);
    
    void replaceDependency(KstDataObjectPtr oldObject, KstDataObjectPtr newObject);

    void replaceDependency(KstVectorPtr oldVector, KstVectorPtr newVector);
    void replaceDependency(KstMatrixPtr oldMatrix, KstMatrixPtr newMatrix);

    bool uses(KstObjectPtr p) const;

  protected:
    bool event(QEvent *e);

  private slots:
    void slotUpdate();
    void doLog(const QString& logMessage) const;

  private:
    void logImmediately(bool sendEvent = true);
    void commonConstructor(const QString &in_tag);

    static const QString OUTXVECTOR;
    static const QString OUTYVECTOR;

    KstVectorMap _vectorsUsed;
    QValueList<int> _indexArray;
    QString _event;
    QString _description;
    QString _eMailRecipients;
    KstDebug::LogLevel _level;
    Equation::Node* _pExpression;
    KstVectorMap::Iterator _xVector;
    KstVectorMap::Iterator _yVector;
    bool _logKstDebug;
    bool _logEMail;
    bool _logELOG;
    bool _isValid;
    int _numDone;
    QString _script;
};

typedef KstSharedPtr<EventMonitorEntry> EventMonitorEntryPtr;
typedef KstObjectList<EventMonitorEntryPtr> KstEventMonitorEntryList;

#endif
// vim: ts=2 sw=2 et
