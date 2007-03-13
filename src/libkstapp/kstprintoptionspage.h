/***************************************************************************
                      kstprintoptionspage.h  -  Part of KST
                             -------------------
    begin                : Thu Sep 23 2004
    copyright            : (C) 2004 The University of Toronto
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

#ifndef _KST_POP_H
#define _KST_POP_H

class Q3Button;
class QCheckBox;
#include <kdeprint/kprintdialogpage.h>

class KstPrintOptionsPage : public KPrintDialogPage {
  public:
    KstPrintOptionsPage(QWidget *parent = 0L);
    virtual ~KstPrintOptionsPage();

    void setOptions(const QMap<QString,QString>& opts);
    void getOptions(QMap<QString,QString>& opts, bool include_def = false);
    bool isValid(QString& msg);

  private:
    QCheckBox *_dateTimeFooter;
    QCheckBox *_maintainAspectRatio;
    QCheckBox *_bw;
    QPushButton *_configureBW;
    QSpinBox *_curveWidthAdjust;
};

#endif

// vim: ts=2 sw=2 et
