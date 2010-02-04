//
// C++ Interface: CommandLineParser
//
// Description: parse the command line; return directives to be
//
//
// Author: Barth Netterfield <netterfield@physics.utoronto.ca>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KSTCOMMANDLINEPARSER_H
#define KSTCOMMANDLINEPARSER_H

#include "document.h"
#include "datavector.h"
#include "plotitem.h"

#include <QStringList>
namespace Kst {
/**
	@author Barth Netterfield <netterfield@physics.utoronto.ca>
*/
class CommandLineParser{
public:
  CommandLineParser(Document *doc);
  ~CommandLineParser();

  bool processCommandLine(bool *ok);
  QString kstFileName();
  QString pngFile() const {return _pngFile;}
  QString printFile() const {return _printFile;}
  //bool landscape() const {return _landscape;}
private:
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
  QPrinter::PaperSize _paperSize;

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
};

}

#endif
