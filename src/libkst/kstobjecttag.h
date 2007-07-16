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

#ifndef KSTOBJECTTAG_H
#define KSTOBJECTTAG_H

class KstObjectTag {
  public:
    static const KstObjectTag invalidTag;

    static const QChar tagSeparator;
    static const QChar tagSeparatorReplacement;

    static const QStringList globalTagContext;
    static const QStringList constantTagContext;
    static const QStringList orphanTagContext;


    // construct a tag in a given context
    KstObjectTag(const QString& tag, const QStringList& context,
        unsigned int minDisplayComponents = 1) : _tag(cleanTag(tag)),
                                                 _context(context),
                                                 _minDisplayComponents(minDisplayComponents),
                                                 _uniqueDisplayComponents(UINT_MAX)
    {
    }

    // construct a tag in the context of another tag
    KstObjectTag(const QString& tag, const KstObjectTag& contextTag, bool alwaysShowContext = true) :
      _uniqueDisplayComponents(UINT_MAX)
    {
      _tag = cleanTag(tag);
      _context = contextTag.fullTag();
      _minDisplayComponents = 1 + (alwaysShowContext ? qMax(contextTag._minDisplayComponents, (unsigned int)1) : 0);
    }

    // construct a tag from a fullTag representation
    KstObjectTag(QStringList fullTag) : _minDisplayComponents(1), _uniqueDisplayComponents(UINT_MAX) {
      _tag = cleanTag(fullTag.last());
      fullTag.pop_back();
      _context = fullTag;
    }

    QString tag() const { return _tag; }
    QStringList fullTag() const { 
      QStringList ft(_context);
      ft << _tag;
      return ft;
    }
    QStringList context() const { return _context; }

    unsigned int components() const { 
      if (!isValid()) {
        return 0;
      } else {
        return 1 + _context.count();
      }
    }

    // change the tag, maintaining context
    void setTag(const QString& tag) {
      _tag = cleanTag(tag);
      _uniqueDisplayComponents = UINT_MAX;
    }

    // change the context
    void setContext(const QStringList& context) {
      _context = context;
      _uniqueDisplayComponents = UINT_MAX;
    }

    // change the tag and context
    void setTag(const QString& tag, const QStringList& context) {
      setTag(tag);
      setContext(context);
    }

    bool isValid() const { return !_tag.isEmpty(); }

    QString tagString() const { return fullTag().join(tagSeparator); }

    // display methods
    void setUniqueDisplayComponents(unsigned int n) {
      _uniqueDisplayComponents = n;
    }
    unsigned int uniqueDisplayComponents() const { return _uniqueDisplayComponents; }

    void setMinDisplayComponents(unsigned int n) {
      _minDisplayComponents = n;
    }

    QStringList displayFullTag() const { 
      QStringList out_tag = _context + QStringList(_tag);
      int componentsToDisplay = qMin(qMax(_uniqueDisplayComponents, _minDisplayComponents), components());
      while (out_tag.count() > componentsToDisplay) {
        out_tag.pop_front();
      }
      return out_tag;
    }

    QString displayString() const { 
      return displayFullTag().join(tagSeparator);
    }

    // factory for String representation
    static KstObjectTag fromString(const QString& str) {
      QStringList l = str.split(tagSeparator);
      if (l.isEmpty()) {
        return invalidTag;
      }

      QString t = l.last();
      l.pop_back();
      return KstObjectTag(t, l);
    }

    bool operator==(const KstObjectTag& tag) const {
      return (_tag == tag._tag && _context == tag._context);
    }

    bool operator!=(const KstObjectTag& tag) const {
      return (_tag != tag._tag || _context != tag._context);
    }

    static QString cleanTag(const QString& in_tag) {
      if (in_tag.contains(tagSeparator)) {
        QString tag = in_tag;
        tag.replace(tagSeparator, tagSeparatorReplacement);
//        kstdWarning() << "cleaning tag name containing " << tagSeparator << ":\"" << in_tag << "\" -> \"" << tag << "\"" << endl;
        return tag;
      } else {
        return in_tag;
      }
    }

  private:
    QString _tag;
    QStringList _context;
    unsigned int _minDisplayComponents; // minimum number of components to use in display tag
    unsigned int _uniqueDisplayComponents;  // number of components necessary for unique display tag
} KST_EXPORT;

#endif

// vim: ts=2 sw=2 et

