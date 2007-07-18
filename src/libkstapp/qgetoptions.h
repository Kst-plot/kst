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

#ifndef QGETOPTIONS_H
#define QGETOPTIONS_H

#include <QVariant>
#include <QHash>
#include <QStringList>

class QOption {
  public:
    enum Type { Argument, Switch, Option };

    QOption();
    QOption(const QString &name, Type type, QVariant *value);

    QString name() const;
    Type type() const;
    QVariant *value() const;

  private:
    QString _name;
    Type _type;
    QVariant *_value;
};

class QGetOptions
{
  public:
    QGetOptions();
    ~QGetOptions();

    void addOption(const QString &name, QOption::Type type, QVariant *value);
    void getValues();
    void usage() const;

  private:
    bool getNextValue(const QString &argument);

    QHash<QString, QOption> _arguments;
    QHash<QString, QOption> _switches;
    QHash<QString, QOption> _options;
};

#endif

// vim: ts=2 sw=2 et
