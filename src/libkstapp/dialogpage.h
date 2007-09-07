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

#ifndef DIALOGPAGE_H
#define DIALOGPAGE_H

#include <QWidget>

#include "kst_export.h"

#include "ui_dialogpage.h"

namespace Kst {

class DialogTab;

class KST_EXPORT DialogPage : public QWidget, public Ui::DialogPage
{
  Q_OBJECT
  public:
    DialogPage(QWidget *parent);
    virtual ~DialogPage();

    QString pageTitle() const { return _pageTitle; }
    void setPageTitle(const QString &pageTitle) { _pageTitle = pageTitle; }

    QPixmap pageIcon() const { return _pageIcon; }
    void setPageIcon(const QPixmap &pageIcon) { _pageIcon = pageIcon; }

    void addDialogTab(DialogTab *tab);

  Q_SIGNALS:
    void ok();
    void apply();
    void cancel();
    void modified();

  private:
    QString _pageTitle;
    QPixmap _pageIcon;
};

}

#endif

// vim: ts=2 sw=2 et
