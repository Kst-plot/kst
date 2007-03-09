/***************************************************************************
                   view2dplotwidget.cpp
                             -------------------
    begin                : 02/28/07
    copyright            : (C) 2007 The University of Toronto
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

#include "view2dplotwidget.h"

#include <qfontdatabase.h>
#include <qstyle.h>
#include <qpainter.h>
#include <qdeepcopy.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include "kstplotdefines.h"
#include "kstsettings.h"
#include "kstlinestyle.h"
#include "kstdataobjectcollection.h"
#include "kstplotlabel.h"
#include "kstdebug.h"
#include "kstdefaultnames.h"

#include <kst_export.h>
//Added by qt3to4:
#include <QPixmap>

struct MajorTickSpacing {
  const char *label;
  int majorTickDensity;
};


const MajorTickSpacing MajorTickSpacings[] = {
      {
        I18N_NOOP("Coarse"), 2
      },
      { I18N_NOOP("Default"), 5 },
      { I18N_NOOP("Fine"), 10 },
      { I18N_NOOP("Very fine"), 15 }
    };


const unsigned int numMajorTickSpacings = sizeof( MajorTickSpacings ) / sizeof( MajorTickSpacing );


View2DPlotWidget::View2DPlotWidget(QWidget *parent)
    : QWidget(parent) {
  setupUi(this);
}


View2DPlotWidget::~View2DPlotWidget() {}


void View2DPlotWidget::init() {

  QFontDatabase qfd;

  _plot = 0L;

  connect(DisplayedCurveList, SIGNAL(clicked(Q3ListBoxItem*)),
          this, SLOT(updateButtons()));
  connect(AvailableCurveList, SIGNAL(clicked(Q3ListBoxItem*)),
          this, SLOT(updateButtons()));
  connect(DisplayedCurveList, SIGNAL(doubleClicked(Q3ListBoxItem*)),
          this, SLOT(removeDisplayedCurve()));
  connect(AvailableCurveList, SIGNAL(doubleClicked(Q3ListBoxItem*)),
          this, SLOT(addDisplayedCurve()));
  connect(DisplayedCurveList, SIGNAL(selectionChanged()),
          this, SLOT(updateButtons()));
  connect(AvailableCurveList, SIGNAL(selectionChanged()),
          this, SLOT(updateButtons()));
  connect(_remove, SIGNAL(clicked()),
          this, SLOT(removeDisplayedCurve()));
  connect(_add, SIGNAL(clicked()),
          this, SLOT(addDisplayedCurve()));
  connect(_up, SIGNAL(clicked()),
          DisplayedCurveList, SLOT(up()));
  connect(_down, SIGNAL(clicked()),
          DisplayedCurveList, SLOT(down()));

  _up->setIcon(BarIcon("up"));
  _up->setEnabled(false);
  _up->setShortcut(Qt::ALT+Qt::Key_Up);
  _down->setIcon(BarIcon("down"));
  _down->setEnabled(false);
  _down->setShortcut(Qt::ALT+Qt::Key_Down);
  _add->setIcon(BarIcon("forward"));
  _add->setEnabled(false);
  _add->setShortcut(Qt::ALT+Qt::Key_S);
  _remove->setIcon(BarIcon("back"));
  _remove->setEnabled(false);
  _remove->setShortcut(Qt::ALT+Qt::Key_R);

  _up->setToolTip(i18n("Shortcut: Alt+Up"));
  _down->setToolTip(i18n("Shortcut: Alt+Down"));
  _add->setToolTip(i18n("Shortcut: Alt+s"));
  _remove->setToolTip(i18n("Shortcut: Alt+r"));

  FontComboBox->setEditable(false);

  // axes range
  connect(XAC, SIGNAL(toggled(bool)), XACRange, SLOT(setEnabled(bool)));
  connect(YAC, SIGNAL(toggled(bool)), YACRange, SLOT(setEnabled(bool)));
  connect(YExpression, SIGNAL(toggled(bool)), this, SLOT(updateButtons()));
  connect(XExpression, SIGNAL(toggled(bool)), this, SLOT(updateButtons()));

  connect(scalarSelectorX1, SIGNAL(activated(const QString&)),
          XExpressionMin, SLOT(insert(const QString&)));
  connect(scalarSelectorY1, SIGNAL(activated(const QString&)),
          YExpressionMin, SLOT(insert(const QString&)));
  connect(scalarSelectorX2, SIGNAL(activated(const QString&)),
          XExpressionMax, SLOT(insert(const QString&)));
  connect(scalarSelectorY2, SIGNAL(activated(const QString&)),
          YExpressionMax, SLOT(insert(const QString&)));

  // adding/removing curves
  connect(DisplayedCurveList, SIGNAL(clicked(Q3ListBoxItem*)), this, SLOT(updateButtons()));
  connect(AvailableCurveList, SIGNAL(clicked(Q3ListBoxItem*)), this, SLOT(updateButtons()));
  connect(DisplayedCurveList, SIGNAL(doubleClicked(Q3ListBoxItem*)), this, SLOT(removeDisplayedCurve()));
  connect(AvailableCurveList, SIGNAL(doubleClicked(Q3ListBoxItem*)), this, SLOT(addDisplayedCurve()));
  connect(DisplayedCurveList, SIGNAL(selectionChanged()), this, SLOT(updateButtons()));
  connect(AvailableCurveList, SIGNAL(selectionChanged()), this, SLOT(updateButtons()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(removeDisplayedCurve()));
  connect(_add, SIGNAL(clicked()), this, SLOT(addDisplayedCurve()));
  connect(_up, SIGNAL(clicked()),
          DisplayedCurveList, SLOT(up()));
  connect(_down, SIGNAL(clicked()),
          DisplayedCurveList, SLOT(down()));

  connect(AutoLabel, SIGNAL(clicked()), this, SLOT(generateDefaultLabels()));


  connect(ScalarList, SIGNAL(activated(int)), this, SLOT(insertCurrentScalar()));

  connect(YAxisText, SIGNAL(selectionChanged()), this, SLOT(setScalarDestYLabel()));
  connect(YAxisText, SIGNAL(textChanged(const QString &)), this, SLOT(setScalarDestYLabel()));
  connect(XAxisText, SIGNAL(selectionChanged()), this, SLOT(setScalarDestXLabel()));
  connect(XAxisText, SIGNAL(textChanged(const QString &)), this, SLOT(setScalarDestXLabel()));
  connect(TopLabelText, SIGNAL(selectionChanged()), this, SLOT(setScalarDestTopLabel()));
  connect(TopLabelText, SIGNAL(textChanged(const QString &)), this, SLOT(setScalarDestTopLabel()));

  connect(_checkBoxXInterpret, SIGNAL(toggled(bool)), _comboBoxXInterpret, SLOT(setEnabled(bool)));
  connect(_checkBoxXInterpret, SIGNAL(toggled(bool)), _comboBoxXDisplay, SLOT(setEnabled(bool)));
  connect(_checkBoxXInterpret, SIGNAL(toggled(bool)), textLabelXDisplayAs, SLOT(setEnabled(bool)));
  connect(_checkBoxYInterpret, SIGNAL(toggled(bool)), _comboBoxYInterpret, SLOT(setEnabled(bool)));
  connect(_checkBoxYInterpret, SIGNAL(toggled(bool)), _comboBoxYDisplay, SLOT(setEnabled(bool)));
  connect(_checkBoxYInterpret, SIGNAL(toggled(bool)), textLabelYDisplayAs, SLOT(setEnabled(bool)));
  connect(_xTransformTop, SIGNAL(toggled(bool)), _xTransformTopExp, SLOT(setEnabled(bool)));
  connect(_yTransformRight, SIGNAL(toggled(bool)), _yTransformRightExp, SLOT(setEnabled(bool)));
  connect(_checkBoxDefaultMarkerColor, SIGNAL(toggled(bool)), _colorMarker, SLOT(setDisabled(bool)));

  ScalarDest = TopLabelText;

  connect(_pushButtonEditLegend, SIGNAL(clicked()), this, SLOT(editLegend()));

  FontComboBox->setFonts(qfd.families());

  for (unsigned i = 0; i < numMajorTickSpacings; i++) {
    _xMajorTickSpacing->addItem(i18n(MajorTickSpacings[i].label));
    _yMajorTickSpacing->addItem(i18n(MajorTickSpacings[i].label));
  }

  for (unsigned i = 0; i < numAxisInterpretations; i++) {
    _comboBoxXInterpret->addItem(i18n(AxisInterpretations[i].label));
    _comboBoxYInterpret->addItem(i18n(AxisInterpretations[i].label));
  }
  for (unsigned i = 0; i < numAxisDisplays; i++) {
    _comboBoxXDisplay->addItem(i18n(AxisDisplays[i].label));
    _comboBoxYDisplay->addItem(i18n(AxisDisplays[i].label));
  }

  _comboBoxXInterpret->setEnabled(false);
  _comboBoxXDisplay->setEnabled(false);
  textLabelXDisplayAs->setEnabled(false);

  _comboBoxYInterpret->setEnabled(false);
  _comboBoxYDisplay->setEnabled(false);
  textLabelYDisplayAs->setEnabled(false);

  appearanceThisPlot->setChecked(true);
  XAxisThisPlot->setChecked(true);
  YAxisThisPlot->setChecked(true);
  rangeThisPlot->setChecked(true);
  markersThisPlot->setChecked(true);

  // plot markers
  connect(AddPlotMarker, SIGNAL(clicked()),
          this, SLOT(addPlotMarker()));
  connect(RemovePlotMarker, SIGNAL(clicked()),
          this, SLOT(removePlotMarker()));
  connect(RemoveAllPlotMarkers, SIGNAL(clicked()),
          this, SLOT(removeAllPlotMarkers()));
  connect(PlotMarkerList, SIGNAL(clicked(Q3ListBoxItem*)),
          this, SLOT(updateButtons()));
  connect(PlotMarkerList, SIGNAL(selectionChanged()),
          this, SLOT(updateButtons()));
  connect(NewPlotMarker, SIGNAL(returnPressed()),
          this, SLOT(addPlotMarker()));
  connect(NewPlotMarker, SIGNAL(textChanged(const QString &)),
          this, SLOT(updateButtons()));
  connect(UseCurve, SIGNAL(clicked()),
          this, SLOT(updateButtons()));
  connect(UseVector, SIGNAL(clicked()),
          this, SLOT(updateButtons()));

  // grid lines
  connect(_xMajorGrid, SIGNAL(clicked()),
          this, SLOT(updateAxesButtons()));
  connect(_yMajorGrid, SIGNAL(clicked()),
          this, SLOT(updateAxesButtons()));
  connect(_xMinorGrid, SIGNAL(clicked()),
          this, SLOT(updateAxesButtons()));
  connect(_yMinorGrid, SIGNAL(clicked()),
          this, SLOT(updateAxesButtons()));

  connect(_xMinorTicksAuto, SIGNAL(clicked()),
          this, SLOT(updateAxesButtons()));
  connect(_yMinorTicksAuto, SIGNAL(clicked()),
          this, SLOT(updateAxesButtons()));
  connect(_checkBoxDefaultMajorGridColor, SIGNAL(clicked()),
          this, SLOT(updateAxesButtons()));
  connect(_checkBoxDefaultMinorGridColor, SIGNAL(clicked()),
          this, SLOT(updateAxesButtons()));

  QColor defaultColor((KstSettings::globalSettings()->foregroundColor.Qt::red() + KstSettings::globalSettings()->backgroundColor.Qt::red())/2,
                      (KstSettings::globalSettings()->foregroundColor.Qt::green() + KstSettings::globalSettings()->backgroundColor.Qt::green())/2,
                      (KstSettings::globalSettings()->foregroundColor.Qt::blue() + KstSettings::globalSettings()->backgroundColor.Qt::blue())/2);
  _majorGridColor->setColor(defaultColor);
  _minorGridColor->setColor(defaultColor);

  /* set defaults */
  plotColors->setColor(KstSettings::globalSettings()->backgroundColor);

  _xMajorTickSpacing->setCurrentIndex(1);
  _yMajorTickSpacing->setCurrentIndex(1);

  _yMarksInsidePlot->setChecked(true);
  _xMarksInsidePlot->setChecked(true);

  _comboBoxTopLabelJustify->addItem(i18n("Left"));
  _comboBoxTopLabelJustify->addItem(i18n("Right"));
  _comboBoxTopLabelJustify->addItem(i18n("Center"));

  // FIXME: should use kstsettings
  _axisPenWidth->setValue(0);
  _majorPenWidth->setValue(0);
  _minorPenWidth->setValue(0);

  _colorMarker->setColor(QColor("black"));
  _spinBoxMarkerLineWidth->setValue(0);
  _checkBoxDefaultMarkerColor->setChecked(true);
  fillMarkerLineCombo();

}


void View2DPlotWidget::generateDefaultLabels() {
  if (_plot) {
    _plot->generateDefaultLabels();
    XAxisText->setText(_plot->xLabel()->text());
    YAxisText->setText(_plot->yLabel()->text());
    TopLabelText->setText(_plot->topLabel()->text());
  }
}

//FIXME:Destroy(?)

void View2DPlotWidget::updateButtons() {
  bool selected = false;
  uint count = AvailableCurveList->count();

  for (uint i = 0; i < count; i++) {
    if (AvailableCurveList->isSelected(i)) {
      selected = true;
    }
  }
  if (selected && !_add->isEnabled()) {
    _add->setEnabled(true);
  } else if (!selected && _add->isEnabled()) {
    _add->setEnabled(false);
  }

  selected = false;
  count = DisplayedCurveList->count();
  for (uint i = 0; i < count; i++) {
    if (DisplayedCurveList->isSelected(i)) {
      selected = true;
    }
  }
  if (selected && !_remove->isEnabled()) {
    _remove->setEnabled(true);
  } else if (!selected && _remove->isEnabled()) {
    _remove->setEnabled(false);
  }
  if (selected && !_up->isEnabled()) {
    _up->setEnabled(true);
    _down->setEnabled(true);
  } else if (!selected && _up->isEnabled()) {
    _up->setEnabled(false);
    _down->setEnabled(false);
  }

  // updates for Plot Markers tab
  AddPlotMarker->setEnabled(!NewPlotMarker->text().isEmpty());

  selected = false;
  count = PlotMarkerList->count();
  for (uint i = 0; i < count; i++) {
    if (PlotMarkerList->isSelected(i)) {
      selected = true;
    }
  }
  RemovePlotMarker->setEnabled(selected);
  RemoveAllPlotMarkers->setEnabled(count > 0);

  // updates for auto-generation marker curve section
  CurveCombo->setEnabled(UseCurve->isChecked());
  Rising->setEnabled(UseCurve->isChecked());
  Falling->setEnabled(UseCurve->isChecked());
  Both->setEnabled(UseCurve->isChecked());
  _textLabelCreateMarkersOn->setEnabled(UseCurve->isChecked());
  _vectorForMarkers->setEnabled(UseVector->isChecked());

  //updates for range tab
  YExpressionMin->setEnabled(YExpression->isChecked());
  YExpressionMax->setEnabled(YExpression->isChecked());
  scalarSelectorY1->setEnabled(YExpression->isChecked());
  scalarSelectorY2->setEnabled(YExpression->isChecked());
  XExpressionMin->setEnabled(XExpression->isChecked());
  XExpressionMax->setEnabled(XExpression->isChecked());
  scalarSelectorX1->setEnabled(XExpression->isChecked());
  scalarSelectorX2->setEnabled(XExpression->isChecked());

  _xTransformTopExp->setEnabled(_xTransformTop->isChecked());
  _yTransformRightExp->setEnabled(_yTransformRight->isChecked());

}


void View2DPlotWidget::addDisplayedCurve() {
  uint count = AvailableCurveList->count();

  if (count > 0) {
    for (int i = count-1; i >= 0; i--) {
      if (AvailableCurveList->isSelected(i)) {
        DisplayedCurveList->insertItem(AvailableCurveList->text(i));
        AvailableCurveList->removeItem(i);
      }
    }
    updateButtons();
    emit changed();
  }
  TrackContents->setChecked(false);
}


void View2DPlotWidget::removeDisplayedCurve() {
  uint count = DisplayedCurveList->count();

  if (count > 0) {
    for (int i = count-1; i >= 0; i--) {
      if (DisplayedCurveList->isSelected(i)) {
        AvailableCurveList->insertItem(DisplayedCurveList->text(i));
        DisplayedCurveList->removeItem(i);
      }
    }
    updateButtons();
    emit changed();
  }
  TrackContents->setChecked(false);

}


void View2DPlotWidget::fillMarkerLineCombo() {
  QRect rect = _comboMarkerLineStyle->style().querySubControlMetrics(
                 QStyle::CC_ComboBox, _comboMarkerLineStyle, QStyle::SC_ComboBoxEditField);
  rect.setLeft(rect.left() + 2);
  rect.setRight(rect.right() - 2);
  rect.setTop(rect.top() + 2);
  rect.setBottom(rect.bottom() - 2);

  // fill the point type dialog with point types
  QPixmap ppix(rect.width(), rect.height());
  QPainter pp(&ppix);
  QPen pen(QColor("black"), 0);

  int currentIndex = _comboMarkerLineStyle->currentIndex();
  _comboMarkerLineStyle->clear();

  for (int style = 0; style < (int)KSTLINESTYLE_MAXTYPE; style++) {
    pen.setStyle(KstLineStyle[style]);
    pp.setPen(pen);
    pp.fillRect( pp.window(), QColor("white"));
    pp.drawLine(1,ppix.height()/2,ppix.width()-1, ppix.height()/2);
    _comboMarkerLineStyle->addItem(ppix, "");
  }

  _comboMarkerLineStyle->setCurrentIndex(currentIndex);
}


void View2DPlotWidget::updateAxesButtons() {
  bool major = _xMajorGrid->isChecked() || _yMajorGrid->isChecked();
  bool minor = _xMinorGrid->isChecked() || _yMinorGrid->isChecked();

  _checkBoxDefaultMajorGridColor->setEnabled(major);
  _checkBoxDefaultMinorGridColor->setEnabled(minor);
  _majorGridColor->setEnabled(major && !_checkBoxDefaultMajorGridColor->isChecked());
  _minorGridColor->setEnabled(minor && !_checkBoxDefaultMinorGridColor->isChecked());
  _majorPenWidth->setEnabled(major);
  _minorPenWidth->setEnabled(minor);
  _majorPenWidthLabel->setEnabled(major);
  _minorPenWidthLabel->setEnabled(minor);
  _xMinorTicks->setEnabled(!_xMinorTicksAuto->isChecked());
  _yMinorTicks->setEnabled(!_yMinorTicksAuto->isChecked());
}


void View2DPlotWidget::updateScalarCombo() {
  ScalarList->clear();
  scalarSelectorX1->clear();
  scalarSelectorX2->clear();
  scalarSelectorY1->clear();
  scalarSelectorY2->clear();
  KST::scalarList.lock().readLock();
  KstScalarList sl = Q3DeepCopy<KstScalarList>(KST::scalarList.list());
  KST::scalarList.lock().unlock();
  qSort(sl);
  for (KstScalarList::ConstIterator i = sl.begin(); i != sl.end(); ++i) {
    (*i)->readLock();
    QString n = (*i)->tagLabel();
    (*i)->unlock();
    ScalarList->addItem(n);
    scalarSelectorX1->addItem(n);
    scalarSelectorX2->addItem(n);
    scalarSelectorY1->addItem(n);
    scalarSelectorY2->addItem(n);
  }
}


void View2DPlotWidget::updatePlotMarkers(const Kst2DPlot *plot) {
  for (KstMarkerList::ConstIterator it = plot->plotMarkers().begin(); it != plot->plotMarkers().end(); ++it) {
    if ((*it).isRising) {
      PlotMarkerList->insertItem(i18n("%1 [rising]").arg(QString::number((*it).value, 'g', MARKER_LABEL_PRECISION)));
    } else if ((*it).isFalling) {
      PlotMarkerList->insertItem(i18n("%1 [falling]").arg(QString::number((*it).value, 'g', MARKER_LABEL_PRECISION)));
    } else if ((*it).isVectorValue) {
      PlotMarkerList->insertItem(i18n("%1 [value]").arg(QString::number((*it).value, 'g', MARKER_LABEL_PRECISION)));
    } else {
      PlotMarkerList->insertItem(QString::number((*it).value, 'g', MARKER_LABEL_PRECISION));
    }
  }

  // update the auto-generation settings
  KstBaseCurveList curves = kstObjectSubList<KstDataObject, KstBaseCurve>(KST::dataObjectList);
  CurveCombo->clear();
  for (KstBaseCurveList::ConstIterator curves_iter = curves.begin(); curves_iter != curves.end(); ++curves_iter) {
    (*curves_iter)->readLock();
    CurveCombo->addItem((*curves_iter)->tagName());
    (*curves_iter)->unlock();
  }

  if (plot->hasCurveToMarkers()) {
    UseCurve->setChecked(true);
    Both->setChecked(false);
    Falling->setChecked(false);
    Rising->setChecked(false);
    if (plot->curveToMarkersFallingDetect()) {
      if (plot->curveToMarkersRisingDetect()) {
        Both->setChecked(true);
      } else {
        Falling->setChecked(true);
      }
    } else {
      Rising->setChecked(true);
    }
    for (int curveComboIndex = 0; curveComboIndex < CurveCombo->count(); curveComboIndex++) {
      if (CurveCombo->itemText(curveComboIndex) == plot->curveToMarkers()->tagName()) {
        CurveCombo->setCurrentIndex(curveComboIndex);
        break;
      }
    }
  } else {
    UseCurve->setChecked(false);
  }

  if (plot->hasVectorToMarkers()) {
    UseVector->setChecked(true);
    _vectorForMarkers->setSelection(plot->vectorToMarkers()->tag().displayString());
  } else {
    UseVector->setChecked(false);
  }

}


void View2DPlotWidget::fillWidget(const Kst2DPlot *plot) {
  _plot = Kst2DPlot::findPlotByName(plot->tagName());
  _vectorForMarkers->update();
  scalarSelectorX1->update();
  scalarSelectorY1->update();
  scalarSelectorX2->update();
  scalarSelectorY2->update();

  KstBaseCurveList curves = kstObjectSubList<KstDataObject, KstBaseCurve>(KST::dataObjectList);

  DisplayedCurveList->clear();
  AvailableCurveList->clear();

  // add curves while retaining the order in the plot
  for (KstBaseCurveList::ConstIterator it = plot->Curves.begin(); it != plot->Curves.end(); ++it) {
    (*it)->readLock();
    DisplayedCurveList->insertItem((*it)->tagName());
    (*it)->unlock();
  }
  for (KstBaseCurveList::ConstIterator it = curves.begin(); it != curves.end(); ++it) {
    (*it)->readLock();
    if (plot->Curves.find(*it) == plot->Curves.end()) {
      AvailableCurveList->insertItem((*it)->tagName());
    }
    (*it)->unlock();
  }

  updateScalarCombo();
  updatePlotMarkers(plot);

  // update the checks and buttons
  _xMajorGrid->setChecked(plot->hasXMajorGrid());
  _xMinorGrid->setChecked(plot->hasXMinorGrid());
  _yMajorGrid->setChecked(plot->hasYMajorGrid());
  _yMinorGrid->setChecked(plot->hasYMinorGrid());
  _majorGridColor->setColor(plot->majorGridColor());
  _minorGridColor->setColor(plot->minorGridColor());
  _majorPenWidth->setValue(plot->majorPenWidth());
  _minorPenWidth->setValue(plot->minorPenWidth());
  _checkBoxDefaultMajorGridColor->setChecked(plot->defaultMajorGridColor());
  _checkBoxDefaultMinorGridColor->setChecked(plot->defaultMinorGridColor());

  // insert the current plot name in the plot name edit box
  _title->setText(plot->tagName());

  // fill the log axis check boxes
  XIsLog->setChecked(plot->isXLog());
  YIsLog->setChecked(plot->isYLog());

  _checkBoxXOffsetMode->setChecked(plot->xOffsetMode());
  _checkBoxYOffsetMode->setChecked(plot->yOffsetMode());

  double xmin, ymin, xmax, ymax;
  plot->getScale(xmin, ymin, xmax, ymax);

  QString xMinExp, xMaxExp, yMinExp, yMaxExp;
  plot->getXScaleExps(xMinExp, xMaxExp);
  plot->getYScaleExps(yMinExp, yMaxExp);

  XACRange->setText(QString::number(xmax - xmin, 'g', 16));
  YACRange->setText(QString::number(ymax - ymin, 'g', 16));

  switch (plot->xScaleMode()) {
  case AUTO:
    XAuto->setChecked(true);
    break;
  case AUTOBORDER:
    XAutoBorder->setChecked(true);
    break;
  case AC:
    XAC->setChecked(true);
    break;
  case FIXED:
    XExpression->setChecked(true); // treat fixed ranges as expressions
    XExpressionMin->setText(QString::number(xmin, 'g', 16));
    XExpressionMax->setText(QString::number(xmax, 'g', 16));
    break;
  case EXPRESSION:
    XExpression->setChecked(true);
    XExpressionMin->setText(xMinExp);
    XExpressionMax->setText(xMaxExp);
    break;
  case AUTOUP:
    XAutoUp->setChecked(true);
    break;
  case NOSPIKE:
    XNoSpikes->setChecked(true);
    break;
  default:
    XAuto->setChecked(true);
    break;
  }

  switch (plot->yScaleMode()) {
  case AUTO:
    YAuto->setChecked(true);
    break;
  case AUTOBORDER:
    YAutoBorder->setChecked(true);
    break;
  case AC:
    YAC->setChecked(true);
    break;
  case FIXED:
    YExpression->setChecked(true);
    YExpressionMin->setText(QString::number(ymin, 'g', 16));
    YExpressionMax->setText(QString::number(ymax, 'g', 16));
    break;
  case EXPRESSION:
    YExpression->setChecked(true);
    YExpressionMin->setText(yMinExp);
    YExpressionMax->setText(yMaxExp);
    break;
  case AUTOUP:
    YAutoUp->setChecked(true);
    break;
  case NOSPIKE:
    YNoSpikes->setChecked(true);
    break;
  default:
    YAuto->setChecked(true);
    break;
  }

  // update the major and minor tick settings
  _xMinorTicks->setValue(plot->xMinorTicks());
  _yMinorTicks->setValue(plot->yMinorTicks());
  _xMinorTicksAuto->setChecked(plot->xMinorTicksAuto());
  _yMinorTicksAuto->setChecked(plot->yMinorTicksAuto());

  //void KstPlotDialogI::setMajorSpacing(QComboBox* majorTicks, int spacing) {

  for (int i = 0; i < (int)numMajorTickSpacings; i++) {
    if (MajorTickSpacings[i].majorTickDensity <= plot->xMajorTicks()) {
      _xMajorTickSpacing->setCurrentIndex(i);
    }
    if (MajorTickSpacings[i].majorTickDensity <= plot->yMajorTicks()) {
      _yMajorTickSpacing->setCurrentIndex(i);
    }

  }

  NumberFontSize->setValue(plot->xTickLabel()->fontSize());
  _spinBoxXAngle->setValue((int)plot->xTickLabel()->rotation());
  _spinBoxYAngle->setValue((int)plot->yTickLabel()->rotation());

  XAxisText->setText(plot->xLabel()->text());
  XLabelFontSize->setValue(plot->xLabel()->fontSize());

  YAxisText->setText(plot->yLabel()->text());
  YLabelFontSize->setValue(plot->yLabel()->fontSize());

  TopLabelText->setText(plot->topLabel()->text());
  TopLabelFontSize->setValue(plot->topLabel()->fontSize());
  FontComboBox->setCurrentFont(plot->topLabel()->fontName());
  switch (plot->topLabel()->justification()) {
  case KST_JUSTIFY_H_LEFT:
    _comboBoxTopLabelJustify->setCurrentIndex(0);
    break;
  case KST_JUSTIFY_H_RIGHT:
    _comboBoxTopLabelJustify->setCurrentIndex(1);
    break;
  case KST_JUSTIFY_H_CENTER:
    _comboBoxTopLabelJustify->setCurrentIndex(2);
    break;
  default:
    _comboBoxTopLabelJustify->setCurrentIndex(0);
    break;
  }

  // update the x-axis interpretation
  KstAxisInterpretation xAxisInterpretation;
  KstAxisInterpretation yAxisInterpretation;
  KstAxisDisplay xAxisDisplay;
  KstAxisDisplay yAxisDisplay;
  bool xAxisInterpret;
  bool yAxisInterpret;

  plot->getXAxisInterpretation(xAxisInterpret, xAxisInterpretation, xAxisDisplay);
  _checkBoxXInterpret->setChecked(xAxisInterpret);
  _comboBoxXInterpret->setEnabled(xAxisInterpret);
  _comboBoxXDisplay->setEnabled(xAxisInterpret);
  textLabelXDisplayAs->setEnabled(xAxisInterpret);

  if (xAxisInterpret) {
    for (unsigned i = 0; i < numAxisInterpretations; i++) {
      if (AxisInterpretations[i].type == xAxisInterpretation) {
        _comboBoxXInterpret->setCurrentIndex(i);
        break;
      }
    }
    for (unsigned i = 0; i < numAxisDisplays; i++) {
      if (AxisDisplays[i].type == xAxisDisplay) {
        _comboBoxXDisplay->setCurrentIndex(i);
        break;
      }
    }
  } else {
    _comboBoxXInterpret->setCurrentIndex(KstSettings::globalSettings()->xAxisInterpretation);
    _comboBoxXDisplay->setCurrentIndex(KstSettings::globalSettings()->xAxisDisplay);
  }

  plot->getYAxisInterpretation(yAxisInterpret, yAxisInterpretation, yAxisDisplay);
  _checkBoxYInterpret->setChecked(yAxisInterpret);
  _comboBoxYInterpret->setEnabled(yAxisInterpret);
  _comboBoxYDisplay->setEnabled(yAxisInterpret);
  textLabelYDisplayAs->setEnabled(yAxisInterpret);

  if (yAxisInterpret) {
    for (unsigned i = 0; i < numAxisInterpretations; i++) {
      if (AxisInterpretations[i].type == yAxisInterpretation) {
        _comboBoxYInterpret->setCurrentIndex(i);
        break;
      }
    }
    for (unsigned i = 0; i < numAxisDisplays; i++) {
      if (AxisDisplays[i].type == yAxisDisplay) {
        _comboBoxYDisplay->setCurrentIndex(i);
        break;
      }
    }
  } else {
    // FIXME: these should use kstsettings defaults
    _comboBoxYInterpret->setCurrentIndex(KstSettings::globalSettings()->xAxisInterpretation);
    _comboBoxYDisplay->setCurrentIndex(KstSettings::globalSettings()->xAxisDisplay);
  }

  // initialize the legend settings for the current plot
  KstViewLegendPtr vl = plot->legend();
  if (vl) {
    ShowLegend->setChecked(true);
    TrackContents->setChecked(vl->trackContents());
  } else { // plot does not currently have a legend: use defaults.
    ShowLegend->setChecked(false);
  }
  // initialize the plot color widget
  plotColors->setColor(plot->backgroundColor());

  _axisPenWidth->setValue(plot->axisPenWidth());

  //update the tick display options
  _xMarksInsidePlot->setChecked(plot->xTicksInPlot() && !plot->xTicksOutPlot());
  _xMarksOutsidePlot->setChecked(plot->xTicksOutPlot() && !plot->xTicksInPlot());
  _xMarksInsideAndOutsidePlot->setChecked(plot->xTicksOutPlot() && plot->xTicksInPlot());

  _yMarksInsidePlot->setChecked(plot->yTicksInPlot() && !plot->yTicksOutPlot());
  _yMarksOutsidePlot->setChecked(plot->yTicksOutPlot() && !plot->yTicksInPlot());
  _yMarksInsideAndOutsidePlot->setChecked(plot->yTicksOutPlot() && plot->yTicksInPlot());

  //update axis suppression
  _suppressTop->setChecked(plot->suppressTop());
  _suppressBottom->setChecked(plot->suppressBottom());
  _suppressRight->setChecked(plot->suppressRight());
  _suppressLeft->setChecked(plot->suppressLeft());

  //update axes transforms
  _yTransformRight->setChecked(!plot->yTransformedExp().isEmpty());
  _yTransformRightExp->setText(plot->yTransformedExp());
  _xTransformTop->setChecked(!plot->xTransformedExp().isEmpty());
  _xTransformTopExp->setText(plot->xTransformedExp());

  _xReversed->setChecked(plot->xReversed());
  _yReversed->setChecked(plot->yReversed());

  // update marker attributes
  _comboMarkerLineStyle->setCurrentIndex(plot->lineStyleMarkers());
  _spinBoxMarkerLineWidth->setValue(plot->lineWidthMarkers());
  _checkBoxDefaultMarkerColor->setChecked(plot->defaultColorMarker());
  _colorMarker->setColor(plot->colorMarkers());
  _colorMarker->setEnabled(!plot->defaultColorMarker());

  updateButtons();
  updateAxesButtons();
  ScalarDest = TopLabelText;

}


void View2DPlotWidget::applyAppearance(Kst2DPlotPtr plot) {
  Kst2DPlotList plots;
  Kst2DPlotPtr plotExtra;

  // only apply label text to this plot, despite radio button values
  plot->xLabel()->setText(XAxisText->text());
  plot->yLabel()->setText(YAxisText->text());
  plot->topLabel()->setText(TopLabelText->text());
  switch (_comboBoxTopLabelJustify->currentIndex()) {
  case 0:
    plot->topLabel()->setJustification(SET_KST_JUSTIFY(KST_JUSTIFY_H_LEFT, KST_JUSTIFY_V_NONE));
    break;
  case 1:
    plot->topLabel()->setJustification(SET_KST_JUSTIFY(KST_JUSTIFY_H_RIGHT, KST_JUSTIFY_V_NONE));
    break;
  case 2:
    plot->topLabel()->setJustification(SET_KST_JUSTIFY(KST_JUSTIFY_H_CENTER, KST_JUSTIFY_V_NONE));
    break;
  }

  if (appearanceThisPlot->isChecked()) {
    plots += plot;
  } else if (appearanceThisWindow->isChecked()) {
    plots = plot->topLevelParent()->findChildrenType<Kst2DPlot>(true);
  } else {
    plots = plot->globalPlotList();
  }

  for (uint i = 0; i < plots.size(); i++) {
    plotExtra = plots[i];

    plotExtra->setForegroundColor(plotColors->color());
    plotExtra->setBackgroundColor(plotColors->color());

    // gridlines colors
    plotExtra->setGridLinesColor(_majorGridColor->color(),
                                 _minorGridColor->color(),
                                 _checkBoxDefaultMajorGridColor->isChecked(),
                                 _checkBoxDefaultMinorGridColor->isChecked());

    plotExtra->setAxisPenWidth(_axisPenWidth->value());
    plotExtra->setMajorPenWidth(_majorPenWidth->value());
    plotExtra->setMinorPenWidth(_minorPenWidth->value());

    plotExtra->xLabel()->setFontName(FontComboBox->currentText());
    plotExtra->xLabel()->setFontSize(XLabelFontSize->value());

    plotExtra->yLabel()->setFontName(FontComboBox->currentText());
    plotExtra->yLabel()->setFontSize(YLabelFontSize->value());

    plotExtra->topLabel()->setFontName(FontComboBox->currentText());
    plotExtra->topLabel()->setFontSize(TopLabelFontSize->value());
    switch (_comboBoxTopLabelJustify->currentIndex()) {
    case 0:
      plotExtra->topLabel()->setJustification(SET_KST_JUSTIFY(KST_JUSTIFY_H_LEFT, KST_JUSTIFY_V_NONE));
      break;
    case 1:
      plotExtra->topLabel()->setJustification(SET_KST_JUSTIFY(KST_JUSTIFY_H_RIGHT, KST_JUSTIFY_V_NONE));
      break;
    case 2:
      plotExtra->topLabel()->setJustification(SET_KST_JUSTIFY(KST_JUSTIFY_H_CENTER, KST_JUSTIFY_V_NONE));
      break;
    }

    plotExtra->xTickLabel()->setFontName(FontComboBox->currentText());
    plotExtra->xTickLabel()->setFontSize(NumberFontSize->value());
    plotExtra->xTickLabel()->setRotation(_spinBoxXAngle->value());

    plotExtra->fullTickLabel()->setFontName(FontComboBox->currentText());
    plotExtra->fullTickLabel()->setFontSize(NumberFontSize->value());

    plotExtra->yTickLabel()->setFontName(FontComboBox->currentText());
    plotExtra->yTickLabel()->setFontSize(NumberFontSize->value());
    plotExtra->yTickLabel()->setRotation(_spinBoxYAngle->value());
    if (ShowLegend->isChecked()) {
      KstViewLegendPtr vl = plotExtra->getOrCreateLegend();
      vl->setTrackContents(TrackContents->isChecked());
    } else {
      KstViewLegendPtr vl = plotExtra->legend();
      if (vl) {
        plotExtra->removeChild(KstViewObjectPtr(vl));
      }
    }
    plotExtra->setDirty();
  }
}


void View2DPlotWidget::applyXAxis(Kst2DPlotPtr plot) {
  Kst2DPlotList plots;
  Kst2DPlotPtr  plotExtra;

  if (XAxisThisPlot->isChecked()) {
    plots += plot;
  } else if (XAxisThisWindow->isChecked()) {
    plots = plot->topLevelParent()->findChildrenType<Kst2DPlot>(true);
  } else {
    plots = plot->globalPlotList();
  }

  for (uint i = 0; i < plots.size(); i++) {
    plotExtra = plots[i];

    plotExtra->setLog(XIsLog->isChecked(), YIsLog->isChecked());

    plotExtra->setXOffsetMode(_checkBoxXOffsetMode->isChecked());

    if (_checkBoxXInterpret->isChecked()) {
      plotExtra->setXAxisInterpretation(true,
                                        AxisInterpretations[_comboBoxXInterpret->currentIndex()].type,
                                        AxisDisplays[_comboBoxXDisplay->currentIndex()].type);
    } else {
      plotExtra->setXAxisInterpretation(false,
                                        AXIS_INTERP_CTIME,
                                        AXIS_DISPLAY_YEAR);
    }
    // minor tick settings.
    if (_xMinorTicksAuto->isChecked()) {
      plotExtra->setXMinorTicks(-1);
    } else {
      plotExtra->setXMinorTicks(_xMinorTicks->value());
    }

    // major tick settings.
    plotExtra->setXMajorTicks(
      MajorTickSpacings[_xMajorTickSpacing->currentIndex()].majorTickDensity);

    // tick display
    plotExtra->setXTicksInPlot(_xMarksInsidePlot->isChecked() || _xMarksInsideAndOutsidePlot->isChecked());
    plotExtra->setXTicksOutPlot(_xMarksOutsidePlot->isChecked() || _xMarksInsideAndOutsidePlot->isChecked());

    // grid lines
    plotExtra->setXGridLines(_xMajorGrid->isChecked(), _xMinorGrid->isChecked());

    // axis suppression
    plotExtra->setSuppressTop(_suppressTop->isChecked());
    plotExtra->setSuppressBottom(_suppressBottom->isChecked());

    // transformed opposite axis
    if (_xTransformTop->isChecked()) {
      plotExtra->setXTransformedExp(_xTransformTopExp->text());
    } else {
      plotExtra->setXTransformedExp(QString::null);
    }

    plotExtra->setXReversed(_xReversed->isChecked());
    plotExtra->setDirty();
  }
}


void View2DPlotWidget::applyYAxis(Kst2DPlotPtr plot) {

  Kst2DPlotList plots;
  Kst2DPlotPtr  plotExtra;

  if (YAxisThisPlot->isChecked()) {
    plots += plot;
  } else if (YAxisThisWindow->isChecked()) {
    plots = plot->topLevelParent()->findChildrenType<Kst2DPlot>(true);
  } else {
    plots = plot->globalPlotList();
  }

  for (uint i = 0; i < plots.size(); i++) {
    plotExtra = plots[i];

    plotExtra->setYOffsetMode(_checkBoxYOffsetMode->isChecked());

    if (_checkBoxYInterpret->isChecked()) {
      plotExtra->setYAxisInterpretation(true,
                                        AxisInterpretations[_comboBoxYInterpret->currentIndex()].type,
                                        AxisDisplays[_comboBoxYDisplay->currentIndex()].type);
    } else {
      plotExtra->setYAxisInterpretation(false,
                                        AXIS_INTERP_CTIME,
                                        AXIS_DISPLAY_YEAR);
    }
    // minor ticks
    if (_yMinorTicksAuto->isChecked()) {
      plotExtra->setYMinorTicks(-1);
    } else {
      plotExtra->setYMinorTicks(_yMinorTicks->value());
    }
    // major ticks
    plotExtra->setYMajorTicks(
      MajorTickSpacings[_yMajorTickSpacing->currentIndex()].majorTickDensity);

    // tick display
    plotExtra->setYTicksInPlot(_yMarksInsidePlot->isChecked() || _yMarksInsideAndOutsidePlot->isChecked());
    plotExtra->setYTicksOutPlot(_yMarksOutsidePlot->isChecked() || _yMarksInsideAndOutsidePlot->isChecked());

    // grid lines
    plotExtra->setYGridLines(_yMajorGrid->isChecked(), _yMinorGrid->isChecked());

    // axis suppression
    plotExtra->setSuppressLeft(_suppressLeft->isChecked());
    plotExtra->setSuppressRight(_suppressRight->isChecked());

    // transformed opposite axis
    if (_yTransformRight->isChecked()) {
      plotExtra->setYTransformedExp(_yTransformRightExp->text());
    } else {
      plotExtra->setYTransformedExp(QString::null);
    }

    // reversed
    plotExtra->setYReversed(_yReversed->isChecked());
    plotExtra->setDirty();
  }
}


void View2DPlotWidget::applyRange(Kst2DPlotPtr plot) {
  Kst2DPlotList plots;
  Kst2DPlotPtr  plotExtra;

  if (rangeThisPlot->isChecked()) {
    plots += plot;
  } else if (rangeThisWindow->isChecked()) {
    plots = plot->topLevelParent()->findChildrenType<Kst2DPlot>(true);
  } else {
    plots = plot->globalPlotList();
  }

  for (uint i = 0; i < plots.size(); i++) {
    plotExtra = plots[i];

    // do X Scale
    if (XAC->isChecked()) {
      plotExtra->setXScaleMode(AC);
      plotExtra->setXScale(0, XACRange->text().toDouble());
    } else if (XExpression->isChecked()) {
      plotExtra->setXScaleMode(EXPRESSION);
      if (!plotExtra->setXExpressions(XExpressionMin->text(), XExpressionMax->text())) {
        KMessageBox::sorry(this, i18n("There is a problem with the X range expressions."));
        return;
      }
      //if expressions are constant, just use FIXED mode
      plotExtra->optimizeXExps();
    } else if (XAutoUp->isChecked()) {
      plotExtra->setXScaleMode(AUTOUP);
    } else if (XAuto->isChecked()) {
      plotExtra->setXScaleMode(AUTO);
    } else if (XAutoBorder->isChecked()) {
      plotExtra->setXScaleMode(AUTOBORDER);
    } else if (XNoSpikes->isChecked()) {
      plotExtra->setXScaleMode(NOSPIKE);
    } else {
      KstDebug::self()->log(i18n("Internal error: No X scale type checked in %1.").arg(_title->text()), KstDebug::Error);
    }

    // do Y Scale
    if (YAC->isChecked()) {
      plotExtra->setYScaleMode(AC);
      plotExtra->setYScale(0, YACRange->text().toDouble());
    } else if (YExpression->isChecked()) {
      plotExtra->setYScaleMode(EXPRESSION);
      if (!plotExtra->setYExpressions(YExpressionMin->text(), YExpressionMax->text())) {
        KMessageBox::sorry(this, i18n("There is a problem with the Y range expressions."));
        return;
      }
      //if expressions are constant, just use FIXED mode
      plotExtra->optimizeYExps();
    } else if (YAutoUp->isChecked()) {
      plotExtra->setYScaleMode(AUTOUP);
    } else if (YAuto->isChecked()) {
      plotExtra->setYScaleMode(AUTO);
    } else if (YAutoBorder->isChecked()) {
      plotExtra->setYScaleMode(AUTOBORDER);
    } else if (YNoSpikes->isChecked()) {
      plotExtra->setYScaleMode(NOSPIKE);
    } else {
      KstDebug::self()->log(i18n( "Internal error: No Y scale type checked in %1." ).arg(_title->text()), KstDebug::Error);
    }
    plotExtra->setDirty();
  }
}


void View2DPlotWidget::addPlotMarker() {
  // silently do nothing if there is no text, to
  // be consistent with "Add" button disabled
  if (!NewPlotMarker->text().isEmpty()) {
    bool ok;
    double newMarkerVal = NewPlotMarker->text().toDouble(&ok);

    if (ok) {
      uint i = 0;
      QString stringnum;

      stringnum.setNum(newMarkerVal, 'g', MARKER_LABEL_PRECISION);
      while (i < PlotMarkerList->count() && PlotMarkerList->text(i).toDouble() < newMarkerVal) {
        i++;
      }
      if (i == PlotMarkerList->count()) {
        PlotMarkerList->insertItem(stringnum, -1);
        NewPlotMarker->clear();
      } else if (newMarkerVal != PlotMarkerList->text(i).toDouble()) {
        PlotMarkerList->insertItem(stringnum, i);
        NewPlotMarker->clear();
      } else {
        KMessageBox::sorry(this,
                           i18n("A plot marker with equal (or very close) value already exists."),
                           i18n("Kst"));
        NewPlotMarker->selectAll();
      }
    } else {
      KMessageBox::sorry(this,
                         i18n("The text you have entered is not a valid number."),
                         i18n("Kst"));
      NewPlotMarker->selectAll();
    }
  }
}


void View2DPlotWidget::removePlotMarker() {
  uint count = PlotMarkerList->count();
  if (count > 0) {
    for (int i = count-1; i >= 0; i--) {
      if (PlotMarkerList->isSelected(i)) {
        PlotMarkerList->removeItem(i);
      }
    }
    updateButtons();
  }
}


void View2DPlotWidget::removeAllPlotMarkers() {
  uint count = PlotMarkerList->count();
  if (count > 0) {
    for (int i = count-1; i >= 0; i--) {
      PlotMarkerList->removeItem(i);
    }
    updateButtons();
  }
}


void View2DPlotWidget::applyPlotMarkers(Kst2DPlotPtr plot) {
  Kst2DPlotList plots;

  if (markersThisPlot->isChecked()) {
    plots += plot;
  } else if (markersThisWindow->isChecked()) {
    plots = plot->topLevelParent()->findChildrenType<Kst2DPlot>(true);
  } else {
    plots = plot->globalPlotList();
  }

  KstMarker marker;
  KstMarkerList newMarkers;
  marker.isRising = false;
  marker.isFalling = false;
  marker.isVectorValue = false;
  for (unsigned i = 0; i < PlotMarkerList->count(); ++i) {
    marker.value = PlotMarkerList->text(i).toDouble();
    // FIXME: this is very broken.  you can't search for i18n() substrings!
    if (PlotMarkerList->text(i).find( i18n("rising")) == -1 &&
        PlotMarkerList->text(i).find( i18n("falling")) == -1 &&
        PlotMarkerList->text(i).find( i18n("value")) == -1) {
      newMarkers.append(marker);
    }
  }

  for (unsigned i = 0; i < plots.size(); i++) {
    Kst2DPlotPtr plotExtra = plots[i];
    plotExtra->setPlotMarkerList(newMarkers);

    // apply the auto-generation settings
    if (UseCurve->isChecked()) {
      KstBaseCurveList curves = kstObjectSubList<KstDataObject, KstBaseCurve>(KST::dataObjectList);
      KstVCurveList vcurves = kstObjectSubList<KstBaseCurve, KstVCurve>(curves);
      KstVCurvePtr curve = *(vcurves.findTag(CurveCombo->currentText()));
      bool falling = Falling->isChecked();
      bool rising = Rising->isChecked();

      if (Both->isChecked()) {
        falling = true;
        rising = true;
      }
      plotExtra->setCurveToMarkers(curve, rising, falling);
    } else {
      plotExtra->removeCurveToMarkers();
    }

    if (UseVector->isChecked()) {
      KstVectorPtr vector = *(KST::vectorList.findTag(_vectorForMarkers->selectedVector()));
      plotExtra->setVectorToMarkers(vector);
    } else {
      plotExtra->removeVectorToMarkers();
    }

    plotExtra->setLineStyleMarkers(_comboMarkerLineStyle->currentIndex());
    plotExtra->setLineWidthMarkers(_spinBoxMarkerLineWidth->value());
    plotExtra->setColorMarkers(_colorMarker->color());
    plotExtra->setDefaultColorMarker(_checkBoxDefaultMarkerColor->isChecked());
    plotExtra->setDirty();
  }
}


void View2DPlotWidget::fillPlot( Kst2DPlotPtr plot ) {

  applyAppearance(plot);

  // FIXME: be more efficient here.  Only remove the curves that we need, only
  //        add the curves that we need
  // add the curves
  KstBaseCurveList curves = kstObjectSubList<KstDataObject, KstBaseCurve>(KST::dataObjectList);
  plot->clearCurves();
  for (unsigned i = 0; i < DisplayedCurveList->count(); i++) {
    KstBaseCurveList::Iterator it = curves.findTag(DisplayedCurveList->text(i));
    if (it != curves.end()) {
      plot->addCurve(*it);
    }
  }
  curves.clear();
  applyXAxis(plot);
  applyYAxis(plot);
  applyRange(plot);
  applyPlotMarkers(plot);

  //_title->setText(plot->tagName());
  QString tag = _title->text().simplified();
  if (tag.isEmpty()) {
    plot->setTagName(KstObjectTag(KST::suggestPlotName(), KstObjectTag::globalTagContext)); // FIXME: always global tag context?
  } else {
    plot->setTagName(KstObjectTag(tag+"randomtextasholdingpattern", KstObjectTag::globalTagContext)); // FIXME: always global tag context?
    if (KstData::self()->viewObjectNameNotUnique(tag)) {
      int j = 1;
      while (KstData::self()->viewObjectNameNotUnique(tag+"-"+QString::number(j))) {
        j++;
      }
      tag = tag + "-" + QString::number(j);
    }
    plot->setTagName(KstObjectTag(tag, KstObjectTag::globalTagContext));   // FIXME: always global tag context?
  }

  plot->setDirty();

  /*
  // make sure we paint all the necessary windows
  if (appearanceAll->isChecked() ||
      XAxisAll->isChecked() ||
      YAxisAll->isChecked() ||
      rangeAll->isChecked() ||
      markersAll->isChecked()) {
    KMdiIterator<KMdiChildView*> *it = KstApp::inst()->createIterator();
    while (it->currentItem()) {
      KstViewWindow *win = dynamic_cast<KstViewWindow*>(it->currentItem());
      if (win) {
        win->view()->paint(KstPainter::P_PLOT);
      }
      it->next();
    }
    KstApp::inst()->deleteIterator(it);
  } else {
    static_cast<KstViewWindow*>(c)->view()->paint(KstPainter::P_PLOT);
  }

  _plotName = _title->text().simplified();
  update();
  emit docChanged();
  */
}


void View2DPlotWidget::insertCurrentScalar() {
  ScalarDest->insert(ScalarList->currentText());
}


void View2DPlotWidget::setScalarDestXLabel() {
  ScalarDest = XAxisText;
}


void View2DPlotWidget::setScalarDestYLabel() {
  ScalarDest = YAxisText;
}


void View2DPlotWidget::setScalarDestTopLabel() {
  ScalarDest = TopLabelText;
}


void View2DPlotWidget::editLegend() {
  KstTopLevelViewPtr tlv = kst_cast<KstTopLevelView>(_plot->topLevelParent());

  _plot->getOrCreateLegend()->showDialog(tlv, false);
  ShowLegend->setChecked(true);
}

#include "view2dplotwidget.moc"

// vim: ts=2 sw=2 et
