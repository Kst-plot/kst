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
#include "datasource.h"

namespace Kst {

class  KSTWIDGETS_EXPORT DataRange : public QWidget, public Ui::DataRange {
  Q_OBJECT
  public:
    explicit DataRange(QWidget *parent = 0);
    virtual ~DataRange();

    qreal start() const;
    bool startDirty() const;
    bool startIsDuration() const;
    bool startIsISOTime() const;
    void setStart(qreal start, bool callUpdateFields = true);

    QString startUnits() const;
    // int startUnitsIndex() const;
    void setStartUnits(const QString &startUnits);

    qreal range() const;
    bool rangeDirty() const;
    bool rangeIsDuration() const;
    void setRange(qreal range, bool callUpdateFields = true);

    qreal last() const;
    bool lastDirty() const;
    bool lastIsDuration() const;
    bool lastIsISOTime() const;
    void setLast(qreal last, bool callUpdateFields = true);

    void setDataSource(const DataSourcePtr &dataSource);
    DataSourcePtr dataSource() const;
    void setReferenceField(const QString &field);

    bool setFrameRange(qint64 startFrame, qint64 endFrameExclusive);
    bool setCountFromEndFrameCount(qint64 frameCount);

    void updateIndexList(const QList<Kst::IndexFieldProperties> &indexFields);
    void clearIndexList();

    QList<Kst::IndexFieldProperties> indexFieldProperties() const { return _indexFieldProps; }

    QString rangeUnits() const;
    // int rangeUnitsIndex() const;
    void setRangeUnits(const QString &rangeUnits);

    // helpers for current selection
    bool startIsFrame() const;
    bool startIsSeconds() const;
    bool startIsCTime() const;

    bool rangeIsFrame() const;
    bool rangeIsSeconds() const;
    bool rangeIsCTime() const;

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

    bool rangeIsValid();

    enum ControlField {Start, Last, Range, None};

    void updateFields(ControlField cf);
  Q_SIGNALS:
    void modified();
    void indexUnitsChanged();

  private Q_SLOTS:
    void countFromEndChanged();
    void readToEndChanged();
    void doSkipChanged();
    void startChanged();
    void lastChanged();
    void rangeChanged();
    void unitsChanged();
    void convertRangeToNumber();
    void convertRangeToDuration();
    void convertStartToNumber();
    void convertStartToDuration();
    void convertStartToISOTime();

  private:
    bool isFrameUnits(const QString &units);
    bool canConvertUnits(const QString &units);
    bool frameFromUnits(double value, const QString &units, bool roundUp, qint64 *frameOut);
    bool unitsFromFrame(qint64 frame, const QString &units, double *valueOut);
    bool displayCanonicalFrameRange();
    bool displayCanonicalCountFromEnd();
    qint64 availableFramesForReferenceField();
    bool frameCountFromEndRange(double value, const QString &units,
                  qint64 *frameCountOut);
    // int maxFrameForClamp(const QString &preferredUnits);

    QString _requestedRangeUnits;
    QString _requestedStartUnits;
    QString _displayedStartUnits;
    QString _displayedRangeUnits;
    ControlField _controlField0;
    ControlField _controlField1;
    QList<Kst::IndexFieldProperties> _indexFieldProps;
    DataSourcePtr _dataSource;
    QString _referenceField;
    bool _canonicalFrameRangeValid = false;
    qint64 _canonicalStartFrame = 0;
    qint64 _canonicalEndFrameExclusive = 0;
    bool _canonicalCountFromEndValid = false;
    qint64 _canonicalCountFromEnd = 0;
    bool _settingRangeModeProgrammatically = false;
};

}

#endif

// vim: ts=2 sw=2 et
