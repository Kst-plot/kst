//
// C++ Implementation: CommandLineParser
//
// Description: 
//
//
// Author: Barth Netterfield <netterfield@physics.utoronto.ca>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "commandlineparser.h"
#include "datasource.h"
#include "objectstore.h"
#include "colorsequence.h"

#include <iostream>
#include <QCoreApplication>
#include <QFileInfo>
#include "curve.h"
#include "psd.h"
#include "histogram.h"
#include "kst_i18n.h"

namespace Kst {

CommandLineParser::CommandLineParser(Document *doc):
      _doAve(false), _doSkip(false), _doConsecutivePlots(true), _useBargraph(false), 
      _useLines(true), _usePoints(false), _overrideStyle(false), _sampleRate(1.0), 
      _numFrames(-1), _startFrame(-1),
      _skip(0), _plotName(), _errorField(), _fileName(), _xField(QString("INDEX")) {

  Q_ASSERT(QCoreApplication::instance());
  _arguments = QCoreApplication::instance()->arguments();
  _arguments.takeFirst(); //appname

  _document = doc;

  _fileNames.clear();
  _vectors.clear();
  _plotNames.clear();
  _plotItems.clear();
}


CommandLineParser::~CommandLineParser() {
}

void CommandLineParser::usage(QString Message) {
  //FIXME: proper printing and exiting!
  std::cerr <<
  i18n("KST Command Line Usage\n"
"Load a kst file:\n"
"kst [OPTIONS] kstfile\n"
"\n"
"[OPTIONS] will override the datasource parameters for all data sources in the kst file:\n"
"      -F  <datasource>\n"
"      -f  <startframe>\n"
"      -n  <numframes>\n"
"      -s  <frames per sample>\n"
"      -a                     (apply averaging filter: requires -s)\n"
"Read a data file:\n"
"kst datasource OPTIONS [datasource OPTIONS []]\n"
"\n"
"OPTIONS are read and interpreted in order. Except for data object options, all are applied to all future data objects, unless later overridden.\n"
"File Options:\n"
"      -f <startframe>          default: 'end' counts from end.\n"
"      -n <numframes>           default: 'end' reads to end of file\n"
"      -s <frames per sample>   default: 0 (read every sample)\n"
"      -a                       apply averaging filter: requires -s\n"
"Position:\n"
"      -P <plot name>:          Place curves in one plot.\n"
"      -A                       Place future curves in individual plots.\n"
"Appearance\n"
"      -d:                      use points for the next curve\n"
"      -l:                      use lines for the next curve\n"
"      -b:                      use bargraph for the next curve\n"
"Data Object Modifiers\n"
"      -x <field>:              Create vector and use as X vector for curves.\n"
"      -e <field>:              Create vector and use as Y-error vector for next -y.\n"
"      -r <rate>:               sample rate (spectra & spectograms).\n"
"Data Objects:\n"
"      -y <field>               plot an XY curve of field.\n"
"      -p <field>               plot the spectrum of field.\n"
"      -h <field>               plot a histogram of field.\n"
"      -z <field>               plot an image of matrix field.\n"
"Examples:\n"
"Data sources and fields\n"
"Plot all data in column 2 from data.dat.\n"
"       kst data.dat -y 2\n"
"\n"
"Same as above, except only read 20 lines, starting at line 10.\n"
"       kst data.dat -f 10 -n 20 -y 2\n"
"\n"
"...also read col 1. One plot per curve.\n"
"       kst data.dat -f 10 -n 20 -y 1 -y 2\n"
"\n"
"...read col 1 from data2.dat and col 1 from data.dat\n"
"       kst data.dat -f 10 -n 20 -y 2 data2.dat -y 1\n"
"\n"
"...same as above, except read 40 lines starting at 30 in data2.dat\n"
"       kst data.dat -f 10 -n 20 -y 2 data2.dat -f 30 -n 40 -y 1\n"
"Specify the X vector and error bars.\n"
"Plot x = col 1 and Y = col 2 and error flags = col 3 from data.dat\n"
"       kst data.dat -x 1 -e 3 -y 2\n"
"\n"
"Get the X vector from data1.dat, and the Y vector from data2.dat.\n"
"       kst data1.dat -x 1 data2.dat -y 1\n"
"Placement:\n"
"Plot column 2 and column 3 in plot P1 and column 4 in plot P2\n"
"       kst data.dat -P P1 -y 2 -y 3 -P P2 -y 4\n"
      ).latin1();

  std::cerr << Message.latin1();

  exit(0);
}

void CommandLineParser::_setIntArg(int *arg, QString Message, bool accept_end) {
  QString param;
  bool ok = true;

  if (_arguments.count()> 0) {
    param = _arguments.takeFirst();
    if ((param==i18n("end") || (param=="end")) && (accept_end)) {
      *arg = -1;
    } else {
      *arg = param.toInt(&ok);
    }
  } else {
    ok=false;
  }
  if (!ok) usage(Message);

}

void CommandLineParser::_setDoubleArg(double *arg, QString Message) {
  QString param;
  bool ok = true;
  
  if (_arguments.count()> 0) {
    param = _arguments.takeFirst();
    *arg = param.toDouble(&ok);
  } else {
    ok=false;
  }
  if (!ok) usage(Message);
}

void CommandLineParser::_setStringArg(QString &arg, QString Message) {
  bool ok = true;

  if (_arguments.count()> 0) {
    arg = _arguments.takeFirst();
  } else {
    ok=false;
  }
  if (!ok) usage(Message);

}

DataVectorPtr CommandLineParser::createOrFindDataVector(QString field, DataSourcePtr ds) {
    DataVectorPtr xv;
    bool found = false;

    if ((_startFrame==-1) && (_numFrames==-1)) { // count from end and read to end
      _startFrame = 0;
    }

    // check to see if an identical vector already exists.  If so, use it.
    for (int i=0; i<_vectors.count(); i++) {
      xv = _vectors.at(i);
      if (field == xv->field()) {
        if ((xv->startFrame() == _startFrame) &&
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

      xv->update();
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
    curve->setYMinusError(0);
    curve->setColor(ColorSequence::next());
    curve->setHasPoints(_usePoints);
    curve->setHasLines(_useLines);
    curve->setHasBars(_useBargraph);
    curve->setLineWidth(1); //FIXME: use defaults
    //curve->setPointType(ptype++ % KSTPOINT_MAXTYPE);

    if (ev) {
      curve->setYError(ev);
    } else {
      curve->setYError(0);
    }

    curve->writeLock();
    curve->update();
    curve->unlock();

    if (_doConsecutivePlots) {
      CreatePlotForCurve *cmd = new CreatePlotForCurve();
      cmd->createItem();
      _plotItem = static_cast<PlotItem*>(cmd->item());
      _plotItem->setName(QString("P-")+curve->Name());
    }
    PlotRenderItem *renderItem = _plotItem->renderItem(PlotRenderItem::Cartesian);
    renderItem->addRelation(kst_cast<Relation>(curve));
    _plotItem->update();

    _plotItem->parentView()->appendToLayout(CurvePlacement::Auto, _plotItem);
}

void CommandLineParser::createOrFindPlot( const QString plot_name ) {
    bool found = false;
    PlotItem *pi;

    // check to see if a plot with this name exists.  If so, use it.
    for (int i=0; i<_plotItems.count(); i++) {
      pi = _plotItems.at(i);
      if (plot_name == pi->name()) {
        found = true;
        break;
      }
    }

    if (!found) {

      CreatePlotForCurve *cmd = new CreatePlotForCurve();
      cmd->createItem();
      pi = static_cast<PlotItem*> ( cmd->item() );

      pi->setName ( plot_name );
      _plotNames.append(plot_name);
      _plotItems.append(pi);
    }
    _plotItem = pi;
}

QString CommandLineParser::kstFileName() {
  if (_fileNames.size()>0) {
    return (_fileNames.at(0));
  } else {
    return QString();
  }
}

bool CommandLineParser::processCommandLine() {
  QString arg, param;
  bool ok=true;
  bool new_fileList=true;
  bool dataPlotted = false;

  while (1) {
    if (_arguments.count()<1) break;

    arg = _arguments.takeFirst();
    ok = true;
    if ((arg == "--help")||(arg == "-help")) {
      usage();
    } else if (arg == "-f") {
      _setIntArg(&_startFrame, i18n("Usage: -f <startframe>\n"), true);
    } else if (arg == "-n") {
      _setIntArg(&_numFrames, i18n("Usage: -f <numframes>\n"), true);
    } else if (arg == "-s") {
      _setIntArg(&_skip, i18n("Usage: -s <frames per sample>\n"));
    } else if (arg == "-a") {
      _doAve = true;
    } else if (arg == "-P") {
      QString plot_name;
      _setStringArg(plot_name,i18n("Usage: -P <plotname>\n"));
      _doConsecutivePlots=false;

      createOrFindPlot(plot_name);
    } else if (arg == "-A") {
      _doConsecutivePlots = true;
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
      _setStringArg(_xField,i18n("Usage: -x <xfieldname>\n"));
    } else if (arg == "-e") {
      _setStringArg(_errorField,i18n("Usage: -e <errorfieldname>\n"));
    } else if (arg == "-r") {
      _setDoubleArg(&_sampleRate,i18n("Usage: -r <samplerate>\n"));
    } else if (arg == "-y") {
      QString field;
      _setStringArg(field,i18n("Usage: -y <fieldname>\n"));

      if (_fileNames.size()<1) {
        usage(i18n("No data files specified\n"));
      }
      for (int i_file=0; i_file<_fileNames.size(); i_file++) { 
        QString file = _fileNames.at(i_file);
        QFileInfo info(file);
        if (!info.exists())
          usage(i18n("file %1 does not exist\n").arg(file));

        DataSourcePtr ds = DataSource::findOrLoadSource(_document->objectStore(), file);
        DataVectorPtr xv = createOrFindDataVector(_xField, ds);
        DataVectorPtr yv = createOrFindDataVector(field, ds);

        DataVectorPtr ev;
        if (!_errorField.isEmpty()) {
          DataVectorPtr ev = createOrFindDataVector(_errorField, ds);
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

      _errorField = QString();
      new_fileList = true;
      _overrideStyle = false;
    } else if (arg == "-p") {
      QString field;
      _setStringArg(field,i18n("Usage: -p <fieldname>\n"));

      for (int i_file=0; i_file<_fileNames.size(); i_file++) { 
        QString file = _fileNames.at(i_file);
        QFileInfo info(file);
        if (!info.exists() || !info.isFile())
          usage(i18n("file %1 does not exist\n").arg(file));

        DataSourcePtr ds = DataSource::findOrLoadSource(_document->objectStore(), file);

        DataVectorPtr pv = createOrFindDataVector(field, ds);

        Q_ASSERT(_document && _document->objectStore());
        PSDPtr powerspectrum = _document->objectStore()->createObject<PSD>();
        Q_ASSERT(powerspectrum);

        powerspectrum->writeLock();
        powerspectrum->change(pv, _sampleRate, true, 14, true, true, QString(), QString());
        powerspectrum->update();
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

    } else if (arg == "-h") {
      QString field;
      _setStringArg(field,i18n("Usage: -h <fieldname>\n"));

      for ( int i_file=0; i_file<_fileNames.size(); i_file++ ) {
        QString file = _fileNames.at ( i_file );
        QFileInfo info ( file );
        if ( !info.exists() || !info.isFile() )
            usage ( i18n ( "file %1 does not exist\n" ).arg ( file ) );

        DataSourcePtr ds = DataSource::findOrLoadSource ( _document->objectStore(), file );

        DataVectorPtr hv = createOrFindDataVector ( field, ds );
        Q_ASSERT ( _document && _document->objectStore() );
        HistogramPtr histogram = _document->objectStore()->createObject<Histogram> ();

        histogram->change(hv, -1, 1, 60, Histogram::Number, true);

        histogram->writeLock();
        histogram->update ();
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
    } else if (arg == "-z") {
      QString field;
      _setStringArg(field,i18n("Usage: -z <fieldname>\n"));
      //FIXME: Create the matrix, and the image
      new_fileList = true;
      dataPlotted = true;
    } else { // arg is not an option... must be a file
      if (new_fileList) { // if the file list has been used, clear it.
        _fileNames.clear();
        new_fileList = false;
      }
      _fileNames.append(arg);
    }
  }
  return (dataPlotted);
}

}
