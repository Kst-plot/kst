/***************************************************************************
                               js_dataobject.h
                             -------------------
    begin                : Apr 23 2005
    copyright            : (C) 2005 The University of Toronto
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

#ifndef JS_DATAOBJECT_H
#define JS_DATAOBJECT_H

#include <kstdataobject.h>

#include <qguardedptr.h>

class JSDataObject : public KstDataObject {
  public:
    JSDataObject();
    JSDataObject(const QDomElement &e);
    ~JSDataObject();

    UpdateType update(int update_counter);

    void save(QTextStream &ts, const QString& indent = QString::null);

    bool slaveVectorsUsed() const;
    bool isValid() const;

    QString propertyString() const;

    const KstCurveHintList* curveHints() const;

    const QString& script() const;
    void setScript(const QString& script);
    void setProperty(const QString& property);

  protected:
    void showNewDialog();
    void showEditDialog();
    QString _propertyString;
    QString _script;
};

typedef KstSharedPtr<JSDataObject> JSDataObjectPtr;
typedef KstObjectList<JSDataObjectPtr> JSDataObjectList;

#endif

// vim: ts=2 sw=2 et
