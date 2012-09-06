/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DATARANGE_H
#define DATARANGE_H

#include <QWidget>
#include "ui_datarange.h"

#include "kstwidgets_export.h"

namespace Kst {

//FIXME Need to add time related methods/functionality

class  KSTWIDGETS_EXPORT DataRange : public QWidget, public Ui::DataRange {
  Q_OBJECT
  public:
    DataRange(QWidget *parent = 0);
    virtual ~DataRange();

    qreal start() const;
    bool startDirty() const;
    void setStart(qreal start);

    QString startUnits() const;
    int startUnitsIndex() const;
    void setStartUnits(const QString &startUnits);

    qreal range() const;
    bool rangeDirty() const;
    void setRange(qreal range);

    void updateIndexList(const QStringList &indexFields);
    void clearIndexList();

    //FIXME Probably should be an enum...
    QString rangeUnits() const;
    int rangeUnitsIndex() const;
    void setRangeUnits(const QString &rangeUnits);

    int skip() const;
    bool skipDirty() const;
    void setSkip(int skip);

    bool countFromEnd() const;
    bool countFromEndDirty() const;
    void setCountFromEnd(bool countFromEnd);

    bool readToEnd() const;
    bool readToEndDirty() const;
    void setReadToEnd(bool readToEnd);

    bool doSkip() const;
    bool doSkipDirty() const;
    void setDoSkip(bool doSkip);

    bool doFilter() const;
    bool doFilterDirty() const;
    void setDoFilter(bool doFilter);

    void clearValues();

    void setWidgetDefaults();
    void loadWidgetDefaults();

  Q_SIGNALS:
    void modified();

  private Q_SLOTS:
    void countFromEndChanged();
    void readToEndChanged();
    void doSkipChanged();

  private:
    QString _requestedRangeUnits;
    QString _requestedStartUnits;

};

}

#endif

// vim: ts=2 sw=2 et
