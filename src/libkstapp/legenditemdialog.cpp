/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "legenditemdialog.h"

#include "legendtab.h"
#include "dialogpage.h"
#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

#include "curve.h"
#include "curvedialog.h"
#include "plotrenderitem.h"
#include "plotitem.h"
#include "image.h"
#include "imagedialog.h"

namespace Kst {

LegendItemDialog::LegendItemDialog(LegendItem *item, QWidget *parent)
    : ViewItemDialog(item, parent), _legendItem(item) {

  _store = kstApp->mainWindow()->document()->objectStore();

  _legendTab = new LegendTab(this);
  connect(_legendTab, SIGNAL(apply()), this, SLOT(legendChanged()));

  DialogPage *page = new DialogPage(this);
  page->setPageTitle(tr("Legend"));
  page->addDialogTab(_legendTab);
  addDialogPage(page);

  setupLegend();

  setSupportsMultipleEdit(true);

  QList<LegendItem *> legends = ViewItem::getItems<LegendItem>();

  clearMultipleEditOptions();
  foreach(LegendItem* legend, legends) {
    addMultipleEditOption(legend->Name(), legend->descriptionTip(), legend->shortName());
  }

  _saveAsDefault->show();
  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultiple()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingle()));
  connect(this, SIGNAL(apply()), this, SLOT(slotApply()));
}


LegendItemDialog::~LegendItemDialog() {
}

void LegendItemDialog::slotApply() {
  //FIXME: it is not clear that slotApply must be executed last.
  // experimentally, it seems to be...
  if (_saveAsDefault->isChecked()) {
    _legendItem->saveAsDialogDefaults();
  }
}


void LegendItemDialog::setupLegend() {
  QStringList displayedRelations;
  QStringList availableRelations;
  QStringList allRelations;
  QStringList displayedRelationTips;
  QStringList availableRelationTips;
  QStringList allRelationTips;

  CurveList curves = _store->getObjects<Curve>();
  ImageList images = _store->getObjects<Image>();

  if (_legendItem->relations().isEmpty()) {
    foreach (RelationPtr relation, _legendItem->plot()->renderItem(PlotRenderItem::Cartesian)->relationList()) {
      displayedRelations.append(relation->Name());
      displayedRelationTips.append(relation->descriptionTip());
    }
  } else {
    foreach (RelationPtr relation, _legendItem->relations()) {
      displayedRelations.append(relation->Name());
      displayedRelationTips.append(relation->descriptionTip());
    }
  }

  foreach (CurvePtr curve, curves) {
    allRelations.append(curve->Name());
    allRelationTips.append(curve->descriptionTip());
    if (!displayedRelations.contains(curve->Name())) {
      availableRelations.append(curve->Name());
      availableRelationTips.append(curve->descriptionTip());
    }
  }

  foreach (ImagePtr image, images) {
    allRelations.append(image->Name());
    allRelationTips.append(image->descriptionTip());
    if (!displayedRelations.contains(image->Name())) {
      availableRelations.append(image->Name());
      availableRelationTips.append(image->descriptionTip());
    }
  }

  _legendTab->setDisplayedRelations(displayedRelations, displayedRelationTips);
  _legendTab->setAvailableRelations(availableRelations, availableRelationTips);

  _legendTab->setFont(_legendItem->font());
  _legendTab->setFontScale(_legendItem->fontScale());
  _legendTab->setTitle(_legendItem->title());
  _legendTab->setAutoContents(_legendItem->autoContents());
  _legendTab->setVerticalDisplay(_legendItem->verticalDisplay());

}

void LegendItemDialog::editMultiple() {
  _legendTab->clearTabValues();
  _legendTab->setSingle(false);
}

void LegendItemDialog::editSingle() {
  _legendTab->setSingle(true);
  setupLegend();
}

void LegendItemDialog::legendChanged() {
  if (editMode() == Multiple) {
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      LegendItem* legendItem = (LegendItem*)item;
      saveLegend(legendItem, false);
    }
  } else {
    saveLegend(_legendItem, true);
  }
}

void LegendItemDialog::saveLegend(LegendItem *legendItem, bool save_relations) {
  legendItem->setFont(_legendTab->font(legendItem->font()));
  legendItem->setFontScale(_legendTab->fontScale());
  legendItem->setTitle(_legendTab->title());
  legendItem->setAutoContents(_legendTab->autoContents());
  legendItem->setVerticalDisplay(_legendTab->verticalDisplay());

  QStringList displayedRelations = _legendTab->displayedRelations();

  if (save_relations) {
    RelationList newRelations;
    foreach (QString relationName, displayedRelations) {
      if (RelationPtr relation = kst_cast<Relation>(_store->retrieveObject(relationName))) {
        newRelations.append(relation);
      }
    }
    legendItem->setRelations(newRelations);
  }
}

}

// vim: ts=2 sw=2 et
