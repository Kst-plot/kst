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

#ifndef BUTTONITEM_H
#define BUTTONITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"
#include <QLocalSocket>

class QPushButton;

namespace Kst {

/** For scripting, a button which sends via a socket on trigger. */

class ButtonItem : public ViewItem
{
  Q_OBJECT
  protected:
    QList<QLocalSocket*> _sockets;
    QPushButton* _pushButton;
    QGraphicsProxyWidget* _proxy;
  public:
    ButtonItem(View *parent);
    virtual ~ButtonItem();

    const QString defaultsGroupName() const {return ButtonItem::staticDefaultsGroupName();}
    static QString staticDefaultsGroupName() { return QString("button");}

    // for view item dialogs
    virtual bool hasStroke() const {return true;}
    virtual bool hasBrush() const {return true;}

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);

  public slots:
    void setText(QString);
    void addSocket(QLocalSocket* s);
    void notifyScripts();

    friend class ButtonItemFactory;
};

class ButtonItemFactory : public GraphicsFactory {
  public:
    ButtonItemFactory();
    ~ButtonItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
