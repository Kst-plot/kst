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

#include "objecttag.h"
#include "math_kst.h"

namespace Kst {

/** Tag globals */
const QChar ObjectTag::tagSeparator = QChar('/');
const QChar ObjectTag::tagSeparatorReplacement = QChar('_');

const QStringList ObjectTag::globalTagContext = QStringList();
const QStringList ObjectTag::constantTagContext = QStringList("CONSTANTS");
const QStringList ObjectTag::orphanTagContext = QStringList();

const ObjectTag ObjectTag::invalidTag = ObjectTag(QString::null, ObjectTag::globalTagContext);


ObjectTag::ObjectTag()
  : _minDisplayComponents(0), _uniqueDisplayComponents(UINT_MAX) {
}


ObjectTag::ObjectTag(const QString& name, const QStringList& context,
                           unsigned int minDisplayComponents)
  : _name(cleanTagComponent(name)),
    _context(context),
    _minDisplayComponents(minDisplayComponents),
    _uniqueDisplayComponents(UINT_MAX) {
}


ObjectTag::ObjectTag(const QString& name, const ObjectTag& contextTag,
                           bool alwaysShowContext)
  : _uniqueDisplayComponents(UINT_MAX) {

  _name = cleanTagComponent(name);
  _context = contextTag.fullTag();
  _minDisplayComponents = 1 + (alwaysShowContext ? qMax(contextTag._minDisplayComponents, (unsigned int)1) : 0);
}


ObjectTag::ObjectTag(const QStringList &fullTag)
  : _minDisplayComponents(1), _uniqueDisplayComponents(UINT_MAX) {

  if (fullTag.isEmpty()) {
    _minDisplayComponents = 0;
  } else {
    QStringList context = fullTag;
    _name = cleanTagComponent(context.last());
    context.pop_back();
    _context = context;
  }
}


QString ObjectTag::name() const {
  return _name;
}


QStringList ObjectTag::fullTag() const {
  QStringList ft(_context);
  if (!_name.isEmpty()) {
    ft << _name;
  }
  return ft;
}


QStringList ObjectTag::context() const {
  return _context;
}


unsigned int ObjectTag::components() const {
  if (!isValid()) {
    return 0;
  } else {
    return 1 + _context.count();
  }
}


void ObjectTag::setName(const QString& name) {
  _name = cleanTagComponent(name);
  _uniqueDisplayComponents = UINT_MAX;
}


void ObjectTag::setContext(const QStringList& context) {
  _context = context;
  _uniqueDisplayComponents = UINT_MAX;
}


void ObjectTag::setTag(const QString& name, const QStringList& context) {
  setName(name);
  setContext(context);
}


bool ObjectTag::isValid() const {
  return !_name.isEmpty();
}


QString ObjectTag::tagString() const {
  return fullTag().join(tagSeparator);
}


void ObjectTag::setUniqueDisplayComponents(unsigned int n) {
  _uniqueDisplayComponents = n;
}


unsigned int ObjectTag::uniqueDisplayComponents() const {
  return _uniqueDisplayComponents;
}


void ObjectTag::setMinDisplayComponents(unsigned int n) {
  _minDisplayComponents = n;
}


QStringList ObjectTag::displayFullTag() const {
  QStringList out_tag = _context + QStringList(_name);
  int componentsToDisplay = qMin(qMax(_uniqueDisplayComponents, _minDisplayComponents), components());
  while (out_tag.count() > componentsToDisplay) {
    out_tag.pop_front();
  }
  return out_tag;
}


QString ObjectTag::displayString() const {
  return displayFullTag().join(tagSeparator);
}


ObjectTag ObjectTag::fromString(const QString& str) {
  QStringList l = str.split(tagSeparator);
  if (l.isEmpty()) {
    return invalidTag;
  }

  QString t = l.last();
  l.pop_back();
  return ObjectTag(t, l);
}


bool ObjectTag::operator==(const ObjectTag& tag) const {
  return (_name == tag._name && _context == tag._context);
}


bool ObjectTag::operator!=(const ObjectTag& tag) const {
  return (_name != tag._name || _context != tag._context);
}


QString ObjectTag::cleanTagComponent(const QString& component) {
  if (component.contains(tagSeparator)) {
    QString s = component;
    s.replace(tagSeparator, tagSeparatorReplacement);
//        kstdWarning() << "cleaning tag component containing " << tagSeparator << ":\"" << component << "\" -> \"" << s << "\"" << endl;
    return s;
  } else {
    return component;
  }
}


uint qHash(const ObjectTag& tag) {
  // FIXME: better hash function
//  return qHash(tag._name) + qHash(tag._context);
  return qHash(tag.name());
}


}

// vim: ts=2 sw=2 et
