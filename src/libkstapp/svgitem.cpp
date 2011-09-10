/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "svgitem.h"
#include "debug.h"
#include "dialogdefaults.h"

#include <QDebug>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QSvgRenderer>


namespace Kst {

SvgItem::SvgItem(View *parent, const QString &file)
  : ViewItem(parent) {

  if (!file.isNull()) {
    _svg = new QSvgRenderer(file);
    QFile svgfile(file);
    if (svgfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      while (!svgfile.atEnd()) {
          _svgData.append(svgfile.readLine());
      }
    }
  } else {
    _svg = new QSvgRenderer();
  }
  setTypeName("Svg");
  setLockAspectRatio(true);
  setLockAspectRatioFixed(true);
}


SvgItem::~SvgItem() {
}


void SvgItem::paint(QPainter *painter) {
  // We can do better here.  Cache the svg also.
  if (_svg->isValid() && rect().isValid()) {
    _svg->render(painter, rect());
  }
}


void SvgItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("svg");
    ViewItem::save(xml);
    xml.writeStartElement("data");
    xml.writeCharacters(qCompress(_svgData).toBase64());
    xml.writeEndElement();
    xml.writeEndElement();
  }
}


void SvgItem::creationPolygonChanged(View::CreationEvent event) {

  double aspect = 1.0;
  if ((_svg->defaultSize().width()>0) && (_svg->defaultSize().height()>0)) {
    aspect = double(_svg->defaultSize().width())/double(_svg->defaultSize().height());
  }

  creationPolygonChangedFixedAspect(event, aspect);

}



void SvgItem::setSvgData(const QByteArray &svgData) {
  _svg->load(svgData);
  _svgData = svgData;
}

void CreateSvgCommand::createItem() {
  QString start_dir = _dialogDefaults->value("svg/startdir", ".").toString();
  QString filter = "SVG Images (*.svg *.SVG)";
  QString file = QFileDialog::getOpenFileName(_view, tr("Kst: Open SVG Image"), start_dir, filter);

  if (file.isEmpty())
    return;

  _dialogDefaults->setValue("svg/startdir", QFileInfo(file).path());
  _item = new SvgItem(_view, file);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}


SvgItemFactory::SvgItemFactory()
: GraphicsFactory() {
  registerFactory("svg", this);
}


SvgItemFactory::~SvgItemFactory() {
}


ViewItem* SvgItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  SvgItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (!rc && xml.name().toString() == "svg") {
        Q_ASSERT(!rc);
        rc = new SvgItem(view);
        if (parent) {
          rc->setParentViewItem(parent);
        }
        // Add any new specialized SvgItem Properties here.
      } else if (xml.name().toString() == "data") {
        Q_ASSERT(rc);
        xml.readNext();
        QByteArray qbca = QByteArray::fromBase64(xml.text().toString().toLatin1());
        rc->setSvgData(qUncompress(qbca));
        xml.readNext();
        if (!xml.isEndElement() || (xml.name().toString() != "data")) {
          validTag = false;
        }
        xml.readNext();
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "svg") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating svg object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }
  return rc;
}


}

// vim: ts=2 sw=2 et
