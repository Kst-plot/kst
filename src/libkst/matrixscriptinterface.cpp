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

#include "matrixscriptinterface.h"

#include "objectstore.h"
#include "datasourcepluginmanager.h"
#include "updatemanager.h"
#include "updateserver.h"

#include <QStringBuilder>


namespace Kst {

QString doMatrixScriptCommand(QString command,Matrix *matrix) {

  QString v=ScriptInterface::doNamedObjectCommand(command, matrix);
  if (!v.isEmpty()) {
    return v;
  }

  if (command.startsWith("value(")) {
    command.remove("value(").chop(1);
    QStringList p = command.split(',');
    return QString::number(matrix->value(p[0].toDouble(), p[1].toDouble()));
  } else if (command.startsWith("length(")) {
    return QString::number(matrix->sampleCount());
  } else if (command.startsWith("min(")) {
    return QString::number(matrix->minValue());
  } else if (command.startsWith("max(")) {
    return QString::number(matrix->maxValue());
  } else if (command.startsWith("mean(")) {
    return QString::number(matrix->meanValue());
  } else if (command.startsWith("width(")) {
    return QString::number(matrix->xNumSteps());
  } else if (command.startsWith("height(")) {
    return QString::number(matrix->yNumSteps());
  } else if (command.startsWith("dX(")) {
    return QString::number(matrix->xStepSize());
  } else if (command.startsWith("dY(")) {
    return QString::number(matrix->yStepSize());
  } else if (command.startsWith("minX(")) {
    return QString::number(matrix->minX());
  } else if (command.startsWith("minY(")) {
    return QString::number(matrix->minY());
  } else if (command.startsWith("descriptionTip(")) {
    return matrix->descriptionTip();
  }

  return QString();
}


/******************************************************/
/* Data Matrixs                                       */
/******************************************************/
MatrixDataSI::MatrixDataSI(DataMatrixPtr it) {
    matrix=it;
}

QString MatrixDataSI::doCommand(QString command) {

  QString v=doMatrixScriptCommand(command, matrix);
  if (!v.isEmpty()) {
    return v;
  }


  if (command.startsWith(QLatin1String("change("))) {
    command.remove("change(").remove(')');
    QStringList p = command.split(',');
    DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(
                         matrix->store(), p.at(0));

    matrix->writeLock();
    matrix->change(ds,
                   p.at(1),         // field
                   p.at(2).toInt(), // x start
                   p.at(3).toInt(), // y start
                   p.at(4).toInt(), // num x steps
                   p.at(5).toInt(), // num y steps

                   false, false, 0,

                   p.at(6).toDouble(), // min x
                   p.at(7).toDouble(), // min y
                   p.at(8).toDouble(), // step x
                   p.at(9).toDouble() // step y
                   );
    matrix->unlock();
    return "Done";
  } else if (command.startsWith("field(")) {
    return matrix->field();
  } else if (command.startsWith("filename(")) {
    return matrix->filename();
  } else if (command.startsWith("startX(")) {
    return QString::number(matrix->reqXStart());
  } else if (command.startsWith("startY(")) {
    return QString::number(matrix->reqYStart());
  }

  return "No such command";
}

bool MatrixDataSI::isValid() {
  return matrix.isPtrValid();
}

ScriptInterface* MatrixDataSI::newMatrix(ObjectStore *store) {
  DataMatrixPtr matrix;
  matrix = store->createObject<DataMatrix>();
  return new MatrixDataSI(matrix);
}

QByteArray MatrixDataSI::endEditUpdate() {
  matrix->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+matrix->Name()).toLatin1();
}

}
