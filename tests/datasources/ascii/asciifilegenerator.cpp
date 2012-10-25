/***************************************************************************
 *                                                                         *
 *   Copyright : (C) 2012 Peter Kümmel                                     *
 *   email     : syntheticpp@gmx.net                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore>

#include <math.h>
#include <stdio.h>

#ifdef Q_CC_MSVC
#define snprintf _snprintf_s
#endif


void printHelp()
{
  printf("Arguments: <filename> <number of columns>  <file size in MB>\n");
}

QByteArray calcLine(double x, int numCols)
{
  QString cols;
  for (int i = 0; i < numCols; i++) {
    cols += QString("%1").arg(sin(x));
  }
  cols += "\n";
  return cols.toLatin1();
}


QByteArray calcLine2(double x, int numCols)
{
  QByteArray cols;
  for (int i = 0; i < numCols; i++) {
    char buffer[50];
    snprintf(buffer, 50, "%.10f ", sin(x));
    cols += QByteArray(buffer);
  }
  return cols + "\n";
}


int main(int argc, char *argv[]) 
{
  QCoreApplication app(argc, argv);
  QStringList args = app.arguments();
  if (args.size() != 4) {
    printHelp();
    return 1;
  }

  QString filename =  args[1];
  int cols = args[2].toInt();
  qint64 mb = args[3].toInt();
  
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    printf("Could not open file '%s'\n", qPrintable(QFileInfo(filename).absoluteFilePath()));
    return 1;
  }

  printf("Writing file %s\n", qPrintable(QFileInfo(filename).absoluteFilePath()));
  int progress = 0;
  int progStep = mb > 1000 ? 1 : 10;
  double dx = 1.0 * M_PI / 100;
  double x = 0;
  qint64 fileSize = 0;
  qint64 maxSize = mb * 1024 * 1024;
  while (fileSize < maxSize) {
    file.write(calcLine2(x, cols));
    x += dx;
    fileSize = file.size();
    int done = 100.0 / progStep * fileSize / maxSize;
    if (done != progress) {
      printf("%i%%\n", done * progStep);
      progress = done;
    }
  }

  return 0;
}
