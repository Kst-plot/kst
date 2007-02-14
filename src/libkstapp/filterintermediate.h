/***************************************************************************
                            filterintermediate.h
                             -------------------
    begin                : Dec 11 2003
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

#ifndef FILTERINTERMEDIATE_H
#define FILTERINTERMEDIATE_H



class FilterListBoxItem : public QListBoxText {
  public:
    FilterListBoxItem(QListBox *p, const QString& filter) : QListBoxText(p, filter), _filter(filter) {}
    virtual ~FilterListBoxItem() {}

    void updateText() {
      QString txt = _filter + "(";
      for (KstScalarMap::ConstIterator it = arguments.begin(); it != arguments.end(); ++it) {
        if (it != arguments.begin()) {
          txt += ", ";
        }
        if (it.data()) {
          if (it.data()->isGlobal()) {
            txt += it.data()->tagName();
          } else {
            txt += QString::number(it.data()->value());
          }
        }
      }
      txt += ")";
      setText(txt);
    }

    KstScalarMap arguments;
    QString _filter;
};

#endif

// vim: ts=2 sw=2 et
