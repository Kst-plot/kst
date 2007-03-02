/***************************************************************************
                    ksteditviewobjectdialog.cpp  -  Part of KST
                             -------------------
    begin                : 2005
    copyright            : (C) 2005 The University of British Columbia
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

#include "ksteditviewobjectdialog.h"

#include <qbutton.h>
#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmetaobject.h>
#include <qpainter.h>
#include <qspinbox.h>
#include <qstyle.h>
//Added by qt3to4:
#include <QGridLayout>
#include <Q3ValueList>
#include <QPixmap>

#include <kcolorbutton.h>
#include <QFontComboBox>
#include <knuminput.h>
#include <kurlrequester.h>
  
#include "kst.h"

#include <klocale.h>
#include <stdio.h>

KstEditViewObjectDialogI::KstEditViewObjectDialogI(QWidget* parent, Qt::WindowFlags fl) 
: KstEditViewObjectDialog(parent, fl) {
  connect(_cancel, SIGNAL(clicked()), this, SLOT(close()));
  connect(_apply, SIGNAL(clicked()), this, SLOT(applyClicked()));
  connect(_OK, SIGNAL(clicked()), this, SLOT(okClicked()));

  _grid = 0L;
  _viewObject = 0L;
  _isNew = false;

  resize(360, 200);
  setMinimumSize(360, 200);
}


KstEditViewObjectDialogI::~KstEditViewObjectDialogI() {
  if (_viewObject) {
    _viewObject->setDialogLock(false);
  }
}


void KstEditViewObjectDialogI::setNew() {
  _isNew = true;
  _apply->setEnabled(false);
}


void KstEditViewObjectDialogI::updateEditViewObjectDialog() {
  updateWidgets();
}


void KstEditViewObjectDialogI::showEditViewObjectDialog(KstViewObjectPtr viewObject, KstTopLevelViewPtr top) {
  _viewObject = viewObject;
  if (_viewObject) {
    _viewObject->setDialogLock(true);
  }
  _top = top;
  updateWidgets();
  if (_viewObject) {
    setWindowTitle(i18n("Edit %1").arg(_viewObject->type()));
  }
  _apply->setEnabled(false);
  show();
  raise();
}


void KstEditViewObjectDialogI::clearWidgets() {
  // clear all the current widgets from the grid
  for (Q3ValueList<QWidget*>::Iterator i = _inputWidgets.begin(); i != _inputWidgets.end(); ++i) {
    delete *i;
  }
  _inputWidgets.clear();
  for (Q3ValueList<QWidget*>::Iterator i = _widgets.begin(); i != _widgets.end(); ++i) {
    delete *i;
  }
  _widgets.clear();
  delete _customWidget;
  // and the delete the grid itself
  delete _grid;
  _grid = 0L;
}


void KstEditViewObjectDialogI::updateWidgets() {
  // clear all the current widgets from the grid
  clearWidgets();

  // get the qt properties of the viewobject
  if (_viewObject) {
    _customWidget = _viewObject->configWidget();
    if (_customWidget) {
      _grid = new QGridLayout(_propertiesFrame, 1, 1);
      _customWidget->reparent(_propertiesFrame, QPoint(0, 0));
      _grid->addWidget(_customWidget, 0, 0);
      _viewObject->fillConfigWidget(_customWidget, _isNew);
      if (!_isNew) {
        _viewObject->connectConfigWidget(this, _customWidget);
      }
      resize(minimumSizeHint());
      return;
    }
    
    //---------------------------------------------------------------
    // NOTE: due to Early return, nothing after this line is executed
    // if the view object provides a custom widget.

    int numProperties = _viewObject->metaObject()->numProperties(true);

    // create a new grid
    _grid = new QGridLayout(_propertiesFrame, numProperties, 2, 0, 8);
    _grid->setColStretch(0,0);
    _grid->setColStretch(1,1);
    
    // get the property names and types
    for (int i = 0; i < numProperties; i++) {
      const QMetaProperty* property = _viewObject->metaObject()->property(i, true);
      QString propertyType(property->type());
      QString propertyName(property->name());
     
      // for this property, get the meta-data map
      QMap<QString, QVariant> metaData = _viewObject->widgetHints(propertyName);
      
      if (!metaData.empty()) {
        QString friendlyName = metaData["_kst_label"].toString();
        QString widgetType = metaData["_kst_widgetType"].toString();
        metaData.erase("_kst_label");
        metaData.erase("_kst_widgetType");
        
        // use friendly name for label
        QLabel* propertyLabel = new QLabel(_propertiesFrame, "label-"+i);
        propertyLabel->setText(friendlyName);
        _grid->addWidget(propertyLabel,i,0);
        _widgets.append(propertyLabel);
        propertyLabel->show();
        
        // display different types of widgets depending on what dialogData specifies
        QWidget* propertyWidget = 0L;
        if (widgetType == "QSpinBox") {
          // insert a spinbox
          propertyWidget = new QSpinBox(_propertiesFrame, (propertyName+","+"value").toLatin1()); 
          propertyWidget->setProperty("value", _viewObject->property(property->name()));
          if (!_isNew) {
            connect(propertyWidget, SIGNAL(valueChanged(const QString&)), this, SLOT(modified()));
            connect(propertyWidget->child("qt_spinbox_edit"), SIGNAL(textChanged(const QString&)), this, SLOT(modified()));
          }
        } else if (widgetType == "KColorButton") {
          // insert a colorbutton
          propertyWidget = new KColorButton(_propertiesFrame, (propertyName+","+"color").toLatin1());
          propertyWidget->setProperty("color", _viewObject->property(property->name()));
          if (!_isNew) {
            connect(propertyWidget, SIGNAL(changed(const QColor&)), this, SLOT(modified()));
          }
        } else if (widgetType == "QLineEdit") {
          // insert a text field
          propertyWidget = new QLineEdit(_propertiesFrame, (propertyName+","+"text").toLatin1());
          propertyWidget->setProperty("text", _viewObject->property(property->name()));
          if (!_isNew) {
            connect(propertyWidget, SIGNAL(textChanged(const QString&)), this, SLOT(modified()));
          }
        } else if (widgetType == "KUrlRequester") {
          // insert a url requester
          propertyWidget = new KUrlRequester(_propertiesFrame, (propertyName+","+"url").toLatin1());
          propertyWidget->setProperty("url", _viewObject->property(property->name()));
          if (!_isNew) {
            connect(propertyWidget, SIGNAL(textChanged(const QString&)), this, SLOT(modified()));
            connect(propertyWidget, SIGNAL(urlSelected(const QString&)), this, SLOT(modified()));
          }
        } else if (widgetType == "PenStyleWidget") {
          // insert a combobox with QT pen styles
          QComboBox* combo = new QComboBox(_propertiesFrame, (propertyName+","+"currentItem").toLatin1());
          fillPenStyleWidget(combo);
          propertyWidget = combo;
          propertyWidget->setProperty("currentItem", _viewObject->property(property->name()));
          if (!_isNew) {
            connect(propertyWidget, SIGNAL(activated(int)), this, SLOT(modified()));
          }
        } else if (widgetType == "QCheckBox") {
          // insert a checkbox
          propertyWidget = new QCheckBox(_propertiesFrame, (propertyName+","+"checked").toLatin1());
          propertyWidget->setProperty("checked", _viewObject->property(property->name()));
          if (!_isNew) {
            connect(propertyWidget, SIGNAL(pressed()), this, SLOT(modified()));
          }
        } else if (widgetType == "KDoubleSpinBox") {
          // insert a double num spinbox
          KDoubleSpinBox* input = new KDoubleSpinBox(_propertiesFrame, (propertyName+","+"value").toLatin1());
          // need this so that setValue later works
          input->setMinValue(metaData["minValue"].toDouble());
          input->setMaxValue(metaData["maxValue"].toDouble());
          input->setLineStep(metaData["lineStep"].toDouble());
          metaData.erase("minValue");
          metaData.erase("maxValue");
          metaData.erase("lineStep");
          propertyWidget = input; 
          propertyWidget->setProperty("value", _viewObject->property(property->name()));
          // need the following line because of a KDE bug causing valueChanged(double) never to be emitted 
          if (!_isNew) {
            connect(propertyWidget, SIGNAL(valueChanged(int)), this, SLOT(modified()));
            connect(propertyWidget, SIGNAL(valueChanged(double)), this, SLOT(modified()));
            connect(propertyWidget->child("qt_spinbox_edit"), SIGNAL(textChanged(const QString&)), this, SLOT(modified()));
          }
        } else if (widgetType == "QFontComboBox") {
          // insert a font combo box
          propertyWidget = new QFontComboBox(_propertiesFrame, (propertyName+","+"currentText").toLatin1());
          propertyWidget->setProperty("currentText", _viewObject->property(property->name()));  
          if (!_isNew) {
            connect(propertyWidget, SIGNAL(activated(int)), this, SLOT(modified()));
          }
        } else if (widgetType == "HJustifyCombo") {
          // insert a combo box filled with horizontal justifications
          QComboBox* combo = new QComboBox(_propertiesFrame, (propertyName+","+"currentItem").toLatin1());
          fillHJustifyWidget(combo);
          propertyWidget = combo;
          propertyWidget->setProperty("currentItem", _viewObject->property(property->name()));
          if (!_isNew) {
            connect(propertyWidget, SIGNAL(activated(int)), this, SLOT(modified()));
          }
        } else if (widgetType == "VJustifyCombo") {
          // insert a combo box filled with vertical justifications
          QComboBox* combo = new QComboBox(_propertiesFrame, (propertyName+","+"currentItem").toLatin1());
          fillVJustifyWidget(combo);
          propertyWidget = combo;
          propertyWidget->setProperty("currentItem", _viewObject->property(property->name()));
          if (!_isNew) {
            connect(propertyWidget, SIGNAL(activated(int)), this, SLOT(modified()));
          }
        }
        
        // also set any additional properties specified by metaData
        for (QMap<QString, QVariant>::ConstIterator it = metaData.begin(); it != metaData.end(); ++it) {
          propertyWidget->setProperty(it.key().toLatin1(), it.data());
        }
        
        _grid->addWidget(propertyWidget, i, 1);
        _inputWidgets.append(propertyWidget);
        propertyWidget->show();
      }   
    }
    
    // geometry cleanup
    resize(minimumSizeHint());
    //setFixedHeight(height());
  }
}


void KstEditViewObjectDialogI::fillPenStyleWidget(QComboBox* widget) {
  QRect rect = widget->style().querySubControlMetrics(QStyle::CC_ComboBox, 
                                                      widget, 
                                                      QStyle::SC_ComboBoxEditField);
  rect.setLeft(rect.left() + 2);
  rect.setRight(rect.right() - 2);
  rect.setTop(rect.top() + 2);
  rect.setBottom(rect.bottom() - 2);

  // fill the combo with pen styles
  QPixmap ppix(rect.width(), rect.height());
  QPainter pp(&ppix);
  QPen pen(Qt::black, 0);

  widget->clear(); 
  
  Q3ValueList<Qt::PenStyle> styles;
  styles.append(Qt::SolidLine);
  styles.append(Qt::DashLine);
  styles.append(Qt::DotLine);
  styles.append(Qt::DashDotLine);
  styles.append(Qt::DashDotDotLine);
  
  while (!styles.isEmpty()) {
    pen.setStyle(styles.front());
    pp.setPen(pen);
    pp.fillRect( pp.window(), QColor("white"));
    pp.drawLine(1,ppix.height()/2,ppix.width()-1, ppix.height()/2);
    widget->insertItem(ppix);
    styles.pop_front();
  }
}


void KstEditViewObjectDialogI::fillHJustifyWidget(QComboBox* widget) {
  widget->insertItem(i18n("Left"));
  widget->insertItem(i18n("Right"));
  widget->insertItem(i18n("Center")); 
}
    

void KstEditViewObjectDialogI::fillVJustifyWidget(QComboBox* widget) {
  widget->insertItem(i18n("Top"));
  widget->insertItem(i18n("Bottom"));
  widget->insertItem(i18n("Center")); 
}


void KstEditViewObjectDialogI::modified() {
  _apply->setEnabled(true);
}


void KstEditViewObjectDialogI::applyClicked() {
  if (_customWidget) {
    // FILL ME IN TODO
    _viewObject->readConfigWidget(_customWidget);
  } else {
    // get all the properties and set them
    for (Q3ValueList<QWidget*>::ConstIterator iter = _inputWidgets.begin(); iter != _inputWidgets.end(); ++iter) {
      
      // get the widget type and property name
      QString propertyName = QString((*iter)->name()).section(',', 0, 0);
      QString widgetPropertyName = QString((*iter)->name()).section(',', 1, 1);
      
      // get the widget's property and set it on the viewObject
      _viewObject->setProperty(propertyName.toLatin1(), (*iter)->property(widgetPropertyName.toLatin1()));
    }
#if 0
    // Removed by George.  This is very strange.  Some dialogs have 10+
    // properties, and when I change 8 of them, the next "new" object of the
    // same type has all of these modified on me.  I have to go through and
    // change them all back to what they were before.  I think this is too
    // confusing and annoying.  We could add a sticky flag or something like
    // that if this feature is really demanded.
    
    // and then save this viewObject's properties as the default
    if (_top) {
      _top->saveDefaults(_viewObject); 
    }
#endif
  }
  
  _apply->setDisabled(true);
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


void KstEditViewObjectDialogI::okClicked() {
  if (!_viewObject) {
    QDialog::reject();
    return;
  }

  applyClicked();

  QDialog::accept();
}


#include "ksteditviewobjectdialog.moc"
// vim: ts=2 sw=2 et
