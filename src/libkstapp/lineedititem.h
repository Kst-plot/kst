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

#ifndef LINEEDITITEM_H
#define LINEEDITITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"
#include <QLocalSocket>

class QLineEdit;

namespace Kst {

/** For scripting, a line edit which sends via a socket on edit. */

class LineEditItem : public ViewItem
{
  Q_OBJECT
  protected:
    QList<QLocalSocket*> _sockets;
    QLineEdit* _lineEdit;
    QGraphicsProxyWidget* _proxy;
  public:
    LineEditItem(View *parent);
    virtual ~LineEditItem();

    const QString defaultsGroupName() const {return LineEditItem::staticDefaultsGroupName();}
    static QString staticDefaultsGroupName() { return QString("lineEdit");}

    // for view item dialogs
    virtual bool hasStroke() const {return true;}
    virtual bool hasBrush() const {return true;}

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);

  public slots:
    void setText(QString);
    void addSocket(QLocalSocket* s);
    void notifyScripts();

    friend class LineEditItemFactory;
};

class LineEditItemFactory : public GraphicsFactory {
  public:
    LineEditItemFactory();
    ~LineEditItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
