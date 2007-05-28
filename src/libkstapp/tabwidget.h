/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>

namespace Kst {

class View;

class TabWidget : public QTabWidget
{
  Q_OBJECT
  public:
    TabWidget(QWidget *parent);
    ~TabWidget();

    View *currentView() const;

  public Q_SLOTS:
    View *createView();

  private Q_SLOTS:
    void viewDestroyed(QObject *object);
};

}

#endif

// vim: ts=2 sw=2 et
