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

  bool processCommandLine();
  QString kstFileName();
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
  QStringList _fileNames;
  QStringList _arguments;
  PlotItem *_plotItem;
  
  // lists of command line named plot items (and their names).
  QList<PlotItem*> _plotItems;
  QStringList _plotNames;

  DataVectorList _vectors;

  Document *_document;

  void _setIntArg(int *arg, QString Message);
  void _setDoubleArg(double *arg, QString Message);
  void _setStringArg(QString &arg, QString Message);
  void usage(QString Message = "");
  DataVectorPtr createOrFindDataVector(QString field, DataSourcePtr ds);
  void createOrFindPlot(const QString tagname);
  void createCurveInPlot(const ObjectTag &tag, VectorPtr xv, VectorPtr yv, VectorPtr ev=0);
};

}

#endif
