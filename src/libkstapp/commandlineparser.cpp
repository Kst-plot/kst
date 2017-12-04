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

#include <config.h>
#include <stdio.h>

#include "commandlineparser.h"
//#include "datasource.h"
#include "datasourcepluginmanager.h"
#include "objectstore.h"
#include "colorsequence.h"
#ifdef KST_HAVE_REVISION_H
#include "kstrevision.h"
#endif

#include <iostream>
#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>

#include "curve.h"
#include "psd.h"
#include "histogram.h"
#include "datamatrix.h"
#include "image.h"
#include "palette.h"

#include "updatemanager.h"
#include "dialogdefaults.h"
#include "datasourcepluginmanager.h"

namespace Kst {

  static const char *usageMessage =
"KST Command Line Usage\n"
"************************\n"
"*** Load a kst session: ***\n"
"kst [OPTIONS] <kstfile>\n"
"\n"
"[OPTIONS] will override the datasource parameters for all data sources in the kst file:\n"
"      -F  <datasource>\n"
"      -f  <startframe>\n"
"      -n  <numframes>\n"
"      -s  <frames per sample>\n"
"      -a                     (apply averaging filter: requires -s)\n\n"
"************************\n";

  static const char *usageDetailsMessage =
"*** Read a data file ***\n"
"kst [<datasource> [OPTIONS]]\n"
"\n"
"OPTIONS are read and interpreted in order. Except for data object options, all are applied to all future data objects, unless later overridden.\n"
"Output Options:\n"
"      --print <filename>       Print to file and exit.\n"
"      --landscape              Print in landscape mode.\n"
"      --portrait               Print in portrait mode.\n"
"      --Letter                 Print to Letter sized paper.\n"
"      --A4                     Print to A4 sized paper.\n"
"      --png <filename>         Render to a png image, and exit.\n"
"      --pngHeight <height>     Height of png image (pixels)\n"
"      --pngWidth <width>       Width of png image (pixels)\n"
"File Options:\n"
"      -f <startframe>          default: 'end' counts from end.\n"
"      -n <numframes>           default: 'end' reads to end of file\n"
"      -s <frames per sample>   default: 0 (read every sample)\n"
"      -a                       apply averaging filter: requires -s\n\n"
"Ascii File Options - for ascii files only: these are all stick\n"
"      --asciiDataStart <Line>  Data starts here. Files start at line 1.\n"
"      --asciiFieldNames <Line> Field names are in this row.\n"
"      --asciiNoFieldNames      Fields are named for their data column\n"
"      --asciiReadUnits <Line>  Read units from line <Line>\n"
"      --asciiNoUnits       Do not read units\n"
"      --asciiSpaceDelim        Columns are Space/tab delimited\n"
"      --asciiDelim <char>      Columns are dlimited with <char>\n"
"      --asciiFixedWidth <w>    Columns have width <w>\n"
"      --asciiNoFixedWidth      Columns are delimited, not fixed width\n"
"      --asciiDecimalDot        Use a . as a decimal separator (ie, 10.1)\n"
"      --asciiDecimalComma      Use a , as a decimal separator (ie, 10,1)\n"
"Position:\n"
"      -P <plot name>:          Place curves in one plot.\n"
"      -A                       Place future curves in individual plots.\n"
"      -m <columns>             Layout plots in columns\n"
"      -T <tab name>            Place future curves a new tab.\n"
"Appearance\n"
"      -d:                      use points for the next curve\n"
"      -l:                      use lines for the next curve\n"
"      -b:                      use bargraph for the next curve\n"
"      --xlabel <X Label>       Set X label of all future plots.\n"
"      --ylabel <Y Label>       Set Y label of all future plots.\n"
"      --xlabelauto             AutoSet X label of all future plots.\n"
"      --ylabelauto             AutoSet Y label of all future plots.\n"
"Data Object Modifiers\n"
"      -x <field>:              Create vector and use as X vector for curves.\n"
"      -e <field>:              Create vector and use as Y-error vector for next -y.\n"
"      -r <rate>:               sample rate (spectra & spectograms).\n"
"Data Objects:\n"
"      -y <field>               plot an XY curve of field.\n"
"      -p <field>               plot the spectrum of field.\n"
"      -h <field>               plot a histogram of field.\n"
"      -z <field>               plot an image of matrix field.\n"
"Misc:\n"
"      --serverName=<server name> Set the server name for pyKst scrips connecting with this session.\n"
"\n"
"****************\n"
"*** Examples ***\n"
"\n"
"Data sources and fields:\n"
"Plot all data in column 2 from data.dat.\n"
"       kst data.dat -y 2\n"
"\n"
"Same as above, except only read 20 lines, starting at line 10.\n"
"       kst data.dat -f 10 -n 20 -y 2\n"
"\n"
"... also read col 1. One plot per curve.\n"
"       kst data.dat -f 10 -n 20 -y 1 -y 2\n"
"\n"
"Read col 1 from data2.dat and col 1 from data.dat\n"
"       kst data.dat -f 10 -n 20 -y 2 data2.dat -y 1\n"
"\n"
"Same as above, except read 40 lines starting at 30 in data2.dat\n"
"       kst data.dat -f 10 -n 20 -y 2 data2.dat -f 30 -n 40 -y 1\n"
"\n"
"Specify the X vector and error bars:\n"
"Plot x = col 1 and Y = col 2 and error flags = col 3 from data.dat\n"
"       kst data.dat -x 1 -e 3 -y 2\n"
"\n"
"Get the X vector from data1.dat, and the Y vector from data2.dat.\n"
"       kst data1.dat -x 1 data2.dat -y 1\n"
"\n"
"Placement:\n"
"Plot column 2 and column 3 in plot P1 and column 4 in plot P2\n"
"       kst data.dat -P P1 -y 2 -y 3 -P P2 -y 4\n";

static void printText(const QString& text, const QString& detailText = QString(), const QString& t = QString())
{
#ifdef Q_OS_WIN
  // No console on Windows.
  QMessageBox box(QMessageBox::Information, "Kst", text + t);
  if (!detailText.isEmpty()) {
    box.setDetailedText(detailText);
  }
  box.exec();
#else
  QString displayText = QString(text) + QString(detailText) + t;

  fprintf(stderr,"%s\n", qPrintable(displayText));
  //qWarning("%s", qPrintable(displayText));
#endif
}


void CommandLineParser::printUsage(const QString &t)
{
  printText(tr(usageMessage), tr(usageDetailsMessage), '\n' + t);
}


CommandLineParser::CommandLineParser(Document *doc, MainWindow* mw) :
      _mainWindow(mw),
      _doAve(false), _doSkip(false), _doConsecutivePlots(true), _useBargraph(false), 
      _useLines(true), _usePoints(false), _overrideStyle(false), _sampleRate(1.0), 
      _numFrames(-1), _startFrame(-1),
      _skip(0), _plotName(), _errorField(), _fileName(), _xField(QString("INDEX")),
      _pngFile(QString()), _pngWidth(-1), _pngHeight(-1), _printFile(QString()), _landscape(false), _plotItem(0),
      _num_cols(0), _asciiFirstLine(-1), _asciiFieldLine(-1), _asciiNoFieldNames(false),
      _asciiUnitsLine(-1), _asciiNoUnits(false), _asciiSpaceDelim(false),
      _asciiDelim('\0'), _asciiFixedWidth(-1), _asciiNoFixedWidth(false),
      _asciiDecimalDot(false), _asciiDecimalComma(false)
{

  Q_ASSERT(QCoreApplication::instance());
  _arguments = QCoreApplication::instance()->arguments();
  _arguments.takeFirst(); //appname

  _document = doc;

  _fileNames.clear();
  _vectors.clear();
  _plotItems.clear();
  _xlabel.clear();
  _ylabel.clear();
}


CommandLineParser::~CommandLineParser() {
}


bool CommandLineParser::_setIntArg(int *arg, QString Message, bool accept_end) {
  QString param;
  bool ok = true;

  if (_arguments.count()> 0) {
    param = _arguments.takeFirst();
    if ((param==tr("end") || (param=="end")) && (accept_end)) {
      *arg = -1;
    } else {
      *arg = param.toInt(&ok);
    }
  } else {
    ok=false;
  }
  if (!ok) printUsage(Message);
  return ok;
}


bool CommandLineParser::_setDoubleArg(double *arg, QString Message) {
  QString param;
  bool ok = true;
  
  if (_arguments.count()> 0) {
    param = _arguments.takeFirst();
    *arg = param.toDouble(&ok);
  } else {
    ok=false;
  }
  if (!ok) printUsage(Message);
  return ok;
}


bool CommandLineParser::_setStringArg(QString &arg, QString Message) {
  bool ok = true;
  if (_arguments.count()> 0) {
    arg = _arguments.takeFirst();
  } else {
    ok=false;
  }
  if (!ok) printUsage(Message);
  return ok;
}


DataVectorPtr CommandLineParser::createOrFindDataVector(QString field, DataSourcePtr ds) {
    DataVectorPtr xv;
    bool found = false;

    if ((_startFrame==-1) && (_numFrames==-1)) { // count from end and read to end
      _startFrame = 0;
    }
    // Flaky magic: if ds is an ascii file, change fields named 0 to 99 to
    // Column xx.  This allows "-y 2" but prevents ascii files with fields
    // actually named "0 to 99" from being read from the command line.
    if (ds->fileType() == "ASCII file") {
      QRegExp num("^[0-9]{1,2}$");
      if (num.exactMatch(field)) {
        field = ds->vector().list()[field.toInt()];
      }
    }
    // check to see if an identical vector already exists.  If so, use it.
    for (int i=0; i<_vectors.count(); i++) {
      xv = _vectors.at(i);
      if (field == xv->field()) {
        if ((xv->reqStartFrame() == _startFrame) &&
            (xv->reqNumFrames() == _numFrames) &&
            (xv->skip() == _skip) &&
            (xv->doSkip() == (_skip>0)) &&
            (xv->doAve() == _doAve) ){
          if (xv->filename()==ds->fileName()) {
            found = true;
            break;
          }
        }
      }
    }

    if (!found) {
      Q_ASSERT(_document && _document->objectStore());

      xv = _document->objectStore()->createObject<DataVector>();

      xv->writeLock();
      xv->change(ds, field, _startFrame, _numFrames, _skip, _skip>0, _doAve);

      xv->registerChange();
      xv->unlock();

      _vectors.append(xv);

    }

    return xv;
}

void CommandLineParser::createCurveInPlot(VectorPtr xv, VectorPtr yv, VectorPtr ev) {
    CurvePtr curve = _document->objectStore()->createObject<Curve>();

    curve->setXVector(xv);
    curve->setYVector(yv);
    curve->setXError(0);
    curve->setXMinusError(0);
    curve->setColor(ColorSequence::self().next());
    curve->setHasPoints(_usePoints);
    curve->setHasLines(_useLines);
    curve->setHasBars(_useBargraph);
    curve->setLineWidth(dialogDefaults().value("curves/lineWidth",0).toInt());
    //curve->setPointType(ptype++ % KSTPOINT_MAXTYPE);

    if (ev) {
      curve->setYError(ev);
      curve->setYMinusError(ev);
    } else {
      curve->setYError(0);
      curve->setYMinusError(0);
    }

    curve->writeLock();
    curve->registerChange();
    curve->unlock();

    addCurve(curve);
}

void CommandLineParser::addCurve(CurvePtr curve)
{
    if (_doConsecutivePlots) {
      CreatePlotForCurve *cmd = new CreatePlotForCurve();
      cmd->createItem();
      _plotItem = static_cast<PlotItem*>(cmd->item());
      _plotItem->view()->appendToLayout(CurvePlacement::Auto, _plotItem);
      applyLabels();
    }
    PlotRenderItem *renderItem = _plotItem->renderItem(PlotRenderItem::Cartesian);
    renderItem->addRelation(kst_cast<Relation>(curve));
    if (renderItem->relationList().size()>1) {
      _plotItem->setShowLegend(true,true);
    }
    _plotItem->update();
}

void CommandLineParser::createImageInPlot(MatrixPtr m) {
    ImagePtr image = _document->objectStore()->createObject<Image>();

    image->changeToColorOnly(m, 0.0, 1.0, true, Palette::getPaletteList().at(0));

    image->writeLock();
    image->registerChange();
    image->unlock();

    if (_doConsecutivePlots) {
      CreatePlotForCurve *cmd = new CreatePlotForCurve();
      cmd->createItem();
      _plotItem = static_cast<PlotItem*>(cmd->item());
      _plotItem->view()->appendToLayout(CurvePlacement::Auto, _plotItem);
      applyLabels();
    }
    PlotRenderItem *renderItem = _plotItem->renderItem(PlotRenderItem::Cartesian);
    renderItem->addRelation(kst_cast<Relation>(image));
    _plotItem->update();
}

void CommandLineParser::createOrFindTab(const QString name) {
  bool found = false;
  int i;
  int n_tabs = _mainWindow->tabWidget()->count();

  for (i=0; i<n_tabs; i++) {
    if (_mainWindow->tabWidget()->tabText(i) == name) {
      found = true;
      _mainWindow->tabWidget()->setCurrentIndex(i);
      return;
    }
  }

  if (!found) {
    _mainWindow->tabWidget()->createView();
    _mainWindow->tabWidget()->setCurrentViewName(name);
  }
}

void CommandLineParser::createOrFindPlot( const QString plot_name ) {
    bool found = false;
    PlotItem *pi;

    // check to see if a plot with this name exists.  If so, use it.
    for (int i=0; i<_plotItems.count(); i++) {
      pi = _plotItems.at(i);
      if (plot_name == pi->descriptiveName()) {
        found = true;
        break;
      }
    }

    if (!found) {

      CreatePlotForCurve *cmd = new CreatePlotForCurve();
      cmd->createItem();
      pi = static_cast<PlotItem*> ( cmd->item() );

      pi->setDescriptiveName( plot_name );
      _plotItems.append(pi);
      pi->view()->appendToLayout(CurvePlacement::Auto, pi);
      _plotItem = pi;
      applyLabels();
    }
    _plotItem = pi;

}

void CommandLineParser::applyLabels() {
  if (!_plotItem) {
    return;
  }

  if (!_xlabel.isEmpty()) {
    _plotItem->bottomLabelDetails()->setText(_xlabel);
    _plotItem->bottomLabelDetails()->setIsAuto(false);
  }
  if (!_ylabel.isEmpty()) {
    _plotItem->leftLabelDetails()->setText(_ylabel);
    _plotItem->leftLabelDetails()->setIsAuto(false);
  }

}

QString CommandLineParser::kstFileName() {
  if (_fileNames.size()>0) {
    return (_fileNames.at(0));
  } else {
    return QString();
  }
}

bool CommandLineParser::processCommandLine(bool *ok) {
  QString arg, param;
  *ok=true;
  bool new_fileList=true;
  bool dataPlotted = false;
  DataVectorPtr xv = 0L;
  bool use_old_xv = false;

#ifndef KST_NO_PRINTER
  // set paper settings to match defaults.
  _paperSize = QPrinter::PaperSize(dialogDefaults().value("print/paperSize", QPrinter::Letter).toInt());
  if (dialogDefaults().value("print/landscape",true).toBool()) {
    _landscape = true;
  } else {
    _landscape = false;
  }
#endif

  while (*ok) {
    if (_arguments.count() < 1) {
      break;
    }

    arg = _arguments.takeFirst();
    if ((arg == "--help")||(arg == "-help")) {
      printUsage(QString());
      *ok = false;
    } else if (arg == "--version" || arg == "-version") {

      printText(QString("Kst ") + KSTVERSION
#ifdef KST_REVISION
+ " Revision " + KST_REVISION
#endif
);

      *ok = false;
    } else if (arg == "-f") {
      *ok = _setIntArg(&_startFrame, tr("Usage: -f <startframe>\n"), true);
      _document->objectStore()->override.f0 = _startFrame;
    } else if (arg == "-n") {
      *ok = _setIntArg(&_numFrames, tr("Usage: -n <numframes>\n"), true);
      _document->objectStore()->override.N = _numFrames;
    } else if (arg == "-s") {
      *ok = _setIntArg(&_skip, tr("Usage: -s <frames per sample>\n"));
      _document->objectStore()->override.skip = _skip;
    } else if (arg == "-a") {
      _doAve = true;
      _document->objectStore()->override.doAve = _doAve;
    } else if (arg == "-P") {
      QString plot_name;
      *ok = _setStringArg(plot_name,tr("Usage: -P <plotname>\n"));
      _doConsecutivePlots=false;
      createOrFindPlot(plot_name);
    } else if (arg == "-A") {
      _doConsecutivePlots = true;
    } else if (arg == "-T") {
      QString tab_name;
      _doConsecutivePlots = true;
      *ok = _setStringArg(tab_name,tr("Usage: -T <tab name>\n"));
      if (dataPlotted) {
        if (_num_cols > 0) {
          _mainWindow->tabWidget()->currentView()->createLayout(false, _num_cols);
        }
        createOrFindTab(tab_name);
      } else {
        _mainWindow->tabWidget()->setCurrentViewName(tab_name);
      }
    } else if (arg == "-m") {
      *ok = _setIntArg(&_num_cols, tr("Usage: -m <columns>\n"), true);
    } else if (arg == "-d") {
      _useBargraph=false;
      _useLines = false;
      _usePoints = true;
      _overrideStyle = true;
    } else if (arg == "-l") {
      _useBargraph=false;
      _useLines = true;
      _usePoints = false;
      _overrideStyle = true;
    } else if (arg == "-b") {
      _useBargraph=true;
      _useLines = false;
      _usePoints = false;
      _overrideStyle = true;
    } else if (arg == "-x") {
      *ok = _setStringArg(_xField,tr("Usage: -x <xfieldname>\n"));
      for (int i_file=0; i_file<_fileNames.size(); i_file++) {
        QString file = _fileNames.at(i_file);
        QFileInfo info(file);
        if (!info.exists()) {
          printUsage(tr("file %1 does not exist\n").arg(file));
          *ok = false;
          break;
        }

        DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(_document->objectStore(), file);
        xv = createOrFindDataVector(_xField, ds);
        use_old_xv = false;
      }
      new_fileList = true;
    } else if (arg == "-e") {
      *ok = _setStringArg(_errorField,tr("Usage: -e <errorfieldname>\n"));
    } else if (arg == "-r") {
      *ok = _setDoubleArg(&_sampleRate,tr("Usage: -r <samplerate>\n"));
    } else if (arg == "-y") {
      QString field;
      *ok = _setStringArg(field,tr("Usage: -y <fieldname>\n"));

      if (_fileNames.size()<1) {
        printUsage(tr("No data files specified\n"));
        *ok = false;
        break;
      }
      for (int i_file=0; i_file<_fileNames.size(); i_file++) { 
        QString file = _fileNames.at(i_file);
        QFileInfo info(file);
        if (!info.exists()) {
          printUsage(tr("file %1 does not exist\n").arg(file));
          *ok = false;
          break;
        }

        DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(_document->objectStore(), file);
        if (!xv || !use_old_xv) {
          xv = createOrFindDataVector(_xField, ds);
        }
        DataVectorPtr yv = createOrFindDataVector(field, ds);

        /*
        DataSourcePluginManager::settingsObject().beginGroup("ASCII file");
        DataSourcePluginManager::settingsObject().beginGroup(file);
        qDebug()  << "ds settings: " << file << DataSourcePluginManager::settingsObject().allKeys();
        qDebug()  << "Use Dot: " << file << DataSourcePluginManager::settingsObject().value("Use Dot");
        DataSourcePluginManager::settingsObject().endGroup();
        DataSourcePluginManager::settingsObject().endGroup();
        */

        DataVectorPtr ev;
        if (!_errorField.isEmpty()) {
          ev = createOrFindDataVector(_errorField, ds);
          if (!_overrideStyle) {
            _useBargraph=false;
            _useLines = false;
            _usePoints = true;
          }
        } else {
          ev = 0;
          if (!_overrideStyle) {
            _useBargraph=false;
            _useLines = true;
            _usePoints = false;
          }

        }

        createCurveInPlot(xv, yv, ev);
        dataPlotted = true;
      }

      _errorField.clear();
      new_fileList = true;
      _overrideStyle = false;
    } else if (arg == "-p") {
      QString field;
      *ok = _setStringArg(field,tr("Usage: -p <fieldname>\n"));

      if (*ok) {
        for (int i_file=0; i_file<_fileNames.size(); i_file++) {
          QString file = _fileNames.at(i_file);
          QFileInfo info(file);
          if (!info.exists()) {
            printUsage(tr("file %1 does not exist\n").arg(file));
            *ok = false;
            break;
          }

          DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(_document->objectStore(), file);

          DataVectorPtr pv = createOrFindDataVector(field, ds);

          Q_ASSERT(_document && _document->objectStore());
          PSDPtr powerspectrum = _document->objectStore()->createObject<PSD>();
          Q_ASSERT(powerspectrum);

          powerspectrum->writeLock();
          powerspectrum->change(pv, _sampleRate, true, 14, true, true, QString(), QString());
          powerspectrum->registerChange();
          powerspectrum->unlock();

          VectorPtr ev=0;

          if ( !_overrideStyle ) {
              _useBargraph=false;
              _useLines = true;
              _usePoints = false;
          }

          createCurveInPlot(powerspectrum->vX(), powerspectrum->vY(), ev);
          dataPlotted = true;
        }
        new_fileList = true;
        _overrideStyle = false;
      }
    } else if (arg == "--xlabel") {
      *ok = _setStringArg(_xlabel, "Usage -xlabel <label>\n");
    } else if (arg == "--ylabel") {
      *ok = _setStringArg(_ylabel, "Usage -ylabel <label>\n");
    } else if (arg == "--xlabelauto") {
      _xlabel.clear();
    } else if (arg == "--ylabelauto") {
      _ylabel.clear();
    } else if (arg == "-h") {
      QString field;
      *ok = _setStringArg(field,tr("Usage: -h <fieldname>\n"));

      if (*ok) {
        for ( int i_file=0; i_file<_fileNames.size(); i_file++ ) {
          QString file = _fileNames.at ( i_file );
          QFileInfo info ( file );
          if ( !info.exists() || !info.isFile() ) {
            printUsage ( tr ( "file %1 does not exist\n" ).arg ( file ) );
            *ok = false;
            break;
          }

          DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource ( _document->objectStore(), file );

          DataVectorPtr hv = createOrFindDataVector ( field, ds );
          Q_ASSERT ( _document && _document->objectStore() );
          HistogramPtr histogram = _document->objectStore()->createObject<Histogram> ();

          histogram->change(hv, -1, 1, 60, Histogram::Number, true);

          histogram->writeLock();
          histogram->registerChange();
          histogram->unlock();

          VectorPtr ev=0;

          if ( !_overrideStyle ) {
              _useBargraph=true;
              _useLines = false;
              _usePoints = false;
          }

          createCurveInPlot(histogram->vX(), histogram->vY(), ev);
          dataPlotted = true;
        }

        new_fileList = true;
        _overrideStyle = false;
      }
    } else if (arg == "-z") {
      QString field;
      *ok = _setStringArg(field,tr("Usage: -z <fieldname>\n"));
      if (*ok) {
        for (int i_file=0; i_file<_fileNames.size(); i_file++) {
          QString file = _fileNames.at(i_file);
          QFileInfo info(file);
          if (!info.exists() || !info.isFile()) {
            printUsage(tr("file %1 does not exist\n").arg(file));
            *ok = false;
            break;
          }

          DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(_document->objectStore(), file);

          DataMatrixPtr dm = _document->objectStore()->createObject<DataMatrix>();

          dm->writeLock();
          dm->change(ds, field, 0, 0, -1, -1, _doAve, _skip>0, _skip, _startFrame, 0.0, 0.0, 1.0, 1.0);

          dm->registerChange();
          dm->unlock();

          createImageInPlot(dm);
        }
        new_fileList = true;
        dataPlotted = true;
      }
    } else if (arg == "-F") {
      *ok = _setStringArg(_document->objectStore()->override.fileName, tr("Usage: -F <datafile>\n"));
    } else if (arg == "--asciiDataStart") {
      *ok = _setIntArg(&_asciiFirstLine, tr("Usage: --asciiDataStart <Line Number>\n"));
    } else if (arg == "--asciiFieldNames") {
      *ok = _setIntArg(&_asciiFieldLine, tr("Usage: --asciiFieldNames <Line Number>\n"));
    } else if  (arg == "--asciiNoFieldNames") {
      _asciiNoFieldNames = true;
    } else if (arg == "--asciiReadUnits") {
      *ok = _setIntArg(&_asciiUnitsLine, tr("Usage: --asciiReadUnits <Line Number>\n"));
    } else if  (arg == "--asciiNoUnits") {
      _asciiNoUnits = true;
    } else if (arg == "--asciiSpaceDelim") {
      _asciiSpaceDelim = true;
    } else if (arg == "--asciiDelim") {
      QString arg;
      *ok = _setStringArg(arg, tr("Usage: --asciiDelim <delimiter>\n"));
      _asciiDelim = arg.at(0).toAscii();
    } else if (arg == "--asciiFixedWidth") {
      *ok = _setIntArg(&_asciiFixedWidth, tr("Usage: --asciiFixedWidth <width>\n"));
    } else if (arg == "--asciiNoFixedWidth") {
      _asciiNoFixedWidth = true;
    } else if (arg == "--asciiDecimalDot") {
      _asciiDecimalDot = true;
    } else if (arg == "--asciiDecimalComma") {
      _asciiDecimalComma = true;
    } else if (arg == "--png") {
      *ok = _setStringArg(_pngFile, tr("Usage: --png <filename>\n"));
    } else if (arg == "--pngWidth") {
      *ok = _setIntArg(&_pngWidth, tr("Usage: --pngWidth <width>\n"));
    } else if (arg == "--pngHeight") {
      *ok = _setIntArg(&_pngHeight, tr("Usage: --pngHeight <height>\n"));
#ifndef KST_NO_PRINTER
    } else if (arg == "--print") {
      *ok = _setStringArg(_printFile, tr("Usage: --print <filename>\n"));
    } else if (arg == "--landscape") {
      _landscape = true;
    } else if (arg == "--portrait") {
      _landscape = false;
    } else if (arg == "--A4") {
      _paperSize = QPrinter::A4;
    } else if (arg == "--letter") {
      _paperSize = QPrinter::Letter;
#endif
    } else if (arg.startsWith("--serverName=")) {
      /* scriptServer has already handled this.  Skip it. */
    } else { // arg is not an option... must be a file
      if (new_fileList) { // if the file list has been used, clear it.
        if (dataPlotted) {
          _document->updateRecentDataFiles(_fileNames);
        }
        _fileNames.clear();
        new_fileList = false;
        use_old_xv = true;
      }
      _fileNames.append(arg);
      if (!arg.endsWith(".kst")) {
        DataSourcePluginManager::settingsObject().beginGroup("ASCII file");
        DataSourcePluginManager::settingsObject().beginGroup(arg);

        if (_asciiFirstLine>0) {
          DataSourcePluginManager::settingsObject().setValue("Data Start", _asciiFirstLine-1);
        }
        if (_asciiFieldLine>0) {
          DataSourcePluginManager::settingsObject().setValue("Fields Line", _asciiFieldLine-1);
          DataSourcePluginManager::settingsObject().setValue("Read Fields", true);
        }
        if (_asciiNoFieldNames) {
          DataSourcePluginManager::settingsObject().setValue("Read Fields", false);
        }
        if (_asciiUnitsLine>0) {
          DataSourcePluginManager::settingsObject().setValue("Units Line", _asciiUnitsLine-1);
          DataSourcePluginManager::settingsObject().setValue("Read Units", true);
        }
        if (_asciiNoUnits) {
          DataSourcePluginManager::settingsObject().setValue("Read Units", false);
        }
        if (_asciiDelim != '\0') {
          DataSourcePluginManager::settingsObject().setValue("Column Delimiter", QString(_asciiDelim));
          DataSourcePluginManager::settingsObject().setValue("Column Type", 2); //FIXME: AsciiSourceConfig::ColumnType::Custom
        }
        if (_asciiSpaceDelim) {
          DataSourcePluginManager::settingsObject().setValue("Column Type", 0); //FIXME: AsciiSourceConfig::ColumnType::Whitespace
        }
        if (_asciiFixedWidth>0) {
          DataSourcePluginManager::settingsObject().setValue("Column Type", 1); //FIXME: AsciiSourceConfig::ColumnType::Fixed
          DataSourcePluginManager::settingsObject().setValue("Column Width", _asciiFixedWidth);
          DataSourcePluginManager::settingsObject().setValue("Column Width is const", true);
        }
        if (_asciiNoFixedWidth) {
          DataSourcePluginManager::settingsObject().setValue("Column Width is const", false);
        }
        if (_asciiDecimalDot) {
          DataSourcePluginManager::settingsObject().setValue("Use Dot", true);
        }
        if (_asciiDecimalComma) {
          DataSourcePluginManager::settingsObject().setValue("Use Dot", false);
        }

        DataSourcePluginManager::settingsObject().endGroup();
        DataSourcePluginManager::settingsObject().endGroup();
      }

      if (!arg.endsWith(".kst") && _arguments.count() == 0) {
        // try loading data without user interaction
        DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(_document->objectStore(), arg);
        if (ds) {
          ObjectList<Object> curves = ds->autoCurves(*_document->objectStore());
          if (curves.isEmpty()) {
            curves = autoCurves(ds);
          }
          int curve_count = 0;
          foreach(const ObjectPtr& ptr, curves) {
            if (kst_cast<Curve>(ptr)) {
              curve_count++;
            }
          }
          if (curve_count > 0) {
            _mainWindow->updateRecentDataFiles(arg);
            int count = 0;
            const int max_count = 6 * 6;
            bool asked = false;
            foreach(const ObjectPtr& ptr, curves) {
              if (kst_cast<Curve>(ptr)) {
                if (!asked && count >= max_count) {
                  asked = true;
                  QMessageBox::StandardButton res = QMessageBox::question(0, "Kst reading datafile", QString(
                    "Kst found %1 Curves in the specified data file.\n"
                    "Should Kst plot all %1 curves?\n"
                    "If not, Kst plots only %2 curves.").arg(curve_count).arg(max_count),
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                  if (res == QMessageBox::No) {
                    break;
                  }
                }
                addCurve(kst_cast<Curve>(ptr));
                count++;
              }
            }
          }
        }
      }
    }
  }
  if (dataPlotted) {
    if (_num_cols > 0) {
      _mainWindow->tabWidget()->currentView()->createLayout(false, _num_cols);
      _mainWindow->tabWidget()->currentView()->createLayout(false);
    }
    _document->updateRecentDataFiles(_fileNames);

    dialogDefaults().setValue("vector/range", _numFrames);
    dialogDefaults().setValue("vector/start", _startFrame);
    dialogDefaults().setValue("vector/countFromEnd", (_startFrame<0));
    dialogDefaults().setValue("vector/readToEnd", (_numFrames<0));
    dialogDefaults().setValue("vector/skip", _skip);
    dialogDefaults().setValue("vector/doSkip", (_skip>0));
    dialogDefaults().setValue("vector/doAve", _doAve);
  }

#ifndef KST_NO_PRINTER
  // set defaults to match what has been set.
  dialogDefaults().setValue("print/landscape", _landscape);
  dialogDefaults().setValue("print/paperSize", int(_paperSize));
#endif

  if (_plotItem) {
    _plotItem->view()->resetPlotFontSizes();
  }
  UpdateManager::self()->doUpdates(true);

  return (dataPlotted);
}


Kst::ObjectList<Kst::Object> CommandLineParser::autoCurves(DataSourcePtr ds)
{
  QStringList fieldList = ds->vector().list();

  if (fieldList.isEmpty()) {
    return ObjectList<Kst::Object>();
  }

  ObjectList<Kst::Object> curves;

  DataVectorPtr xv = _document->objectStore()->createObject<DataVector>();
  xv->writeLock();
  xv->change(ds, "INDEX", 0, -1, 0, false, false);
  xv->registerChange();
  xv->unlock();

  foreach(const QString& field, fieldList) {
    if (field != "INDEX") {
      DataVectorPtr yv= _document->objectStore()->createObject<DataVector>();
      yv->writeLock();
      yv->change(ds, field, 0, -1, 0, false, false);
      yv->registerChange();
      yv->unlock();

      CurvePtr curve = _document->objectStore()->createObject<Curve>();
      curve->setXVector(xv);
      curve->setYVector(yv);
      curve->setXError(0);
      curve->setXMinusError(0);
      curve->setYMinusError(0);
      curve->setColor(Kst::ColorSequence::self().next());
      curve->setLineWidth(1); 

      curve->writeLock();
      curve->registerChange();
      curve->unlock();

      curves << curve;
    }
  }
  return curves;
}

}
