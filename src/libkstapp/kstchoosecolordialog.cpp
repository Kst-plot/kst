/**************************************************************************
        kstchoosecolordialog.cpp - source file: inherits designer dialog
                             -------------------
    begin                :  2001
    copyright            : (C) 2000-2003 by Barth Netterfield
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

#include <qlineedit.h>
#include <q3groupbox.h>  
#include <q3valuelist.h>
#include <qradiobutton.h>
//Added by qt3to4:
#include <Q3GridLayout>

#include <kcombobox.h>
#include <kcolorcombo.h>

#include "kst.h"
#include "kstchoosecolordialog.h"
#include "kstcolorsequence.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"
#include "kstvcurve.h"
#include "kstrvector.h"

KstChooseColorDialogI::KstChooseColorDialogI(QWidget* parent,
                                           const char* name,
                                           bool modal,
                                           Qt::WFlags fl)
  : KstChooseColorDialog(parent, name, modal, fl) {
    
    xVector->setChecked(true); 
    connect(OK, SIGNAL(clicked()), this, SLOT(applyColors()));  
    grid = 0L; 
  }


  KstChooseColorDialogI::~KstChooseColorDialogI() {
    delete grid;
  }


  void KstChooseColorDialogI::updateChooseColorDialog() {
    
    // cannot use dataSourceList.fileNames() as it contains datasources that
    // are not used by any curves or vectors
    KstRVectorList vcList = kstObjectSubList<KstVector, KstRVector>(KST::vectorList);

    // buildup a list of filenames
    QStringList fileNameList;
    for (KstRVectorList::Iterator vc_iter = vcList.begin(); 
         vc_iter != vcList.end();
         ++vc_iter)
    {
      if (fileNameList.contains((*vc_iter)->filename()) == 0)
        fileNameList.push_back((*vc_iter)->filename());
    }
    
    // cleanup the dialog
    cleanColorGroup();
    
    // new grid
    grid = new Q3GridLayout(colorFrame, fileNameList.count(), 2, 0, 8);
    grid->setColStretch(1,0);
    
    int i = fileNameList.count();
    for (QStringList::Iterator it = fileNameList.begin(); 
         it != fileNameList.end(); 
         ++it)
    { 
      // fill in the textfields
      QLineEdit* dsName = new QLineEdit(colorFrame, "dsName"+i);
      dsName->setReadOnly(true);
      dsName->setText(*it);
      grid->addWidget(dsName,i,0);
      lineEdits.push_back(dsName);
      dsName->show();
      
      // add the colorcombo
      KColorCombo* dsColor = new KColorCombo(colorFrame, "dsColor"+i);
      dsColor->setColor(KstColorSequence::next());
      grid->addWidget(dsColor,i,1);
      colorCombos.push_back(dsColor);
      dsColor->show();
      i++;
    }   
    
    adjustSize();
    resize(QSize(500, minimumSizeHint().height()));
    setFixedHeight(height());
  }
  
  void KstChooseColorDialogI::cleanColorGroup()    
  {
    while (!lineEdits.isEmpty())
    {
      QLineEdit* tempLineEdit = lineEdits.back();
      lineEdits.pop_back();
      delete tempLineEdit;
    }
    
    while (!colorCombos.isEmpty())
    {
      KColorCombo* tempColorCombo = colorCombos.back();
      colorCombos.pop_back();
      delete tempColorCombo;
    }
    delete grid;
  }


  void KstChooseColorDialogI::showChooseColorDialog() {
    updateChooseColorDialog();
    OK->setEnabled(true);
    Cancel->setEnabled(true);
    show();
    raise();
  }


  void KstChooseColorDialogI::applyColors() {
    OK->setEnabled(false);
    Cancel->setEnabled(false);
    
    KstVCurveList cvList = kstObjectSubList<KstDataObject, KstVCurve>(KST::dataObjectList);
    
    for (KstVCurveList::iterator cv_iter = cvList.begin();
         cv_iter != cvList.end();
         ++cv_iter)
    {
      KstVectorPtr vect;
      if (xVector->isChecked())
      {
        vect = (*cv_iter)->xVector();
      }
      else
      {
        vect = (*cv_iter)->yVector();
      }
      if (kst_cast<KstRVector>(vect))
      {
        // directly using a data vector
        (*cv_iter)->setColor(getColorForFile(kst_cast<KstRVector>(vect)->filename()));
      }
    }
    // force an update in case we're in paused mode
    KstApp::inst()->forceUpdate();    
    close();
  }
  
  QColor KstChooseColorDialogI::getColorForFile(const QString &fileName) {
    
    // get the selected color for the file
    Q3ValueList<KColorCombo*>::Iterator kc_iter = colorCombos.begin();
    for (Q3ValueList<QLineEdit*>::Iterator fn_iter = lineEdits.begin(); 
         fn_iter != lineEdits.end(); 
         ++fn_iter)
    {
      if (fileName == (*fn_iter)->text())
      {
        return (*kc_iter)->color();
      }
      ++kc_iter;
    }
    return QColor();   
  }

#include "kstchoosecolordialog.moc"
// vim: ts=2 sw=2 et
