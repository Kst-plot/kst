/***************************************************************************
                             debugnotifier.h
                             ---------------
    begin                : Sep 13 2005
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

#ifndef DEBUGNOTIFIER_H
#define DEBUGNOTIFIER_H

#include <QLabel>
#include <QPixmap>
#include <QVector>

class QMouseEvent;

namespace Kst {

class DebugNotifier : public QLabel {
  Q_OBJECT
  public:
    DebugNotifier(QWidget *parent);
    ~DebugNotifier();

  Q_SIGNALS:
    void showDebugLog();

  public Q_SLOTS:
    void close();
    void reanimate();

  private Q_SLOTS:
    void animate();

  protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

  private:
    int _animationStage;
    bool _gotPress;
    QVector<QPixmap> _pm;
};

}

#endif

// vim: ts=2 sw=2 et
