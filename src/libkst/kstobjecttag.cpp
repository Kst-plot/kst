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

#include "kstobjecttag.h"

KstObjectTag::KstObjectTag()
  : _minDisplayComponents(0), _uniqueDisplayComponents(UINT_MAX) {
}


KstObjectTag::KstObjectTag(const QString& tag, const QStringList& context,
                           unsigned int minDisplayComponents)
  : _tag(cleanTag(tag)),
    _context(context),
    _minDisplayComponents(minDisplayComponents),
    _uniqueDisplayComponents(UINT_MAX) {
}


KstObjectTag::KstObjectTag(const QString& tag, const KstObjectTag& contextTag,
                           bool alwaysShowContext)
  : _uniqueDisplayComponents(UINT_MAX) {

  _tag = cleanTag(tag);
  _context = contextTag.fullTag();
  _minDisplayComponents = 1 + (alwaysShowContext ? qMax(contextTag._minDisplayComponents, (unsigned int)1) : 0);
}


KstObjectTag::KstObjectTag(const QStringList &fullTag)
  : _minDisplayComponents(1), _uniqueDisplayComponents(UINT_MAX) {

  QStringList context = fullTag;
  _tag = cleanTag(context.last());
  context.pop_back();
  _context = context;
}


QString KstObjectTag::tag() const {
  return _tag;
}


QStringList KstObjectTag::fullTag() const {
  QStringList ft(_context);
  ft << _tag;
  return ft;
}


QStringList KstObjectTag::context() const {
  return _context;
}


unsigned int KstObjectTag::components() const {
  if (!isValid()) {
    return 0;
  } else {
    return 1 + _context.count();
  }
}


void KstObjectTag::setTag(const QString& tag) {
  _tag = cleanTag(tag);
  _uniqueDisplayComponents = UINT_MAX;
}


void KstObjectTag::setContext(const QStringList& context) {
  _context = context;
  _uniqueDisplayComponents = UINT_MAX;
}


void KstObjectTag::setTag(const QString& tag, const QStringList& context) {
  setTag(tag);
  setContext(context);
}


bool KstObjectTag::isValid() const {
  return !_tag.isEmpty();
}


QString KstObjectTag::tagString() const {
  return fullTag().join(tagSeparator);
}


void KstObjectTag::setUniqueDisplayComponents(unsigned int n) {
  _uniqueDisplayComponents = n;
}


unsigned int KstObjectTag::uniqueDisplayComponents() const {
  return _uniqueDisplayComponents;
}


void KstObjectTag::setMinDisplayComponents(unsigned int n) {
  _minDisplayComponents = n;
}


QStringList KstObjectTag::displayFullTag() const {
  QStringList out_tag = _context + QStringList(_tag);
  int componentsToDisplay = qMin(qMax(_uniqueDisplayComponents, _minDisplayComponents), components());
  while (out_tag.count() > componentsToDisplay) {
    out_tag.pop_front();
  }
  return out_tag;
}


QString KstObjectTag::displayString() const {
  return displayFullTag().join(tagSeparator);
}


KstObjectTag KstObjectTag::fromString(const QString& str) {
  QStringList l = str.split(tagSeparator);
  if (l.isEmpty()) {
    return invalidTag;
  }

  QString t = l.last();
  l.pop_back();
  return KstObjectTag(t, l);
}


bool KstObjectTag::operator==(const KstObjectTag& tag) const {
  return (_tag == tag._tag && _context == tag._context);
}


bool KstObjectTag::operator!=(const KstObjectTag& tag) const {
  return (_tag != tag._tag || _context != tag._context);
}


QString KstObjectTag::cleanTag(const QString& in_tag) {
  if (in_tag.contains(tagSeparator)) {
    QString tag = in_tag;
    tag.replace(tagSeparator, tagSeparatorReplacement);
//        kstdWarning() << "cleaning tag name containing " << tagSeparator << ":\"" << in_tag << "\" -> \"" << tag << "\"" << endl;
    return tag;
  } else {
    return in_tag;
  }
}

// vim: ts=2 sw=2 et
