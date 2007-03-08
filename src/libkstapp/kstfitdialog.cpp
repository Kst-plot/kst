/***************************************************************************
                     kstfitdialog.cpp  -  Part of KST
                             -------------------
    begin                : Wed Jul 28 2004
    copyright            : (C) 2003 The University of Toronto
                           (C) 2004 The University of British Columbia
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

#include <assert.h>

#include <stdio.h>

// include files for Qt
#include <qlineedit.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QLabel>
#include <QGridLayout>

// include files for KDE
#include <kmessagebox.h>

// application specific includes
#include "curveappearancewidget.h"
#include "kst2dplot.h"
#include "kstdataobjectcollection.h"
#include "kstfitdialog.h"
#include "kstvcurve.h"
#include "kstviewlabel.h"
#include "kstviewwindow.h"
#include "plugincollection.h"
#include "plugindialogwidget.h"
#include "pluginmanager.h"
#include "scalarselector.h"
#include "stringselector.h"
#include "vectorselector.h"

QPointer<KstFitDialogI> KstFitDialogI::_inst;

KstFitDialogI *KstFitDialogI::globalInstance() {
  if (!_inst) {
    _inst = new KstFitDialogI(KstApp::inst());
  }
  return _inst;
}


KstFitDialogI::KstFitDialogI(QWidget* parent, Qt::WindowFlags fl)
: KstPluginDialogI(parent, fl) {
  _w->_curveAppearance->show();
}


KstFitDialogI::~KstFitDialogI() {
}


void KstFitDialogI::show_setCurve(const QString& strCurve,
                                  const QString& strPlotName,
                                  const QString& strWindow) {

  KstVCurvePtr curve;
  KstBaseCurveList curves = kstObjectSubList<KstDataObject, KstBaseCurve>(KST::dataObjectList);
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve, KstVCurve>(curves);
  KstCPluginList c = kstObjectSubList<KstDataObject, KstCPlugin>(KST::dataObjectList);
  QString new_label;

  _strWindow   = strWindow;
  _strPlotName = strPlotName;
  _strCurve    = strCurve;
  curve = *vcurves.findTag(strCurve);
  if (curve) {
    KstReadLocker rl(curve);
    _yvector = curve->yVTag().displayString();
    _xvector = curve->xVTag().displayString();
    _evector = curve->yETag().displayString();
  }
  if (_xvector == "<None>" || _yvector == "<None>") {
    return;
  }
  show();
}


void KstFitDialogI::updatePluginList() {
  PluginCollection *pc = PluginCollection::self();
  const QMap<QString,Plugin::Data>& _pluginMap = pc->pluginList();
  QString previous = _pluginList[_w->PluginCombo->currentItem()];
  QMap<QString,Plugin::Data>::ConstIterator it;
  int newFocus = -1;
  int cnt = 0;

  _w->PluginCombo->clear();
  _pluginList.clear();
  for (it = _pluginMap.begin(); it != _pluginMap.end(); ++it) {
    if (it.value()._isFit) {
      if (!it.value()._isFitWeighted || _evector != "<None>") {
        _pluginList += it.value()._name;
        _w->PluginCombo->insertItem(i18n("%1 (v%2)").arg(it.data()._readableName).arg(it.value()._version));
        if (it.value()._name == previous) {
          newFocus = cnt;
        }
        ++cnt;
      }
    }
  }

  if (newFocus != -1) {
    _w->PluginCombo->setCurrentItem(newFocus);
  } else {
    _w->PluginCombo->setCurrentItem(0);
    pluginChanged(0);
  }
}


bool KstFitDialogI::createCurve(KstCPluginPtr plugin) {
  KstVectorPtr xVector;
  KstVectorPtr yVector;

  if (plugin->inputVectors().contains("X Array")) {
    xVector = plugin->inputVectors()["X Array"];
  }
  if (plugin->outputVectors().contains("Y Fitted")) {
    yVector = plugin->outputVectors()["Y Fitted"];
  }

  if (!xVector || !yVector) {
    return false;
  }

  QString c_name = KST::suggestCurveName(plugin->tag(), true);

  KstVCurvePtr fit = new KstVCurve(c_name, KstVectorPtr(xVector), KstVectorPtr(yVector), KstVectorPtr(0L), KstVectorPtr(0L), KstVectorPtr(0L), KstVectorPtr(0L), _w->_curveAppearance->color());
  fit->setHasPoints(_w->_curveAppearance->showPoints());
  fit->setHasLines(_w->_curveAppearance->showLines());
  fit->setHasBars(_w->_curveAppearance->showBars());
  fit->setLineWidth(_w->_curveAppearance->lineWidth());
  fit->setLineStyle(_w->_curveAppearance->lineStyle());
  fit->pointType = _w->_curveAppearance->pointType();
  fit->setBarStyle(_w->_curveAppearance->barStyle());
  fit->setPointDensity(_w->_curveAppearance->pointDensity());

  // add the label and the curve to the plot
  KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(_strWindow));
  if (w && w->view()->findChild(_strPlotName)) {
    Kst2DPlotPtr plot = kst_cast<Kst2DPlot>(w->view()->findChild(_strPlotName));
    if (plot) {
      QString strLabel = QString("[%1]").arg(plugin->tagName());      
      KstViewLabelPtr label = new KstViewLabel();
      
      label->setTransparent(true);
      label->resizeFromAspect(0.1, 0.1, 0.05, 0.05);
      label->setJustification(SET_KST_JUSTIFY(KST_JUSTIFY_H_CENTER, KST_JUSTIFY_V_CENTER));
      
      plot->appendChild(KstViewObjectPtr(label), true);
      label->setText(strLabel);
      
      plot->addCurve(KstBaseCurvePtr(fit));
    }
  }

  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(fit.data());
  KST::dataObjectList.lock().unlock();

  return true;
}


bool KstFitDialogI::newObject() {
  QString tagName = _tagName->text();

  if (tagName != plugin_defaultTag) {
    if (KstData::self()->dataTagNameNotUnique(tagName, true, this)) {
      _tagName->setFocus();
      return false;
    }
  }

  int pitem = _w->PluginCombo->currentItem();

  if (pitem >= 0 && _w->PluginCombo->count() > 0) {
    KstSharedPtr<Plugin> pPtr = PluginCollection::self()->plugin(_pluginList[pitem]);

    if (pPtr) {
      KstCPluginPtr plugin = new KstCPlugin;
      KstWriteLocker pl(plugin);
      plugin->setDirty();
      if (saveInputs(plugin, pPtr)) {
        if (tagName == plugin_defaultTag) {
          tagName = KST::suggestPluginName(_pluginList[pitem], KstObjectTag::fromString(_strCurve));
        }

        plugin->setTagName(KstObjectTag(tagName, KstObjectTag::globalTagContext)); // FIXME: tag context always global?

        plugin->setPlugin(pPtr);

        if (saveOutputs(plugin, pPtr)) {
          if (plugin->isValid()) {
            if (!createCurve(plugin)) {
              KMessageBox::sorry(this, i18n("Could not create curve from fit, possibly due to a bad plugin."));
              return false;
            }
            KST::dataObjectList.lock().writeLock();
            KST::dataObjectList.append(plugin.data());
            KST::dataObjectList.lock().unlock();
          } else {
            KMessageBox::sorry(this, i18n("There is something wrong (i.e, there is a bug) with the creation of the fit plugin."));
            return false;
          }
        } else {
          KMessageBox::sorry(this, i18n("There is an error in the outputs you entered."));
          return false;
        }
      } else {
        KMessageBox::sorry(this, i18n("There is an error in the inputs you entered."));
        return false;
      }
    } else {
      KMessageBox::sorry(this, i18n("There is something wrong (i.e, there is a bug) with"
                                    " the selected plugin.\n"));
      return false;
    }

    emit modified();
  }
  return true;
}


void KstFitDialogI::generateEntries(bool input, int& cnt, QWidget *parent, QGridLayout *grid, const Q3ValueList<Plugin::Data::IOValue>& table) {

  const QString& pluginName = _pluginList[_w->PluginCombo->currentItem()];
  const Plugin::Data& pluginData = PluginCollection::self()->pluginList()[PluginCollection::self()->pluginNameList()[pluginName]];
  bool isWeighted = pluginData._isFitWeighted;

  QString scalarLabelTemplate, vectorLabelTemplate, stringLabelTemplate;
  int iInputVector = 0;

  if (input) {
    stringLabelTemplate = i18n("Input String - %1:");
    scalarLabelTemplate = i18n("Input Scalar - %1:");
    vectorLabelTemplate = i18n("Input Vector - %1:");
  } else {
    stringLabelTemplate = i18n("Output String - %1:");
    scalarLabelTemplate = i18n("Output Scalar - %1:");
    vectorLabelTemplate = i18n("Output Vector - %1:");
  }

  for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = table.begin(); it != table.end(); ++it) {
    QString labellabel;
    bool string = false;
    bool scalar = false;
    bool fixed = false;
    switch ((*it)._type) {
      case Plugin::Data::IOValue::PidType:
        continue;
      case Plugin::Data::IOValue::FloatType:
        labellabel = scalarLabelTemplate.arg((*it)._name);
        scalar = true;
        break;
      case Plugin::Data::IOValue::StringType:
        labellabel = stringLabelTemplate.arg((*it)._name);
        string = true;
        break;
      case Plugin::Data::IOValue::TableType:
        if (input && ( iInputVector < 2 || ( isWeighted && iInputVector < 3 ) ) ) {
          fixed = true;
        }
        if ((*it)._subType == Plugin::Data::IOValue::FloatSubType ||
            (*it)._subType == Plugin::Data::IOValue::FloatNonVectorSubType) {
          labellabel = vectorLabelTemplate.arg((*it)._name);
        } else {
          // unsupported
          continue;
        }
        break;
      default:
        // unsupported
        continue;
    }

    QLabel *label = new QLabel(labellabel, parent, input ? "Input label" : "Output label");
    QWidget *widget = 0L;

    if (input) {
      if (scalar) {
        ScalarSelector *w = new ScalarSelector(parent, (*it)._name.toLatin1());
        widget = w;
        connect(w->_scalar, SIGNAL(activated(const QString&)), this, SLOT(updateScalarTooltip(const QString&)));
        connect(widget, SIGNAL(newScalarCreated()), this, SIGNAL(modified()));
        if (!(*it)._default.isEmpty()) {
          w->_scalar->addItem((*it)._default);
          w->_scalar->setItemText(w->_scalar->currentIndex(), (*it)._default);
        }
        KstScalarPtr p = *KST::scalarList.findTag(w->_scalar->currentText());
        w->allowDirectEntry( true );
        if (p) {
          w->_scalar->setToolTip(QString::number(p->value()));
        }
      } else if (string) {
        StringSelector *w = new StringSelector(parent, (*it)._name.toLatin1());
        widget = w;
        connect(w->_string, SIGNAL(activated(const QString&)), this, SLOT(updateStringTooltip(const QString&)));
        connect(widget, SIGNAL(newStringCreated()), this, SIGNAL(modified()));
        if (!(*it)._default.isEmpty()) {
          w->_string->addItem((*it)._default);
          w->_string->setItemText(w->_string->currentIndex(), (*it)._default);
        }
        KstStringPtr p = *KST::stringList.findTag(w->_string->currentText());
        w->allowDirectEntry( true );
        if (p) {
          w->_string->setToolTip(p->value());
        }
      } else {
        if (fixed) {
          widget = new QLabel(parent, (*it)._name.toLatin1());
          switch (iInputVector) {
            case 0:
              static_cast<QLabel*>(widget)->setText(_xvector);
              break;
            case 1:
              static_cast<QLabel*>(widget)->setText(_yvector);
              break;
            case 2:
              static_cast<QLabel*>(widget)->setText(_evector);
              break;
          }
          iInputVector++;
        } else {
          widget = new VectorSelector(parent, (*it)._name.toLatin1());
          connect(widget, SIGNAL(newVectorCreated(const QString&)), this, SIGNAL(modified()));
        }
      }
    } else {
      widget = new QLineEdit(parent, (*it)._name.toLatin1());
    }

    grid->addWidget(label, cnt, 0);
    _pluginWidgets.push_back(label);
    label->show();

    grid->addWidget(widget, cnt, 1);
    _pluginWidgets.push_back(widget);
    widget->show();

    if (!(*it)._description.isEmpty()) {
      Q3WhatsThis::remove(label);
      Q3WhatsThis::remove(widget);
      Q3WhatsThis::add(label, (*it)._description);
      Q3WhatsThis::add(widget, (*it)._description);
    }

    ++cnt;
  }
}


bool KstFitDialogI::saveInputs(KstCPluginPtr plugin, KstSharedPtr<Plugin> p) {
  bool rc = true;

  const Q3ValueList<Plugin::Data::IOValue>& itable = p->data()._inputs;
  for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = itable.begin(); it != itable.end(); ++it) {
    if ((*it)._type == Plugin::Data::IOValue::TableType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.toLatin1(), "VectorSelector");
      KstVectorPtr v;
      KstReadLocker vl(&KST::vectorList.lock());
      if (!field) {
        field = _w->_pluginInputOutputFrame->child((*it)._name.toLatin1(), "QLabel");
        assert(field);
        v = *KST::vectorList.findTag(static_cast<QLabel*>(field)->text());
      } else {
        VectorSelector *vs = static_cast<VectorSelector*>(field);
        v = *KST::vectorList.findTag(vs->selectedVector());
      }
      if (v) {
        plugin->inputVectors().insert((*it)._name, v);
      } else if (plugin->inputVectors().contains((*it)._name)) {
        plugin->inputVectors().erase((*it)._name);
        rc = false;
      }
    } else if ((*it)._type == Plugin::Data::IOValue::StringType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.toLatin1(), "StringSelector");
      assert(field);
      StringSelector *ss = static_cast<StringSelector*>(field);
      KstWriteLocker sl(&KST::stringList.lock());
      KstStringPtr s = *KST::stringList.findTag(ss->selectedString());
      if (s == *KST::stringList.end()) {
        QString val = ss->_string->currentText();
        // create orphan string
        KstStringPtr newString = new KstString(KstObjectTag(ss->_string->currentText(), KstObjectTag::orphanTagContext), 0L, val, true);
        plugin->inputStrings().insert((*it)._name, newString);
      } else {
        plugin->inputStrings().insert((*it)._name, s);
      }
    } else if ((*it)._type == Plugin::Data::IOValue::PidType) {
      // Nothing
    } else if ((*it)._type == Plugin::Data::IOValue::FloatType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.toLatin1(), "ScalarSelector");
      assert(field);
      ScalarSelector *ss = static_cast<ScalarSelector*>(field);
      KstWriteLocker sl(&KST::scalarList.lock());
      KstScalarPtr s = *KST::scalarList.findTag(ss->selectedScalar());
      if (s == *KST::scalarList.end()) {
        bool ok;
        double val = ss->_scalar->currentText().toDouble(&ok);

        if (ok) {
          // create orphan scalar
          KstScalarPtr newScalar = new KstScalar(KstObjectTag(ss->_scalar->currentText(), KstObjectTag::orphanTagContext), 0L, val, true, false);
          plugin->inputScalars().insert((*it)._name, newScalar);
        } else {
          rc = false;
        }
      } else {
        plugin->inputScalars().insert((*it)._name, s);
      }
    } else {
    }
  }

  return rc;
}


#include "kstfitdialog.moc"

// vim: ts=2 sw=2 et
