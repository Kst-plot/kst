/***************************************************************************
                      kstprintoptionspage.cpp  -  Part of KST
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

#include <qbutton.h> 
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>
//Added by qt3to4:
#include <QGridLayout>

#include <klocale.h>

#include "kstprintoptionspage.h"
#include "kstmonochromedialog.h"
#include "kst.h"

KstPrintOptionsPage::KstPrintOptionsPage(QWidget *parent, const char *name)
: KPrintDialogPage(parent, name) {
  int row = 0;

  setTitle(i18n("Kst Options"));

  QGridLayout *grid = new QGridLayout(this, 4, 2);

  _dateTimeFooter = new QCheckBox(i18n("Append plot information to each page"), this);
  grid->addMultiCellWidget(_dateTimeFooter, row, row, 0, 1);
  row++;

  _maintainAspectRatio = new QCheckBox(i18n("Maintain aspect ratio"), this);
  grid->addMultiCellWidget(_maintainAspectRatio, row, row, 0, 1);
  row++;

  _bw = new QCheckBox(i18n("Print in monochrome"), this);
  grid->addWidget(_bw, row, 0);
  _configureBW = new QPushButton(i18n("Configure..."), this);
  grid->addWidget(_configureBW, row, 1);
  row++;

  grid->addWidget(new QLabel(i18n("Curve width adjustment:"), this), row, 0);
  _curveWidthAdjust = new QSpinBox(-20, 20, 1, this);
  _curveWidthAdjust->setValue(0);
  _curveWidthAdjust->setSuffix(i18n("px"));
  grid->addWidget(_curveWidthAdjust, row, 1);
  row++;

  grid->activate();
  
  connect(_configureBW, SIGNAL(clicked()), KstApp::inst(), SLOT(showMonochromeDialog())); 
  connect(_bw, SIGNAL(toggled(bool)), _configureBW, SLOT(setEnabled(bool)));
}


KstPrintOptionsPage::~KstPrintOptionsPage() {
}


void KstPrintOptionsPage::setOptions(const QMap<QString,QString>& opts) {
  _bw->setChecked(opts["kst-plot-monochrome"] == "1");
  _dateTimeFooter->setChecked(opts["kst-plot-datetime-footer"] == "1");
  _maintainAspectRatio->setChecked(opts["kst-plot-maintain-aspect-ratio"] == "1");
  _curveWidthAdjust->setValue(opts["kst-plot-curve-width-adjust"].toInt());
  
  // set options for monochrome dialog
  KstApp::inst()->monochromeDialog()->setOptions(opts);
  
  // update buttons
  _configureBW->setEnabled(_bw->isChecked());
}


void KstPrintOptionsPage::getOptions(QMap<QString,QString>& opts, bool include_def) {
  // datetime footer - default is false
  if (_dateTimeFooter->isChecked() || include_def) {
    opts["kst-plot-datetime-footer"] = _dateTimeFooter->isChecked() ? "1" : "0";
  }
  // monochrome - default is false
  if (_bw->isChecked() || include_def) {
    opts["kst-plot-monochrome"] = _bw->isChecked() ? "1" : "0";
  }
  // maintain aspect ratio - default is false
  if (_maintainAspectRatio->isChecked() || include_def) {
    opts["kst-plot-maintain-aspect-ratio"] = _maintainAspectRatio->isChecked() ? "1" : "0";
  }
  // curve width adjustment - default is 0
  int adj = _curveWidthAdjust->value();
  if (adj != 0 || include_def) {
    opts["kst-plot-curve-width-adjust"] = QString::number(adj);
  }
  
  // get options from monochrome dialog
  KstApp::inst()->monochromeDialog()->getOptions(opts, include_def);
}


bool KstPrintOptionsPage::isValid(QString& msg) {
  Q_UNUSED(msg)
  return true;
}

// vim: ts=2 sw=2 et
