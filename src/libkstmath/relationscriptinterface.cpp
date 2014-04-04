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

#include <QStringBuilder>


namespace Kst {

QString doRelationScriptCommand(QString command,Relation *relation) {

  QString v=ScriptInterface::doNamedObjectCommand(command, relation);
  if (!v.isEmpty()) {
    return v;
  }

  if (command.startsWith("maxX(")) {
    return QString::number(relation->maxX());
  } // FIXME the rest...

  return QString();
}


/******************************************************/
/* Curves                                             */
/******************************************************/
CurveSI::CurveSI(CurvePtr it) {
  curve=it;

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

  _fnMap.insert("setLineWidth",&CurveSI::setLineWidth);
  _fnMap.insert("setPointSize",&CurveSI::setPointSize);

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


}
