/***************************************************************************
                     namedparameter.h  -
                             -------------------
    begin                : Oct 15 2010
    copyright            : (C) 2010 The University of Toronto
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
#ifndef KST_NAMED_PARAMETER
#define KST_NAMED_PARAMETER

#include <QSettings>
#include <QXmlStreamWriter>
#include <QXmlStreamAttributes>
#include <QDebug>



template<class T, const char* Key, const char* Tag>
class Parameter
{
public:
  // this is not nice, it sets not the value but its default
  Parameter(const T& default_value) :
      _default_value(default_value),
      _value_set(false) {
  }

  void operator>>(QSettings& settings) {
    settings.setValue(Key, value());
  }

  void operator<<(QSettings& settings) {
    const QVariant var = settings.value(Key);
    if (var.isNull())
      setValue(_default_value);
    else
      setValue(var.value<T>());
  }

  void operator>>(QXmlStreamWriter& xml) {
    xml.writeAttribute(Tag, QVariant(value()).toString());
  }

  void operator<<(QXmlStreamAttributes& att) {
    setValue(QVariant(att.value(Tag).toString()).value<T>());
  }

  void setValue(const T& t) {
    _value = t;
    _value_set = true;
  }

  const T& value() const {
    if (!_value_set)
      qDebug() << "Using unset value " << Key;
    return _value;
  }

  operator const T&() const {
    return value();
  }

  Parameter& operator=(const T& t) {
    setValue(t);
    return *this;
  }

private:
  T _value;
  T _default_value;
  bool _value_set;
};


#endif
