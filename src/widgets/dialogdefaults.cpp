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

#include "dialogdefaults.h"
#include "datasource.h"

#include <QPen>


namespace Kst {
  QSettings *_dialogDefaults;

void setDataVectorDefaults(DataVectorPtr V) {
  _dialogDefaults->setValue("vector/datasource", V->filename());
}

void setGenVectorDefaults(GeneratedVectorPtr V) {
  _dialogDefaults->setValue("genVector/min", V->min());
  _dialogDefaults->setValue("genVector/max", V->max()); 
  _dialogDefaults->setValue("genVector/length", V->length());
}

void setDataMatrixDefaults(DataMatrixPtr M) {
  //qDebug() << "M...filename: " << M->dataSource()->fileName();

  // FIXME: data source filename isn't valid...
  _dialogDefaults->setValue("matrix/datasource",M->dataSource()->fileName());

  _dialogDefaults->setValue("matrix/xCountFromEnd",M->xCountFromEnd());
  _dialogDefaults->setValue("matrix/yCountFromEnd",M->yCountFromEnd());
  _dialogDefaults->setValue("matrix/xReadToEnd",M->xReadToEnd());
  _dialogDefaults->setValue("matrix/yReadToEnd",M->yReadToEnd());

  _dialogDefaults->setValue("matrix/xNumSteps",M->xNumSteps());
  _dialogDefaults->setValue("matrix/yNumSteps",M->yNumSteps());
  _dialogDefaults->setValue("matrix/reqXStart",M->reqXStart());
  _dialogDefaults->setValue("matrix/reqYStart",M->reqYStart());
}

void setHistogramDefaults(HistogramPtr H) {  
  _dialogDefaults->setValue("histogram/realTimeAutoBin", H->realTimeAutoBin());
  _dialogDefaults->setValue("histogram/normalizationType",H->normalizationType());
}

void saveDialogDefaultsLockPosToData(const QString &group_name, const bool lockPosToData) {
  _dialogDefaults->setValue(group_name+"/lockPosToData", QVariant(lockPosToData).toString());
}


void saveDialogDefaultsBrush(const QString &group_name, const QBrush &b) {
  // Save the brush
  _dialogDefaults->setValue(group_name+"/fillBrushColor", QVariant(b.color()).toString());
  _dialogDefaults->setValue(group_name+"/fillBrushStyle", QVariant(b.style()).toString());
  _dialogDefaults->setValue(group_name+"/fillBrushUseGradient", QVariant(bool(b.gradient())).toString());
  if (b.gradient()) {
    QString stopList;
    foreach(const QGradientStop &stop, b.gradient()->stops()) {
      qreal point = (qreal)stop.first;
      QColor color = (QColor)stop.second;

      stopList += QString::number(point);
      stopList += ',';
      stopList += color.name();
      stopList += ',';
    }
     _dialogDefaults->setValue(group_name+"/fillBrushGradient", stopList);
   }
}

void saveDialogDefaultsPen(const QString &group_name, const QPen &p) {
  // Save stroke...
  QBrush b = p.brush();

  _dialogDefaults->setValue(group_name+"/strokeStyle", QVariant(p.style()).toString());
  _dialogDefaults->setValue(group_name+"/strokeWidth", p.widthF());
  _dialogDefaults->setValue(group_name+"/strokeJoinStyle", QVariant(p.joinStyle()).toString());
  _dialogDefaults->setValue(group_name+"/strokeCapStyle", QVariant(p.capStyle()).toString());
  _dialogDefaults->setValue(group_name+"/strokeBrushColor", QVariant(b.color()).toString());
  _dialogDefaults->setValue(group_name+"/strokeBrushStyle", QVariant(b.style()).toString());

}


QBrush dialogDefaultsBrush(const QString &group_name) {
  //set the brush
  QBrush brush;
  bool useGradient = _dialogDefaults->value(group_name +"/fillBrushUseGradient", false).toBool();
  if (useGradient) {
    QStringList stopInfo =
        _dialogDefaults->value(group_name +"/fillBrushGradient", "0,#000000,1,#ffffff,").
        toString().split(',', QString::SkipEmptyParts);
    QLinearGradient gradient(1,0,0,0);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    for (int i = 0; i < stopInfo.size(); i+=2) {
      gradient.setColorAt(stopInfo.at(i).toDouble(), QColor(stopInfo.at(i+1)));
    }
    brush = QBrush(gradient);
  } else {
    QColor color = _dialogDefaults->value(group_name +"/fillBrushColor",QColor(Qt::white)).value<QColor>();
    brush.setColor(color);
    brush.setStyle((Qt::BrushStyle)_dialogDefaults->value(group_name +"/fillBrushStyle",1).toInt());
  }

  return brush;
}

QPen dialogDefaultsPen(const QString &group_name) {
  QPen pen;
  QColor color;
  QBrush brush;
  pen.setStyle((Qt::PenStyle)_dialogDefaults->value(group_name +"/strokeStyle", 1).toInt());
  pen.setWidthF(_dialogDefaults->value(group_name +"/strokeWidth",0).toDouble());
  pen.setJoinStyle((Qt::PenJoinStyle)_dialogDefaults->value(group_name +"/strokeJoinStyle",64).toInt());
  pen.setCapStyle((Qt::PenCapStyle)_dialogDefaults->value(group_name +"/strokeCapStyle",16).toInt());
  color = _dialogDefaults->value(group_name +"/strokeBrushColor",QColor(Qt::black)).value<QColor>();
  brush.setColor(color);
  brush.setStyle((Qt::BrushStyle)_dialogDefaults->value(group_name +"/strokeBrushStyle",1).toInt());
  pen.setBrush(brush);

  return pen;
}

bool dialogDefaultsLockPosToData(const QString &group_name) {
  return _dialogDefaults->value(group_name+"/lockPosToData",false).toBool();
}
}
