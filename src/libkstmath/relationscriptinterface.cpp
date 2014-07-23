/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2014 Barth Netterfield                                *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "relationscriptinterface.h"

#include "objectstore.h"
#include "datasourcepluginmanager.h"
#include "updatemanager.h"
#include "updateserver.h"
#include "vector.h"
#include "colorsequence.h"
#include "linestyle.h"
#include "palette.h"

#include <QStringBuilder>


namespace Kst {

QString doRelationScriptCommand(QString command,Relation *relation) {

  QString v=ScriptInterface::doObjectCommand(command, relation);
  if (!v.isEmpty()) {
    return v;
  }

  return QString();
}


QString RelationSI::maxX(QString&) {
  return QString::number(relation->maxX());
}

QString RelationSI::minX(QString&) {
  return QString::number(relation->minX());
}

QString RelationSI::maxY(QString&) {
  return QString::number(relation->maxY());
}

QString RelationSI::minY(QString&) {
  return QString::number(relation->minY());
}

QString RelationSI::showEditDialog(QString&) {
  relation->showEditDialog();
  return "done";
}

/******************************************************/
/* Curves                                             */
/******************************************************/
CurveSI::CurveSI(CurvePtr it) {
  curve=it;
  relation = it;

  _fnMap.insert("setXVector",&CurveSI::setXVector);
  _fnMap.insert("setYVector",&CurveSI::setYVector);
  _fnMap.insert("setXError",&CurveSI::setXError);
  _fnMap.insert("setYError",&CurveSI::setYError);
  _fnMap.insert("setXMinusError",&CurveSI::setXMinusError);
  _fnMap.insert("setYMinusError",&CurveSI::setYMinusError);

  _fnMap.insert("setColor",&CurveSI::setColor);
  _fnMap.insert("setHeadColor",&CurveSI::setHeadColor);
  _fnMap.insert("setBarFillColor",&CurveSI::setBarFillColor);
  _fnMap.insert("setHasPoints",&CurveSI::setHasPoints);
  _fnMap.insert("setHasLines",&CurveSI::setHasLines);
  _fnMap.insert("setHasBars",&CurveSI::setHasBars);
  _fnMap.insert("setHasHead",&CurveSI::setHasHead);

  _fnMap.insert("color",&CurveSI::color);
  _fnMap.insert("headColor",&CurveSI::headColor);
  _fnMap.insert("barFillColor",&CurveSI::barFillColor);
  _fnMap.insert("hasPoints",&CurveSI::hasPoints);
  _fnMap.insert("hasLines",&CurveSI::hasLines);
  _fnMap.insert("hasBars",&CurveSI::hasBars);
  _fnMap.insert("hasHead",&CurveSI::hasHead);

  _fnMap.insert("setLineWidth",&CurveSI::setLineWidth);
  _fnMap.insert("setPointSize",&CurveSI::setPointSize);

  _fnMap.insert("setPointType",&CurveSI::setPointType);
  _fnMap.insert("setHeadType",&CurveSI::setHeadType);
  _fnMap.insert("setLineStyle",&CurveSI::setLineStyle);
  _fnMap.insert("setPointDensity",&CurveSI::setPointDensity);

  _fnMap.insert("lineWidth",&CurveSI::lineWidth);
  _fnMap.insert("pointSize",&CurveSI::pointSize);
  _fnMap.insert("pointType",&CurveSI::pointType);
  _fnMap.insert("headType",&CurveSI::headType);
  _fnMap.insert("lineStyle",&CurveSI::lineStyle);
  _fnMap.insert("pointDensity",&CurveSI::pointDensity);

  // functions from relationSI
  _fnMap.insert("maxX",&CurveSI::maxX);
  _fnMap.insert("minX",&CurveSI::minX);
  _fnMap.insert("maxY",&CurveSI::maxY);
  _fnMap.insert("minY",&CurveSI::minY);
  _fnMap.insert("showEditDialog",&CurveSI::showEditDialog);

  _fnMap.insert("xVector",&CurveSI::xVector);
  _fnMap.insert("yVector",&CurveSI::yVector);
  _fnMap.insert("xErrorVector",&CurveSI::xErrorVector);
  _fnMap.insert("yErrorVector",&CurveSI::yErrorVector);
  _fnMap.insert("xMinusErrorVector",&CurveSI::xMinusErrorVector);
  _fnMap.insert("yMinusErrorVector",&CurveSI::yMinusErrorVector);

}

QString CurveSI::doCommand(QString command_in) {
  QString command = command_in.left(command_in.indexOf('('));

  CurveInterfaceMemberFn fn=_fnMap.value(command,&CurveSI::noSuchFn);

  if(fn!=&CurveSI::noSuchFn) {
    return CALL_MEMBER_FN(*this,fn)(command_in);
  }


  QString v=doRelationScriptCommand(command_in, curve);
  if (!v.isEmpty()) {
    return v;
  }

  return "No such command";
}

bool CurveSI::isValid() {
  return curve.isPtrValid();
}

ScriptInterface* CurveSI::newCurve(ObjectStore *store) {
  CurvePtr curve;
  curve = store->createObject<Curve>();
  curve->setColor(ColorSequence::self().next());

  return new CurveSI(curve);
}

QByteArray CurveSI::endEditUpdate() {
  curve->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+curve->Name()).toLatin1();
}

/***************************/
/* commands                */
/***************************/
QString CurveSI::setXVector(QString& command) {
  QString parameter = getArg(command);

  VectorPtr v = kst_cast<Vector>(curve->store()->retrieveObject(parameter));
  if (v) {
    curve->setXVector(v);
    return "Done";
  } else {
    return QString("Vector %1 not found").arg(parameter);
  }

}

QString CurveSI::setYVector(QString& command) {
  QString parameter = getArg(command);

  VectorPtr v = kst_cast<Vector>(curve->store()->retrieveObject(parameter));
  if (v) {
    curve->setYVector(v);
    return "Done";
  } else {
    return QString("Vector %1 not found").arg(parameter);
  }
}

QString CurveSI::setXError(QString& command) {
  QString parameter = getArg(command);

  VectorPtr v = kst_cast<Vector>(curve->store()->retrieveObject(parameter));
  if (v) {
    curve->setXError(v);
    return "Done";
  } else {
    return QString("Vector %1 not found").arg(parameter);
  }
}

QString CurveSI::setYError(QString& command) {
  QString parameter = getArg(command);

  VectorPtr v = kst_cast<Vector>(curve->store()->retrieveObject(parameter));
  if (v) {
    curve->setYError(v);
    return "Done";
  } else {
    return QString("Vector %1 not found").arg(parameter);
  }
}

QString CurveSI::setXMinusError(QString& command) {
  QString parameter = getArg(command);

  VectorPtr v = kst_cast<Vector>(curve->store()->retrieveObject(parameter));
  if (v) {
    curve->setXMinusError(v);
    return "Done";
  } else {
    return QString("Vector %1 not found").arg(parameter);
  }

}

QString CurveSI::setYMinusError(QString& command) {
  QString parameter = getArg(command);

  VectorPtr v = kst_cast<Vector>(curve->store()->retrieveObject(parameter));
  if (v) {
    curve->setYMinusError(v);
    return "Done";
  } else {
    return QString("Vector %1 not found").arg(parameter);
  }
}

QString CurveSI::setColor(QString& command) {
  QString parameter = getArg(command);
  curve->setColor(QColor(parameter));
  return "Done";
}

QString CurveSI::setHeadColor(QString& command) {
  QString parameter = getArg(command);
  curve->setHeadColor(QColor(parameter));
  return "Done";
}

QString CurveSI::setBarFillColor(QString& command) {
  QString parameter = getArg(command);
  curve->setBarFillColor(QColor(parameter));
  return "Done";
}

QString CurveSI::setHasPoints(QString& command) {
  QString parameter = getArg(command);

  if (parameter.toLower() == "true") {
    curve->setHasPoints(true);
  } else {
    curve->setHasPoints(false);
  }
  return "Done";
}

QString CurveSI::setHasLines(QString& command) {
  QString parameter = getArg(command);

  if (parameter.toLower() == "true") {
    curve->setHasLines(true);
  } else {
    curve->setHasLines(false);
  }
  return "Done";
}

QString CurveSI::setHasBars(QString& command) {
  QString parameter = getArg(command);

  if (parameter.toLower() == "true") {
    curve->setHasBars(true);
  } else {
    curve->setHasBars(false);
  }
  return "Done";
}

QString CurveSI::setHasHead(QString& command) {
  QString parameter = getArg(command);

  if (parameter.toLower() == "true") {
    curve->setHasHead(true);
  } else {
    curve->setHasHead(false);
  }
  return "Done";
}

QString CurveSI::setLineWidth(QString& command) {
  QString parameter = getArg(command);
  int x = parameter.toInt();

  if (x<0) x = 0;
  if (x>100) x = 100;

  curve->setLineWidth(x);
  return "Done";
}

QString CurveSI::setPointSize(QString& command) {
  QString parameter = getArg(command);
  int x = parameter.toInt();

  if (x<0) x = 0;
  if (x>100) x = 100;

  curve->setPointSize(x);
  return "Done";
}

QString CurveSI::setPointType(QString& command) {
  QString parameter = getArg(command);
  int x = parameter.toInt();

  if (x<0) x = 0;

  curve->setPointType(x);
  return "Done";

}

QString CurveSI::setHeadType(QString& command) {
  QString parameter = getArg(command);
  int x = parameter.toInt();

  if (x<0) x = 0;

  curve->setHeadType(x);
  return "Done";

}

QString CurveSI::setLineStyle(QString& command) {
  QString parameter = getArg(command);
  int x = parameter.toInt();

  if (x<0) x = 0;

  curve->setLineStyle(x);
  return "Done";
}

QString CurveSI::setPointDensity(QString& command) {
  QString parameter = getArg(command);
  int x = parameter.toInt();

  if (x<0) x = 0;
  if (x>POINTDENSITY_MAXTYPE) x = POINTDENSITY_MAXTYPE;

  curve->setPointDensity(x);
  return "Done";
}

QString CurveSI::color(QString&) {
  return curve->color().name();
}

QString CurveSI::headColor(QString&) {
  return curve->headColor().name();
}

QString CurveSI::barFillColor(QString&) {
  return curve->barFillColor().name();
}

QString CurveSI::hasPoints(QString&) {
  if (curve->hasPoints()) {
    return "True";
  } else {
    return "False";
  }
}

QString CurveSI::hasLines(QString&) {
  if (curve->hasLines()) {
    return "True";
  } else {
    return "False";
  }
}

QString CurveSI::hasBars(QString&) {
  if (curve->hasBars()) {
    return "True";
  } else {
    return "False";
  }
}

QString CurveSI::hasHead(QString&) {
  if (curve->hasHead()) {
    return "True";
  } else {
    return "False";
  }
}

QString CurveSI::lineWidth(QString&) {
  return QString::number(curve->lineWidth());
}

QString CurveSI::pointSize(QString&) {
  return QString::number(curve->pointSize());
}

QString CurveSI::pointType(QString&) {
  return QString::number(curve->pointType());
}

QString CurveSI::headType(QString&) {
  return QString::number(curve->headType());
}

QString CurveSI::lineStyle(QString&) {
  return QString::number(curve->lineStyle());
}

QString CurveSI::pointDensity(QString& command) {
  return QString::number(curve->pointDensity());
}

QString CurveSI::xVector(QString &) {
  return curve->xVector()->shortName();
}

QString CurveSI::yVector(QString &) {
  return curve->yVector()->shortName();
}

QString CurveSI::xErrorVector(QString &) {
  return curve->xErrorVector()->shortName();
}

QString CurveSI::yErrorVector(QString &) {
  return curve->yErrorVector()->shortName();
}

QString CurveSI::xMinusErrorVector(QString &) {
  return curve->xMinusErrorVector()->shortName();
}

QString CurveSI::yMinusErrorVector(QString &) {
  return curve->yMinusErrorVector()->shortName();
}


/******************************************************/
/* Images                                             */
/******************************************************/
ImageSI::ImageSI(ImagePtr it) {
  image=it;
  relation = it;

  _fnMap.insert("setMatrix", &ImageSI::setMatrix);
  _fnMap.insert("setPalette", &ImageSI::setPalette);
  _fnMap.insert("setFixedColorRange", &ImageSI::setFixedColorRange);
  _fnMap.insert("setAutoColorRange", &ImageSI::setAutoColorRange);

  _fnMap.insert("minZ", &ImageSI::lowerThreshold);
  _fnMap.insert("maxZ", &ImageSI::upperThreshold);

  // functions from relationSI
  _fnMap.insert("maxX",&ImageSI::maxX);
  _fnMap.insert("minX",&ImageSI::minX);
  _fnMap.insert("maxY",&ImageSI::maxY);
  _fnMap.insert("minY",&ImageSI::minY);
  _fnMap.insert("showEditDialog",&ImageSI::showEditDialog);

}

QString ImageSI::doCommand(QString command_in) {
  QString command = command_in.left(command_in.indexOf('('));

  ImageInterfaceMemberFn fn=_fnMap.value(command,&ImageSI::noSuchFn);

  if(fn!=&ImageSI::noSuchFn) {
    return CALL_MEMBER_FN(*this,fn)(command_in);
  }


  QString v=doRelationScriptCommand(command_in, image);
  if (!v.isEmpty()) {
    return v;
  }

  return "No such command";
}

bool ImageSI::isValid() {
  return image.isPtrValid();
}

ScriptInterface* ImageSI::newImage(ObjectStore *store) {
  ImagePtr image;
  image = store->createObject<Image>();
  image->setAutoThreshold(true);
  image->setPalette(Palette::getPaletteList().at(0));

  return new ImageSI(image);
}

QByteArray ImageSI::endEditUpdate() {
  image->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+image->Name()).toLatin1();
}

/***************************/
/* commands                */
/***************************/

QString ImageSI::setMatrix(QString& command) {
  QString parameter = getArg(command);

  MatrixPtr m = kst_cast<Matrix>(image->store()->retrieveObject(parameter));

  if (m) {
    image->setMatrix(m);
    return "Done";
  } else {
    return QString("matrix %1 not found").arg(parameter);
  }
}

QString ImageSI::setPalette(QString& command) {
  QString parameter = getArg(command);

  int x = parameter.toInt();

  if (x<0) x = 0;

  QStringList palette_list = Palette::getPaletteList();

  if (x>=palette_list.length()) {
    x = palette_list.length();
  }
  image->setPalette(palette_list.at(x));

  return "Done";
}

QString ImageSI::setFixedColorRange(QString& command) {
  QStringList vars = getArgs(command);

  double zmin = vars[0].toDouble();
  double zmax = vars[1].toDouble();

  if (zmin==zmax) {
    image->setAutoThreshold(true);
  } else {
    if (zmin>zmax) {
      double z = zmax;
      zmax = zmin;
      zmin = z;
    }
    image->setAutoThreshold(false);
    image->setLowerThreshold(zmin);
    image->setUpperThreshold(zmax);
  }
  return "Done";
}


QString ImageSI::setAutoColorRange(QString& command) {
  QString parameter = getArg(command);
  double per = parameter.toDouble();

  image->setAutoThreshold(false);
  image->setThresholdToSpikeInsensitive(per);

  return "Done";
}

QString ImageSI::lowerThreshold(QString&) {
  return QString::number(image->lowerThreshold());
}

QString ImageSI::upperThreshold(QString&) {
  return QString::number(image->upperThreshold());
}

}
