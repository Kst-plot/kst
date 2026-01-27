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
#include <QPageSize>
#endif

namespace Kst {
/**
	@author Barth Netterfield <netterfield@physics.utoronto.ca>
*/
class CommandLineParser:QObject
{
    Q_OBJECT
public:
  CommandLineParser(Document *doc, MainWindow* mw);
  ~CommandLineParser();

  bool processCommandLine(bool *ok);
  bool checkFile(QString filename);
  QString kstFileName();
  QString pngFile() const {return _pngFile;}
  int pngWidth() const {return _pngWidth;}
  int pngHeight() const {return _pngHeight;}
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
  int _pngWidth;
  int _pngHeight;
  QString _printFile;
  bool _landscape;
#ifndef KST_NO_PRINTER
  QPageSize::PageSizeId _paperSize;
#endif

  QStringList _fileNames;
  QStringList _arguments;
  PlotItem *_plotItem;
  QString _xlabel;
  QString _ylabel;
  int _legendMode;
    
  // lists of command line named plot items (and their names).
  QList<PlotItem*> _plotItems;

  DataVectorList _vectors;

  Document *_document;

  int _num_cols;

  int _asciiFirstLine;
  int _asciiFieldLine;
  bool _asciiNoFieldNames;
  int _asciiUnitsLine;
  bool _asciiNoUnits;
  bool _asciiSpaceDelim;
  char _asciiDelim;
  int _asciiFixedWidth;
  bool _asciiNoFixedWidth;
  bool _asciiDecimalDot;
  bool _asciiDecimalComma;

  bool _setIntArg(int *arg, QString Message, bool accept_end=false);
  bool _setDoubleArg(double *arg, QString Message);
  bool _setStringArg(QString &arg, QString Message);
  DataVectorPtr createOrFindDataVector(QString field, DataSourcePtr ds);
  void createOrFindPlot(const QString name);
  void createOrFindTab(const QString name);
  void createCurveInPlot(VectorPtr xv, VectorPtr yv, VectorPtr ev=0);
  void createImageInPlot(MatrixPtr m);
  void addCurve(CurvePtr curve);
  ObjectList<Object> autoCurves(DataSourcePtr ds);
  void applyLabels();
  void printUsage(const QString &t);
};

}

#endif
