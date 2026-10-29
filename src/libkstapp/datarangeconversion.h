/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2026 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DATARANGECONVERSION_H
#define DATARANGECONVERSION_H

#include <QtMath>

#include <cmath>
#include <limits>

#include "datarange.h"
#include "datasource.h"

namespace Kst {
namespace DataRangeConversion {

inline qint64 floorFrame(double value) {
  if (!std::isfinite(value) || value <= 0) return 0;
  if (static_cast<long double>(value) >=
      static_cast<long double>(std::numeric_limits<qint64>::max()))
    return std::numeric_limits<qint64>::max();
  return qint64(std::floor(value));
}

inline qint64 ceilFrame(double value) {
  if (!std::isfinite(value) || value <= 0) return 0;
  if (static_cast<long double>(value) >=
      static_cast<long double>(std::numeric_limits<qint64>::max()))
    return std::numeric_limits<qint64>::max();
  return qint64(std::ceil(value));
}

// Determine if index field refered to by label are frame units
inline bool isFrameUnits(const DataRange *dataRange, const QString &label) {
  if (!dataRange) {
    return false;
  }

  const QList<Kst::IndexFieldProperties> fields =
      dataRange->indexFieldProperties();
  foreach (const Kst::IndexFieldProperties &ifp, fields) {
    if (ifp.name == label) {
      return ifp.is_frame;
    }
  }

  return label.compare("frames", Qt::CaseInsensitive) == 0;
}

inline bool isFrameUnits(DataSource *dataSource, const QString &label) {
  if (!dataSource) {
    return false;
  }
  for (const IndexFieldProperties &field : dataSource->indexFieldProperties()) {
    if (field.name == label) {
      return field.is_frame;
    }
  }
  return label.compare("frames", Qt::CaseInsensitive) == 0;
}

// determines the length of the data source by checking, in order, 
// the length of the reference field, the units field, and any frame-counted field
inline qint64 maxFrameLocked(DataSource *ds, const DataRange *dataRange,
                             const QString &referenceField,
                             const QString &unitsHint) {
  if (!ds) {
    return -1;
  }

  QString field = referenceField;
  if (!field.isEmpty() && ds->vector().isValid(field)) {
    const DataVector::DataInfo info = ds->vector().dataInfo(field);
    if (info.frameCount > 0) {
      return qint64(info.frameCount) - 1;
    }
  }

  if (!unitsHint.isEmpty() && !isFrameUnits(ds, unitsHint) &&
      ds->vector().isValid(unitsHint)) {
    const DataVector::DataInfo info = ds->vector().dataInfo(unitsHint);
    if (info.frameCount > 0) {
      return qint64(info.frameCount) - 1;
    }
  }

  const QStringList fields = ds->vector().list();
  foreach (const QString &name, fields) {
    if (ds->vector().isValid(name)) {
      const DataVector::DataInfo info = ds->vector().dataInfo(name);
      if (info.frameCount > 0) {
        return qint64(info.frameCount) - 1;
      }
    }
  }

  return -1;
}

inline bool floorFrameLocked(DataSource *ds, const DataRange *dataRange,
                             double value, const QString &units,
                             const QString &referenceField, qint64 *frameOut) {
  if (!frameOut) {
    return false;
  }

  if (isFrameUnits(ds, units)) {
    const qint64 frame = floorFrame(value);
    *frameOut = frame;
    return true;
  }

  if (!ds->vector().isValid(units)) {
    return false;
  }

  const qint64 maxFrame = maxFrameLocked(ds, dataRange, referenceField, units);
  if (maxFrame < 0) {
    return false;
  }
  qint64 frame = ds->indexToFrame(value, units);
  if (frame < 0) {
    return false;
  }

  const double minValue = ds->frameToIndex(0, units);
  const double maxValue = ds->frameToIndex(maxFrame, units);

  if (value <= minValue) {
    *frameOut = 0;
    return true;
  }

  if (value > maxValue) {
    *frameOut = maxFrame;
    return true;
  }

  const double atFrame = ds->frameToIndex(frame, units);
  if (atFrame > value && frame > 0) {
    --frame;
  }

  if (maxFrame >= 0) {
    frame = qBound(qint64(0), frame, maxFrame);
  }

  *frameOut = frame;
  return true;
}

inline bool ceilFrameLocked(DataSource *ds, const DataRange *dataRange,
                            double value, const QString &units,
                            const QString &referenceField, qint64 *frameOut) {
  if (!frameOut) {
    return false;
  }

  if (isFrameUnits(ds, units)) {
    const qint64 frame = ceilFrame(value);
    *frameOut = frame;
    return true;
  }

  if (!ds->vector().isValid(units)) {
    return false;
  }

  const qint64 maxFrame = maxFrameLocked(ds, dataRange, referenceField, units);
  if (maxFrame < 0) {
    return false;
  }
  qint64 frame = ds->indexToFrame(value, units);
  if (frame < 0) {
    return false;
  }

  const double minValue = ds->frameToIndex(0, units);
  const double maxValue = ds->frameToIndex(maxFrame, units);

  if (value <= minValue) {
    *frameOut = 0;
    return true;
  }

  if (value > maxValue) {
    if (maxFrame == std::numeric_limits<qint64>::max()) {
      return false;
    }
    *frameOut = maxFrame + 1;
    return true;
  }

  const double atFrame = ds->frameToIndex(frame, units);
  if (atFrame < value && (maxFrame < 0 || frame < maxFrame)) {
    ++frame;
  }

  if (maxFrame >= 0) {
    frame = qBound(qint64(0), frame, maxFrame + 1);
  }

  *frameOut = frame;
  return true;
}

inline bool unitsFromFrameLocked(DataSource *ds, const DataRange *dataRange,
                                 qint64 frame, const QString &units,
                                 const QString &referenceField,
                                 double *valueOut) {
  if (!valueOut) {
    return false;
  }

  const qint64 maxFrame = maxFrameLocked(ds, dataRange, referenceField, units);
  if (maxFrame < 0) {
    return false;
  }
  frame = qMax(qint64(0), frame);

  if (isFrameUnits(ds, units)) {
    *valueOut = frame;
    return true;
  }

  if (!ds->vector().isValid(units)) {
    return false;
  }

  if (frame <= maxFrame) {
    *valueOut = ds->frameToIndex(frame, units);
    return true;
  }

  const double fps = ds->framePerIndex(units);
  const double valueAtMax = ds->frameToIndex(maxFrame, units);
  if (fps <= 0.0) {
    return false;
  }
  *valueOut = valueAtMax + double(frame - maxFrame) / fps;
  return true;
}

// Resolve DataRange controls into frame-based start/count using inclusive
// semantics: start uses floor, end uses ceil, and frame bounds are clamped to
// file limits.
inline bool resolveToFrameRange(const DataRange *dataRange,
                                DataSourcePtr dataSource,
                                const QString &referenceField,
                                double *startOffset, double *rangeCount,
                                bool *customStartIndex,
                                bool *customRangeCount) {
  if (!dataRange || !dataSource || !startOffset || !rangeCount) {
    return false;
  }

  DataSource *ds = dataSource.data();
  ds->readLock();

  const QString startUnits = dataRange->startUnits();
  const QString rangeUnits = dataRange->rangeUnits();

  bool localCustomStart = false;
  bool localCustomRange = false;

  qint64 startFrame = floorFrame(dataRange->start());
  if (!dataRange->countFromEnd()) {
    if (!floorFrameLocked(ds, dataRange, dataRange->start(), startUnits,
                          referenceField, &startFrame)) {
      ds->unlock();
      return false;
    }
    *startOffset = startFrame;
    localCustomStart = !isFrameUnits(ds, startUnits);
  }

  if (!dataRange->readToEnd()) {
    if (dataRange->countFromEnd()) {
      if (!isFrameUnits(ds, rangeUnits)) {
        const qint64 endFrame =
            maxFrameLocked(ds, dataRange, referenceField, rangeUnits);
        if (endFrame < 0) {
          ds->unlock();
          return false;
        }
        if (endFrame == std::numeric_limits<qint64>::max()) {
          ds->unlock();
          return false;
        }

        double endExclusiveInRangeUnits = 0;
        if (!unitsFromFrameLocked(ds, dataRange, endFrame + 1, rangeUnits,
                                  referenceField, &endExclusiveInRangeUnits)) {
          ds->unlock();
          return false;
        }

        const double startInRangeUnits =
            endExclusiveInRangeUnits - dataRange->range();
        qint64 computedStartFrame = 0;
        if (!floorFrameLocked(ds, dataRange, startInRangeUnits, rangeUnits,
                              referenceField, &computedStartFrame)) {
          ds->unlock();
          return false;
        }

        *rangeCount = qMax(1.0, double(endFrame - computedStartFrame + 1));
        localCustomRange = true;
      }
    } else {
      qint64 endFrame = startFrame;
      if (isFrameUnits(ds, rangeUnits)) {
        const qint64 span = qMax(qint64(1), floorFrame(dataRange->range()));
        endFrame = qMax(0, startFrame + span - 1);
      } else {
        double startInRangeUnits = 0;
        if (!unitsFromFrameLocked(ds, dataRange, startFrame, rangeUnits,
                                  referenceField, &startInRangeUnits)) {
          ds->unlock();
          return false;
        }

        qint64 exclusiveEndFrame = 0;
        if (!ceilFrameLocked(ds, dataRange,
                             startInRangeUnits + dataRange->range(), rangeUnits,
                             referenceField, &exclusiveEndFrame)) {
          ds->unlock();
          return false;
        }

        endFrame = qMax(startFrame, exclusiveEndFrame - 1);

        localCustomRange = true;
      }

      if (endFrame < startFrame) {
        endFrame = startFrame;
      }
      *rangeCount = qMax(1.0, double(endFrame - startFrame + 1));
    }
  }

  ds->unlock();

  if (customStartIndex) {
    *customStartIndex = localCustomStart;
  }
  if (customRangeCount) {
    *customRangeCount = localCustomRange;
  }

  return true;
}

} // namespace DataRangeConversion
} // namespace Kst

#endif

// vim: ts=2 sw=2 et
