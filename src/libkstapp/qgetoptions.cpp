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

#include "qgetoptions.h"

#include <QDebug>
#include <QCoreApplication>

QOption::QOption()
    : _name(QString::null), _type(Argument), _value(0) {
}


QOption::QOption(const QString &name, Type type, QVariant *value)
    : _name(name), _type(type), _value(value) {
}


QString QOption::name() const {
  return _name;
}


QOption::Type QOption::type() const {
  return _type;
}


QVariant *QOption::value() const {
  return _value;
}


QGetOptions::QGetOptions() {
}


QGetOptions::~QGetOptions() {
}

void QGetOptions::addOption(const QString &name, QOption::Type type, QVariant *value) {

  QOption option(name, type, value);

  switch (type) {
  case QOption::Argument:
    _arguments.insert(name, option); break;
  case QOption::Switch:
    _switches.insert(name, option); break;
  case QOption::Option:
    _options.insert(name, option); break;
  default:
    break;
  }
}

void QGetOptions::getValues() {

  Q_ASSERT(QCoreApplication::instance());
  QStringList arguments = QCoreApplication::instance()->arguments();
  arguments.takeFirst(); //appname

  foreach (QString str, arguments) {
    getNextValue(str);
  }

}

bool QGetOptions::getNextValue(const QString &argument) {

  QString arg = argument;

  if (arg.startsWith("--")) {

    arg.replace("--", ""); //get rid of the switch token

    if (arg.contains("=")) {

      QStringList opt = arg.split("=", QString::SkipEmptyParts);

      Q_ASSERT(opt.count() == 2);

      if (_options.contains(opt[0])) {
          QOption option = _options.value(opt[0]);
          QVariant *value = option.value();
          *value = opt[1];
          return true;
      }

    } else {

      if (_switches.contains(arg)) {
        QOption option = _options.value(arg);
        QVariant *value = option.value();
        *value = true;
        return true;
      }

    }

  } else if (!_arguments.isEmpty()) {
    QOption option = _arguments.begin().value();
    QVariant *value = option.value();
    *value = arg;
    return true;
  }

  usage(); //something went wrong...
  return false;
}

void QGetOptions::usage() const {
  QStringList usage;
  usage << QString("Usage:");
  usage << QString("    %1 %2 %3").arg(QCoreApplication::applicationName(),
                                    (!_options.isEmpty() ? "[options]" : ""),
                                    QStringList(_arguments.keys()).join(" "));

  usage << QString("Options:");

  foreach (QString str, _switches.keys()) {
    usage << QString("    --%1").arg(str);
  }
  foreach (QString str, _options.keys()) {
    usage << QString("    --%1=%2").arg(str, "[value]");
  }

  qDebug() << usage.join("\n") << endl;
}

// vim: ts=2 sw=2 et
