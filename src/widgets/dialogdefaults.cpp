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
#include "settings.h"

#include <QPen>


namespace Kst {

QSettings& dialogDefaults()
{
  static QSettings& settingsObject = createSettings("dialog");
  return settingsObject;
}


void setDataVectorDefaults(DataVectorPtr V) {
  dialogDefaults().setValue("vector/datasource", V->filename());
}

void setGenVectorDefaults(GeneratedVectorPtr V) {
  dialogDefaults().setValue("genVector/first", V->value(V->value(0)));
  dialogDefaults().setValue("genVector/last", V->value(V->length()-1));
  dialogDefaults().setValue("genVector/min", V->min());
  dialogDefaults().setValue("genVector/max", V->max());
  dialogDefaults().setValue("genVector/length", V->length());

}

void setDataMatrixDefaults(DataMatrixPtr M) {
  //qDebug() << "M...filename: " << M->dataSource()->fileName();

  // FIXME: data source filename isn't valid...
  dialogDefaults().setValue("matrix/datasource",M->dataSource()->fileName());

  dialogDefaults().setValue("matrix/xCountFromEnd",M->xCountFromEnd());
  dialogDefaults().setValue("matrix/yCountFromEnd",M->yCountFromEnd());
  dialogDefaults().setValue("matrix/xReadToEnd",M->xReadToEnd());
  dialogDefaults().setValue("matrix/yReadToEnd",M->yReadToEnd());

  dialogDefaults().setValue("matrix/xNumSteps",M->xNumSteps());
  dialogDefaults().setValue("matrix/yNumSteps",M->yNumSteps());
  dialogDefaults().setValue("matrix/reqXStart",M->reqXStart());
  dialogDefaults().setValue("matrix/reqYStart",M->reqYStart());

  dialogDefaults().setValue("matrix/frame",M->frame());
}

void setHistogramDefaults(HistogramPtr H) {
  dialogDefaults().setValue("histogram/realTimeAutoBin", H->realTimeAutoBin());
  dialogDefaults().setValue("histogram/normalizationType",H->normalizationType());
}

void saveDialogDefaultsLockPosToData(const QString &group_name, const bool lockPosToData) {
  dialogDefaults().setValue(group_name+"/lockPosToData", QVariant(lockPosToData).toString());
}


void saveDialogDefaultsBrush(const QString &group_name, const QBrush &b) {
  // Save the brush
  dialogDefaults().setValue(group_name+"/fillBrushColor", QVariant(b.color()).toString());
  dialogDefaults().setValue(group_name+"/fillBrushStyle", QVariant((int)b.style()).toString());
  dialogDefaults().setValue(group_name+"/fillBrushUseGradient", QVariant(bool(b.gradient())).toString());
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
     dialogDefaults().setValue(group_name+"/fillBrushGradient", stopList);
   }
}

void saveDialogDefaultsPen(const QString &group_name, const QPen &p) {
  // Save stroke...
  QBrush b = p.brush();

  dialogDefaults().setValue(group_name+"/strokeStyle", QVariant((int)p.style()).toString());
  dialogDefaults().setValue(group_name+"/strokeWidth", p.widthF());
  dialogDefaults().setValue(group_name+"/strokeJoinStyle", QVariant(p.joinStyle()).toString());
  dialogDefaults().setValue(group_name+"/strokeCapStyle", QVariant(p.capStyle()).toString());
  dialogDefaults().setValue(group_name+"/strokeBrushColor", QVariant(b.color()).toString());
  dialogDefaults().setValue(group_name+"/strokeBrushStyle", QVariant((int)b.style()).toString());

}


QBrush dialogDefaultsBrush(const QString &group_name, bool default_no_fill) {
  //set the brush
  QBrush brush;
  bool useGradient = dialogDefaults().value(group_name +"/fillBrushUseGradient", false).toBool();
  if (useGradient) {
    QStringList stopInfo =
        dialogDefaults().value(group_name +"/fillBrushGradient", "0,#000000,1,#ffffff,").
        toString().split(',', QString::SkipEmptyParts);
    QLinearGradient gradient(1,0,0,0);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    for (int i = 0; i < stopInfo.size(); i+=2) {
      gradient.setColorAt(stopInfo.at(i).toDouble(), QColor(stopInfo.at(i+1)));
    }
    brush = QBrush(gradient);
  } else {
    QColor color = dialogDefaults().value(group_name +"/fillBrushColor",QColor(Qt::white)).value<QColor>();
    brush.setColor(color);
    if (default_no_fill) {
      brush.setStyle((Qt::BrushStyle)dialogDefaults().value(group_name +"/fillBrushStyle",0).toInt());
    } else {
      brush.setStyle((Qt::BrushStyle)dialogDefaults().value(group_name +"/fillBrushStyle",1).toInt());
    }
  }

  return brush;
}

QPen dialogDefaultsPen(const QString &group_name, bool default_no_pen) {
  QPen pen;
  QColor color;
  QBrush brush;
  if (default_no_pen) {
    pen.setStyle((Qt::PenStyle)dialogDefaults().value(group_name +"/strokeStyle", 0).toInt());
  } else {
    pen.setStyle((Qt::PenStyle)dialogDefaults().value(group_name +"/strokeStyle", 1).toInt());
  }
  pen.setWidthF(dialogDefaults().value(group_name +"/strokeWidth",0).toDouble());
  pen.setJoinStyle((Qt::PenJoinStyle)dialogDefaults().value(group_name +"/strokeJoinStyle",64).toInt());
  pen.setCapStyle((Qt::PenCapStyle)dialogDefaults().value(group_name +"/strokeCapStyle",16).toInt());
  color = dialogDefaults().value(group_name +"/strokeBrushColor",QColor(Qt::black)).value<QColor>();
  brush.setColor(color);
  brush.setStyle((Qt::BrushStyle)dialogDefaults().value(group_name +"/strokeBrushStyle",1).toInt());
  pen.setBrush(brush);

  return pen;
}

bool dialogDefaultsLockPosToData(const QString &group_name) {
  return dialogDefaults().value(group_name+"/lockPosToData",false).toBool();
}
}
