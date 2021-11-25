/*
    themedialog.h: A dialog to set brush, pen, and fonts for view objects.
    Copyright (C) 2011  Barth Netterfield <netterfield@astro.utoronto.ca>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "themedialog.h"

#include "filltab.h"
#include "stroketab.h"
#include "labelpropertiestab.h"
#include "dialogdefaults.h"
#include "colorsequence.h"

#include "plotitem.h"
#include "boxitem.h"
#include "circleitem.h"
#include "ellipseitem.h"
#include "arrowitem.h"
#include "labelitem.h"
#include "geticon.h"

namespace Kst {

ThemeDialog::ThemeDialog(QWidget *parent) : QDialog(parent) {

  setupUi(this);
  setWindowTitle(tr("Theme"));

  _fillTab = new FillTab();
  QVBoxLayout *vbox1 = new QVBoxLayout;
  vbox1->addWidget(_fillTab);
  _fillBox->setLayout(vbox1);

  _strokeTab = new StrokeTab();
  QVBoxLayout *vbox2 = new QVBoxLayout;
  vbox2->addWidget(_strokeTab);
  _strokeBox->setLayout(vbox2);

  connect(_buttonBox, SIGNAL(clicked(QAbstractButton*)),
          this, SLOT(buttonClicked(QAbstractButton*)));

  int h = fontMetrics().lineSpacing();

  _bold->setFixedWidth(h);
  _bold->setFixedHeight(h);
  _bold->setIcon(KstGetIcon("kst_bold"));
  _italic->setFixedWidth(h);
  _italic->setFixedHeight(h);
  _italic->setIcon(KstGetIcon("kst_italic"));
  _labelColor->setFixedWidth(h);
  _labelColor->setFixedHeight(h);

  // fill color combobox
  int color_offset = ColorSequence::self().offset();
  ColorSequence::self().setOffset(0);

  for (int i_color = 0; i_color < ColorSequence::self().count(); i_color++) {
    QPixmap pm = QPixmap(32,32);
    pm.fill(ColorSequence::self().entry(i_color));
    QIcon icon = QIcon(pm);
    _curveColor->addItem(icon, "");
  }
  ColorSequence::self().setOffset(color_offset);

  _applyToExisting->setChecked(true);
  _saveAsDefault->setChecked(true);
}

ThemeDialog::~ThemeDialog() {

}

void ThemeDialog::reset() {
  setFillTab();
  setStrokeTab();
  setFontTab();

  // reset the curve color combobox
  int i_color = dialogDefaults().value("curves/default_color", 0).toInt();
  if (i_color > _curveColor->count()) i_color = 0;
  _curveColor->setCurrentIndex(i_color);

}

void ThemeDialog::apply() {
  QBrush b = _fillTab->brush();
  QPen p = _strokeTab->pen();

  QStringList typesWithFill;
  QStringList typesWithStroke;

  typesWithFill.append(PlotItem::staticDefaultsGroupName());
  typesWithStroke.append(PlotItem::staticDefaultsGroupName());

  typesWithFill.append(BoxItem::staticDefaultsGroupName());
  typesWithStroke.append(BoxItem::staticDefaultsGroupName());

  typesWithFill.append(CircleItem::staticDefaultsGroupName());
  typesWithStroke.append(CircleItem::staticDefaultsGroupName());

  typesWithFill.append(EllipseItem::staticDefaultsGroupName());
  typesWithStroke.append(EllipseItem::staticDefaultsGroupName());

  typesWithFill.append(LegendItem::staticDefaultsGroupName());
  typesWithStroke.append(LegendItem::staticDefaultsGroupName());

  typesWithStroke.append(LineItem::staticDefaultsGroupName());
  typesWithStroke.append(ArrowItem::staticDefaultsGroupName());

  typesWithFill.append(View::staticDefaultsGroupName());

  if (_curveBox->isChecked()) {
    ColorSequence::self().setOffset(_curveColor->currentIndex());
  }

  if (_saveAsDefault->isChecked()) {
    if (_fillBox->isChecked()) {
      foreach(const QString &type, typesWithFill) {
        saveDialogDefaultsBrush(type,b);
      }
    }
    if (_strokeBox->isChecked()) {
      foreach(const QString &type, typesWithStroke) {
        saveDialogDefaultsPen(type,p);
      }
    }

    if (_fontBox->isChecked()) {
      QFont F = font();
      QColor C = _labelColor->color();
      PlotItem::saveDialogDefaultsFont(F, C);
      LabelItem::saveDialogDefaultsFont(F, C);
      LegendItem::saveDialogDefaultsFont(F, C);
    }

    if (_curveBox->isChecked()) {
      dialogDefaults().setValue("curves/default_color", _curveColor->currentIndex());
    }
  }

  if (_applyToExisting->isChecked()) {
    QList<ViewItem *> view_items = ViewItem::getItems<ViewItem>();

    foreach (ViewItem *item, view_items) {
      if (_strokeBox->isChecked()) {
        if (item->hasStroke()) {
          item->storePen(p); // fixme: plots don't get repainted
        }
      }
      if (_fillBox->isChecked()) {
        if (item->hasBrush()) {
          item->setBrush(b);
        }
      }
      if (_fontBox->isChecked()) {
        QFont F = font();
        QColor C = _labelColor->color();
        if (item->hasFont()) {
          item->setFont(F,C);
        }
      }
    }
    if (_fillBox->isChecked()) {
      QList<View*> views = kstApp->mainWindow()->tabWidget()->views();
      foreach (View *view, views) {
        view->setBackgroundBrush(b);
      }
    }
  }
}

void ThemeDialog::setFillTab() {
  //set the brush
  QBrush brush = dialogDefaultsBrush(PlotItem::staticDefaultsGroupName());

  _fillTab->initialize(&brush);
}


void ThemeDialog::setStrokeTab() {
  // set the pen
  QPen pen = dialogDefaultsPen(PlotItem::staticDefaultsGroupName());

  _strokeTab->initialize(&pen);
}


void ThemeDialog::setFontTab() {
  QFont font;
  font.fromString(dialogDefaults().value(PlotItem::staticDefaultsGroupName()+"/globalFontFamily",font.toString()).toString());
  QColor color;
  color = dialogDefaults().value(PlotItem::staticDefaultsGroupName()+"/globalFontColor", QColor(Qt::black)).value<QColor>();

  _family->setCurrentFont(font);
  _bold->setChecked(font.bold());
  _italic->setChecked(font.italic());
  _labelColor->setColor(color);
  _labelFontScale->setValue(dialogDefaults().value(PlotItem::staticDefaultsGroupName()+"/globalFontScale", 13).toDouble());
}

void ThemeDialog::buttonClicked(QAbstractButton *button) {
  QDialogButtonBox::StandardButton std = _buttonBox->standardButton(button);
  switch(std) {
  case QDialogButtonBox::Ok:
    apply();
    hide();
    break;
  case QDialogButtonBox::Apply:
    apply();
    break;
  case QDialogButtonBox::Cancel:
    emit cancel();
    break;
  default:
    break;
  }
}

QFont ThemeDialog::font() const {
  QFont f(_family->currentFont());
  f.setItalic(_italic->isChecked());
  f.setBold(_bold->isChecked());
  f.setPointSize(_labelFontScale->value());
  return f;
}

}
