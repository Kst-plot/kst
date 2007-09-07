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

#ifndef DIALOGTAB_H
#define DIALOGTAB_H

#include <QWidget>

#include "kst_export.h"

namespace Kst {

//FIXME maybe we should really have a model...

class KST_EXPORT DialogTab : public QWidget
{
  Q_OBJECT
  public:
    DialogTab(QWidget *parent);
    virtual ~DialogTab();

    QString tabTitle() const { return _tabTitle; }
    void setTabTitle(const QString &tabTitle) { _tabTitle = tabTitle; }

  public Q_SLOTS:
    virtual void apply();
    virtual void restoreDefaults();

  Q_SIGNALS:
    void modified(bool isModified);

  protected:
    virtual void showEvent(QShowEvent *event);

  private:
    QString _tabTitle;
};

}

#endif

// vim: ts=2 sw=2 et
