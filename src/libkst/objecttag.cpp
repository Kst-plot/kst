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

namespace Kst {

ObjectTag::ObjectTag()
  : _minDisplayComponents(0), _uniqueDisplayComponents(UINT_MAX) {
}


ObjectTag::ObjectTag(const QString& tag, const QStringList& context,
                           unsigned int minDisplayComponents)
  : _tag(cleanTag(tag)),
    _context(context),
    _minDisplayComponents(minDisplayComponents),
    _uniqueDisplayComponents(UINT_MAX) {
}


ObjectTag::ObjectTag(const QString& tag, const ObjectTag& contextTag,
                           bool alwaysShowContext)
  : _uniqueDisplayComponents(UINT_MAX) {

  _tag = cleanTag(tag);
  _context = contextTag.fullTag();
  _minDisplayComponents = 1 + (alwaysShowContext ? qMax(contextTag._minDisplayComponents, (unsigned int)1) : 0);
}


ObjectTag::ObjectTag(const QStringList &fullTag)
  : _minDisplayComponents(1), _uniqueDisplayComponents(UINT_MAX) {

  QStringList context = fullTag;
  _tag = cleanTag(context.last());
  context.pop_back();
  _context = context;
}


QString ObjectTag::tag() const {
  return _tag;
}


QStringList ObjectTag::fullTag() const {
  QStringList ft(_context);
  ft << _tag;
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


void ObjectTag::setTag(const QString& tag) {
  _tag = cleanTag(tag);
  _uniqueDisplayComponents = UINT_MAX;
}


void ObjectTag::setContext(const QStringList& context) {
  _context = context;
  _uniqueDisplayComponents = UINT_MAX;
}


void ObjectTag::setTag(const QString& tag, const QStringList& context) {
  setTag(tag);
  setContext(context);
}


bool ObjectTag::isValid() const {
  return !_tag.isEmpty();
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
  QStringList out_tag = _context + QStringList(_tag);
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
  return (_tag == tag._tag && _context == tag._context);
}


bool ObjectTag::operator!=(const ObjectTag& tag) const {
  return (_tag != tag._tag || _context != tag._context);
}


QString ObjectTag::cleanTag(const QString& in_tag) {
  if (in_tag.contains(tagSeparator)) {
    QString tag = in_tag;
    tag.replace(tagSeparator, tagSeparatorReplacement);
//        kstdWarning() << "cleaning tag name containing " << tagSeparator << ":\"" << in_tag << "\" -> \"" << tag << "\"" << endl;
    return tag;
  } else {
    return in_tag;
  }
}

}
// vim: ts=2 sw=2 et
