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
    typedef QMap<QString, QVariant> ValueMap;

    DialogTab(QWidget *parent);
    virtual ~DialogTab();

    QString tabTitle() const { return _tabTitle; }
    void setTabTitle(const QString &tabTitle) { _tabTitle = tabTitle; }

  Q_SIGNALS:
    void ok();
    void apply();
    void cancel();

    //subclasses must emit...
    void modified();

  private:
    QString _tabTitle;
};

}

#endif

// vim: ts=2 sw=2 et
