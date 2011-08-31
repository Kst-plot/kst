/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 Joshua Netterfield                               *
 *                   joshua.netterfield@gmail.com                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SCRIPTINTERFACE_H
#define SCRIPTINTERFACE_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QObject>
typedef QList<QByteArray> QByteArrayList;

namespace Kst {

/** A script interface represents an object exposed through the scripting interface.
  * (ex., dialog, primitive, etc.)
  */
class ScriptInterface : public QObject
{
    Q_OBJECT
public:
    virtual QByteArrayList commands()=0;
    virtual QString doCommand(QString)=0;
    virtual bool isValid()=0;
    virtual QByteArray getHandle()=0;
};

/** Enables recursive if statements in scripting. The syntax for if statements is 'if(...)' to begin a statement and 'fi()' to end.*/
struct IfSI {
    IfSI* parent;
    bool on;
    int recurse;
    IfSI(IfSI*p,bool isTrue) : parent(p),on(isTrue),recurse(0) {}
};

/** Enables variables in scripting. Variables begin with the prefix '$' */
struct VarSI {
    QByteArray handle;
    QByteArray val;
    VarSI(QByteArray v,QByteArray e) : handle(v), val(e) {}
};

/** Enables macros (lists of commands to be executed) in scripting. Macro names begin with the prefix '#' */
struct MacroSI {
    MacroSI* parent;
    QByteArray handle;
    QByteArrayList args;
    QByteArrayList commands;
    MacroSI(MacroSI*p,QByteArray h,QByteArrayList v,QByteArrayList c) : parent(p), handle(h), args(v), commands(c) {}
};

}

#endif // SCRIPTINTERFACE_H
