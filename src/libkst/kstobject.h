/***************************************************************************
              kstobject.h: abstract base class for all Kst objects
                             -------------------
    begin                : May 22, 2003
    copyright            : (C) 2003 The University of Toronto
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

#ifndef KSTOBJECT_H
#define KSTOBJECT_H

#include <qpointer.h>
#include <qmutex.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#include <kglobal.h>

#include "kst_export.h"
#include "kstsharedptr.h"
#include <qdebug.h>
#include "rwlock.h"

//We define two different keys for datasource VS dataobject plugins
//so that if the API for one changes, the other doesn't have to be
//updated also...
#define KST_CURRENT_DATASOURCE_KEY 0x00000006

#define KST_KEY_DATASOURCE_PLUGIN(x) extern "C" Q_UINT32 key_##x() { return KST_CURRENT_DATASOURCE_KEY; }

#define KST_CURRENT_DATAOBJECT_KEY 0x00000006

#define KST_KEY_DATAOBJECT_PLUGIN(x) extern "C" Q_UINT32 key_##x() { return KST_CURRENT_DATAOBJECT_KEY; }

class KstObjectPrivate;

// NOTE: In order to preserve binary compatibility with plugins, you must
//       update the plugin keys whenever you add, remove, or change member
//       variables or virtual functions, or when you remove or change
//       non-virtual functions.


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
      unsigned int componentsToDisplay = qMin(qMax(_uniqueDisplayComponents, _minDisplayComponents), components());
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
};


class KST_EXPORT KstObject : public KstShared, public QObject, public KstRWLock {
  public:
    KstObject();

    enum UpdateType { NO_CHANGE = 0, UPDATE };

    virtual UpdateType update(int updateCounter = -1) = 0;

    virtual QString tagName() const;
    virtual KstObjectTag& tag();
    virtual const KstObjectTag& tag() const;
    virtual void setTagName(const KstObjectTag& tag);

    virtual QString tagLabel() const;
    // Returns count - 2 to account for "this" and the list pointer, therefore
    // you MUST have a reference-counted pointer to call this function
    virtual int getUsage() const;

    // Returns true if update has already been done
    virtual bool checkUpdateCounter(int update_counter);

    int operator==(const QString&) const;

    virtual bool deleteDependents();

    // @since 1.1.0
    virtual void setDirty(bool dirty = true);
    // @since 1.1.0
    bool dirty() const;

  protected:
    virtual ~KstObject();

    friend class UpdateThread;
    int _lastUpdateCounter;

    // @since 1.1.0
    UpdateType setLastUpdateResult(UpdateType result);
    // @since 1.1.0
    UpdateType lastUpdateResult() const;

  private:
    KstObjectTag _tag;
    bool _dirty;
    KstObject::UpdateType _lastUpdate;
};

typedef KstSharedPtr<KstObject> KstObjectPtr;

#include <q3valuelist.h>
#include <q3deepcopy.h>

template<class T>
class KstObjectList : public Q3ValueList<T> {
  friend class Q3DeepCopy<KstObjectList<T> >;
  public:
    KstObjectList() : Q3ValueList<T>() {}
    KstObjectList(const KstObjectList<T>& x) : Q3ValueList<T>(x) {}
    virtual ~KstObjectList() { }

    KstObjectList& operator=(const KstObjectList& l) {
      this->Q3ValueList<T>::operator=(l);
      return *this;
    }

    virtual QStringList tagNames() {
      QStringList rc;
      for (typename Q3ValueList<T>::ConstIterator it = Q3ValueList<T>::begin(); it != Q3ValueList<T>::end(); ++it) {
        rc << (*it)->tagName();
      }
      return rc;
    }

    // @since 1.1.0
    QStringList tagNames() const {
      QStringList rc;
      for (typename Q3ValueList<T>::ConstIterator it = Q3ValueList<T>::begin(); it != Q3ValueList<T>::end(); ++it) {
        rc << (*it)->tagName();
      }
      return rc;
    }

    virtual typename Q3ValueList<T>::Iterator findTag(const QString& x) {
      for (typename Q3ValueList<T>::Iterator it = Q3ValueList<T>::begin(); it != Q3ValueList<T>::end(); ++it) {
        if (*(*it) == x) {
          return it;
        }
      }
      return Q3ValueList<T>::end();
    }

    virtual typename Q3ValueList<T>::ConstIterator findTag(const QString& x) const {
      for (typename Q3ValueList<T>::ConstIterator it = Q3ValueList<T>::begin(); it != Q3ValueList<T>::end(); ++it) {
        if (*(*it) == x) {
          return it;
        }
      }
      return Q3ValueList<T>::end();
    }

    virtual int findIndexTag(const QString& x) const {
      int i = 0;
      for (typename Q3ValueList<T>::ConstIterator it = Q3ValueList<T>::begin(); it != Q3ValueList<T>::end(); ++it) {
        if (*(*it) == x) {
          return i;
        }
        i++;
      }
      return -1;
    }

    virtual typename Q3ValueList<T>::Iterator removeTag(const QString& x) {
      typename Q3ValueList<T>::Iterator it = findTag(x);
      if (it != Q3ValueList<T>::end()) {
        return Q3ValueList<T>::remove(it);
      }
      return it;
    }

    KstRWLock& lock() const { return _lock; }

  private:
    mutable KstRWLock _lock;
};

/* Does locking for you automatically. */
template<class T, class S>
KstObjectList<KstSharedPtr<S> > kstObjectSubList(KstObjectList<KstSharedPtr<T> >& list) {
  list.lock().readLock();
  KstObjectList<KstSharedPtr<S> > rc;
  typename KstObjectList<KstSharedPtr<T> >::Iterator it;

  for (it = list.begin(); it != list.end(); ++it) {
    S *x = dynamic_cast<S*>((*it).data());
    if (x != 0L) {
      rc.append(x);
    }
  }

  list.lock().unlock();
  return rc;
}


/* Does locking for you automatically. */
template<class T, class S>
void kstObjectSplitList(KstObjectList<KstSharedPtr<T> >& list, KstObjectList<KstSharedPtr<S> >& inclusive, KstObjectList<KstSharedPtr<T> >& exclusive) {
  list.lock().readLock();
  typename KstObjectList<KstSharedPtr<T> >::Iterator it;

  for (it = list.begin(); it != list.end(); ++it) {
    S *x = dynamic_cast<S*>((*it).data());
    if (x != 0L) {
      inclusive.append(x);
    } else {
      exclusive.append(*it);
    }
  }

  list.lock().unlock();
}


#include <qmap.h>

template<class T>
class KstObjectMap : public QMap<QString,T> {
  public:
    KstObjectMap() : QMap<QString,T>() {}
    virtual ~KstObjectMap() {}

    virtual QStringList tagNames() {
      QStringList rc;
      for (typename QMap<QString,T>::ConstIterator it = QMap<QString,T>::begin(); it != QMap<QString,T>::end(); ++it) {
        rc << it.value()->tagName();
      }
      return rc;
    }

    // @since 1.1.0
    QStringList tagNames() const {
      QStringList rc;
      for (typename QMap<QString,T>::ConstIterator it = QMap<QString,T>::begin(); it != QMap<QString,T>::end(); ++it) {
        rc << it.value()->tagName();
      }
      return rc;
    }

    // Careful - sets key(value) == value->tagName();
    typename QMap<QString,T>::iterator insertObject(const T& value) {
      return QMap<QString,T>::insert(value->tagName(), value);
    }

    typename QMap<QString,T>::iterator findTag(const QString& tag) {
      typename QMap<QString,T>::iterator i;
      for (i = QMap<QString,T>::begin(); i != QMap<QString,T>::end(); ++i) {
        if (i.value()->tagName() == tag) {
          break;
        }
      }
      return i;
    }

/*
These are wrong.  We should not assume that key(x) == x->tagName().
    bool contains(const T& value) {
      return QMap<QString,T>::contains(value->tagName());
    }

    typename QMap<QString,T>::iterator find(const T& value) {
      return QMap<QString,T>::find(value->tagName());
    }

    void remove(const T& value) {
      QMap<QString,T>::remove(value->tagName());
    }

*/
};

template <typename T, typename U>
inline KstSharedPtr<T> kst_cast(KstSharedPtr<U> object) {
  return dynamic_cast<T*>(object.data());
}


#endif
// vim: ts=2 sw=2 et
