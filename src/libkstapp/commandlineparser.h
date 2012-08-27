/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008  Barth Netterfield                               *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTCOMMANDLINEPARSER_H
#define KSTCOMMANDLINEPARSER_H

#include "document.h"
#include "datavector.h"
#include "plotitem.h"
#include "mainwindow.h"

#include <QStringList>
#ifndef KST_NO_PRINTER
#include <QPrinter>
#endif

namespace Kst {
/**
	@author Barth Netterfield <netterfield@physics.utoronto.ca>
*/
class CommandLineParser
{
public:
  CommandLineParser(Document *doc, MainWindow* mw);
  ~CommandLineParser();

  bool processCommandLine(bool *ok);
  QString kstFileName();
  QString pngFile() const {return _pngFile;}
  QString printFile() const {return _printFile;}
  //bool landscape() const {return _landscape;}

private:
  MainWindow* _mainWindow;
  bool _doAve;
  bool _doSkip;
  bool _doConsecutivePlots;
  bool _useBargraph;
  bool _useLines;
  bool _usePoints;
  bool _overrideStyle;
  double _sampleRate;
  int _numFrames;
  int _startFrame;
  int _skip;
  QString _plotName;
  QString _errorField;
  QString _fileName;
  QString _xField;
  QString _pngFile;
  QString _printFile;
  bool _landscape;
#ifndef KST_NO_PRINTER
  QPrinter::PaperSize _paperSize;
#endif

  QStringList _fileNames;
  QStringList _arguments;
  PlotItem *_plotItem;
  
  // lists of command line named plot items (and their names).
  QList<PlotItem*> _plotItems;

  DataVectorList _vectors;

  Document *_document;

  bool _setIntArg(int *arg, QString Message, bool accept_end=false);
  bool _setDoubleArg(double *arg, QString Message);
  bool _setStringArg(QString &arg, QString Message);
  DataVectorPtr createOrFindDataVector(QString field, DataSourcePtr ds);
  void createOrFindPlot(const QString name);
  void createCurveInPlot(VectorPtr xv, VectorPtr yv, VectorPtr ev=0);
  void createImageInPlot(MatrixPtr m);
  void addCurve(CurvePtr curve);
  ObjectList<Object> autoCurves(DataSourcePtr ds);
};

}

#endif
