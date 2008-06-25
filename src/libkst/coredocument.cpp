/***************************************************************************
                              coredocument.cpp
                             -------------------
    begin                : October 3, 2007
    copyright            : (C) 2007 by The University of Toronto
    email                :
 ***************************************************************************/

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

#include "coredocument.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

#include "objectstore.h"

namespace Kst {

CoreDocument::CoreDocument()
: _objectStore(new ObjectStore()), _dirty(false), _isOpen(false), _fileName(QString::null) {
}


CoreDocument::~CoreDocument() {
}


QString CoreDocument::fileName() const {
  return _fileName;
}


ObjectStore* CoreDocument::objectStore() const {
  return _objectStore;
}


bool CoreDocument::save(const QString& to) {
  Q_UNUSED(to);
#if 0
  // TODO:
  // - KSaveFile-ish behavior
  // - only save if changed
  // - only setChanged(false) if save was successful
  setChanged(false);

  QString file = !to.isEmpty() ? to : _fileName;
  QFile f(file);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    _lastError = QObject::tr("File could not be opened for writing.");
    return false;
  }

  _fileName = file;

  QXmlStreamWriter xml;
  xml.setDevice(&f);
  xml.setAutoFormatting(true);
  xml.writeStartDocument();
  xml.writeStartElement("kst");
  xml.writeAttribute("version", "2.0");

  xml.writeStartElement("data");
  foreach (KstDataSourcePtr s, dataSourceList) {
    s->saveSource(xml);
  }
  xml.writeEndElement();

  xml.writeStartElement("variables");

  foreach (VectorPtr s, vectorList.list()) {
    s->save(xml);
  }
  foreach (KstMatrixPtr s, matrixList.list()) {
    s->save(xml);
  }
  foreach (ScalarPtr s, scalarList.list()) {
    s->save(xml);
  }
  foreach (KstStringPtr s, stringList.list()) {
    s->save(xml);
  }
  xml.writeEndElement();

  xml.writeStartElement("objects");
  foreach (DataObjectPtr s, KST::dataObjectList) {
    s->save(xml);
  }
  xml.writeEndElement();

  xml.writeStartElement("relations");
  foreach (KstRelationPtr s, KST::relationList) {
    s->save(xml);
  }
  xml.writeEndElement();

  xml.writeStartElement("graphics");
  for (int i = 0; i < _win->tabWidget()->count(); ++i) {
    View *v = qobject_cast<View*>(_win->tabWidget()->widget(i));
    xml.writeStartElement("view");
    xml.writeAttribute("name", _win->tabWidget()->tabText(i));
    // TODO: save each item
    xml.writeEndElement();
  }
  xml.writeEndElement();

  xml.writeEndCoreDocument();
#endif
  return true;
}


bool CoreDocument::open(const QString& file) {
  Q_UNUSED(file);
  _isOpen = false;
#if 0
  QFile f(file);
  if (!f.open(QIODevice::ReadOnly)) {
    _lastError = QObject::tr("File could not be opened for reading.");
    return false;
  }

  _fileName = file;

  View *currentView = 0;

  QXmlStreamReader xml;
  xml.setDevice(&f);

  enum State { Unknown=0, Data, Variables, Objects, Relations, Graphics, View };
  State state = Unknown;

#define malformed()

  while (!xml.atEnd()) {
    if (xml.isStartElement()) {
      QString n = xml.name().toString();
      if (n == "kst") {
      } else if (n == "data") {
        if (state != Unknown)
          malformed();
        state = Data;
      } else if (n == "variables") {
        if (state != Unknown)
          malformed();
        state = Variables;
      } else if (n == "objects") {
        if (state != Unknown)
          malformed();
        state = Objects;
      } else if (n == "relations") {
        if (state != Unknown)
          malformed();
        state = Relations;
      } else if (n == "graphics") {
        if (state != Unknown)
          malformed();
        state = Graphics;
      } else {
        switch (state) {
          case Objects:
            {
              DataObjectPtr object = ObjectFactory::parse(xml);
              if (object)
                KST::addDataObjectToList(object);
              else
                malformed();
              break;
            }
          case Graphics:
            {
              if (n == "view") {
                currentView = _win->tabWidget()->createView();
                QXmlStreamAttributes attrs = xml.attributes();
                QStringRef nm = attrs.value("name");
                if (!nm.isNull()) {
                  int idx = _win->tabWidget()->indexOf(currentView);
                  _win->tabWidget()->setTabText(idx, nm.toString());
                }
                state = View;
              } else {
                malformed();
              }
            }
            break;
          case View:
            {
              ViewItem *i = GraphicsFactory::parse(xml, currentView);
              if (i) {
                currentView->scene()->addItem(i);
              }
            }
            break;
          case Data:
            DataSourceFactory::parse(xml);
            break;
          case Variables:
            PrimitiveFactory::parse(xml);
            break;
          case Relations:
            RelationFactory::parse(xml);
            break;
          case Unknown:
            malformed();
            break;
        }
      }
    } else if (xml.isEndElement()) {
      QString n = xml.name().toString();
      if (n == "kst") {
        if (state != Unknown)
          malformed();
        break;
      } else if (n == "view") {
        state = Graphics;
      } else if (n == "data") {
        state = Unknown;
      } else if (n == "objects") {
        state = Unknown;
      } else if (n == "variables") {
        state = Unknown;
      } else if (n == "relations") {
        state = Unknown;
      } else if (n == "graphics") {
        state = Unknown;
      }
    }
    xml.readNext();
  }
#undef malformed

  if (xml.hasError()) {
    _lastError = QObject::tr("File is malformed and encountered an error while reading.");
    return false;
  }

#endif

  return _isOpen = true;
}


QString CoreDocument::lastError() const {
  return _lastError;
}


bool CoreDocument::isChanged() const {
  return _dirty;
}


bool CoreDocument::isOpen() const {
  return _isOpen;
}


void CoreDocument::setChanged(bool dirty) {
  _dirty = dirty;
}


}

// vim: ts=2 sw=2 et
