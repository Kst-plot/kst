/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2012 Barth Netterfield                                *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "plotscriptinterface.h"
#include "plotitem.h"
#include "plotaxis.h"
#include "document.h"
#include "objectstore.h"

#include <QStringBuilder>

namespace Kst {

PlotSI::PlotSI(PlotItem *it) : _layout(new LayoutTabSI), _dim(new DimensionTabSI), _fill(new FillTabSI), _stroke(new StrokeTabSI) {
  _layout->vi=it;
  _dim->item=it;
  _fill->item=it;
  _stroke->item=it;
  _item = it;

  _fnMap.insert("addToCurrentView", &PlotSI::addToCurrentView);

  _fnMap.insert("addRelation", &PlotSI::addRelation);

  _fnMap.insert("setXRange",&PlotSI::setXRange);
  _fnMap.insert("setYRange",&PlotSI::setYRange);
  _fnMap.insert("setXAuto",&PlotSI::setXAuto);
  _fnMap.insert("setYAuto",&PlotSI::setYAuto);
  _fnMap.insert("setXAutoBorder",&PlotSI::setXAutoBorder);
  _fnMap.insert("setYAutoBorder",&PlotSI::setYAutoBorder);
  _fnMap.insert("setXNoSpike",&PlotSI::setXNoSpike);
  _fnMap.insert("setYNoSpike",&PlotSI::setYNoSpike);
  _fnMap.insert("setXAC",&PlotSI::setXAC);
  _fnMap.insert("setYAC",&PlotSI::setYAC);
  _fnMap.insert("setGlobalFont",&PlotSI::setGlobalFont);

  _fnMap.insert("setTopLabel",&PlotSI::setTopLabel);
  _fnMap.insert("setBottomLabel",&PlotSI::setBottomLabel);
  _fnMap.insert("setLeftLabel",&PlotSI::setLeftLabel);
  _fnMap.insert("setRightLabel",&PlotSI::setRightLabel);
  _fnMap.insert("setTopLabelAuto",&PlotSI::setTopLabelAuto);
  _fnMap.insert("setBottomLabelAuto",&PlotSI::setBottomLabelAuto);
  _fnMap.insert("setLeftLabelAuto",&PlotSI::setLeftLabelAuto);
  _fnMap.insert("setRightLabelAuto",&PlotSI::setRightLabelAuto);
  _fnMap.insert("setLogX",&PlotSI::setLogX);
  _fnMap.insert("setLogY",&PlotSI::setLogY);
  _fnMap.insert("normalizeXtoY",&PlotSI::normalizeXtoY);
  _fnMap.insert("setXAxisReversed",&PlotSI::setXAxisReversed);
  _fnMap.insert("setYAxisReversed",&PlotSI::setYAxisReversed);
  _fnMap.insert("setXAxisNotReversed",&PlotSI::setXAxisNotReversed);
  _fnMap.insert("setYAxisNotReversed",&PlotSI::setYAxisNotReversed);

  _fnMap.insert("setXAxisInterpretation",&PlotSI::setXAxisInterpretation);
  _fnMap.insert("clearXAxisInterpretation",&PlotSI::clearXAxisInterpretation);
  _fnMap.insert("setXAxisDisplay",&PlotSI::setXAxisDisplay);
}

ScriptInterface* PlotSI::newPlot() {
    PlotItem* bi=new PlotItem(kstApp->mainWindow()->tabWidget()->currentView());
    //kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new PlotSI(bi);
}


QString PlotSI::doCommand(QString x) {
  QString command = x.left(x.indexOf('('));

  InterfaceMemberFn fn=_fnMap.value(command,&PlotSI::noSuchFn);

  if(fn!=&PlotSI::noSuchFn) {
    return CALL_MEMBER_FN(*this,fn)(x);
  }

  QString v=doNamedObjectCommand(x, _dim->item);
  if(v.isEmpty()) {
    v = _layout->doCommand(x);
  }
  if(v.isEmpty()) {
    v=_dim->doCommand(x);
  }
  if(v.isEmpty()) {
    v=_stroke->doCommand(x);
  }
  if(v.isEmpty()) {
    v=_fill->doCommand(x);
  }
  return v.isEmpty()?"No command":v;
}

bool PlotSI::isValid() {
    return _dim->item;
}

/***************************/
/* commands                */
/***************************/
QString PlotSI::addToCurrentView(QString& command) {
  QStringList vars = getArgs(command);

  if (vars.at(0) == "Auto") {
    kstApp->mainWindow()->tabWidget()->currentView()->resetPlotFontSizes(_item);
    kstApp->mainWindow()->tabWidget()->currentView()->appendToLayout(CurvePlacement::Auto, _item, vars.at(1).toInt());
  } else if (vars.at(0) == "Columns") {
    kstApp->mainWindow()->tabWidget()->currentView()->resetPlotFontSizes(_item);
    kstApp->mainWindow()->tabWidget()->currentView()->appendToLayout(CurvePlacement::Custom, _item, vars.at(1).toInt());
    _item->createCustomLayout(vars.at(1).toInt());
  } else {
    kstApp->mainWindow()->tabWidget()->currentView()->appendToLayout(CurvePlacement::Protect, _item, vars.at(1).toInt());
  }
  return "Done";
}

QString PlotSI::addRelation(QString& command) {
  QString rname = getArg(command);
  RelationPtr relation = kst_cast<Relation>(
                           kstApp->mainWindow()->document()->objectStore()->retrieveObject(rname));
  if (relation) {
    _item->renderItem(PlotRenderItem::Cartesian)->addRelation(relation);
    return "Done";
  } else {
    return QString("Could not find curve/image %1").arg(rname);
  }
}

QString PlotSI::setXRange(QString &command) {

  QStringList vars = getArgs(command);

  double xmin = vars.at(0).toDouble();
  double xmax = vars.at(1).toDouble();
  if (xmin == xmax) {
    xmin -= 0.1;
    xmax += 0.1;
  }
  if (xmin > xmax) {
    double tmp = xmin;
    xmin = xmax;
    xmax = tmp;
  }

  double ymin = _item->yMin();
  double ymax = _item->yMax();

  QRectF R(QPointF(xmin,ymin), QPointF(xmax,ymax));

  _item->zoomXRange(R);

  return "Done.";
}

QString PlotSI::setYRange(QString &command) {
  QStringList vars = getArgs(command);

  if (_item) {
    double ymin = vars.at(0).toDouble();
    double ymax = vars.at(1).toDouble();
    if (ymin == ymax) {
      ymin -= 0.1;
      ymax += 0.1;
    }
    if (ymin > ymax) {
      double tmp = ymin;
      ymin = ymax;
      ymax = tmp;
    }

    double xmin = _item->xMin();
    double xmax = _item->xMax();

    QRectF R(QPointF(xmin,ymin), QPointF(xmax,ymax));

    _item->zoomYRange(R);

  }

  return "Done.";
}

QString PlotSI::setXAuto(QString &) {


  if (_item) {
    _item->zoomXMaximum();
  }
  return "Done.";
}


QString PlotSI::setYAuto(QString &) {


  if (_item) {
    _item->zoomYMaximum();
  }
  return "Done.";
}


QString PlotSI::setXAutoBorder(QString &) {



  if (_item) {
    _item->zoomXAutoBorder();
  }
  return "Done.";
}


QString PlotSI::setYAutoBorder(QString &) {



  if (_item) {
    _item->zoomYAutoBorder();
  }
  return "Done.";
}


QString PlotSI::setXNoSpike(QString &) {



  if (_item) {
    _item->zoomXNoSpike();
  }
  return "Done.";
}


QString PlotSI::setYNoSpike(QString &) {

  if (_item) {
    _item->zoomYNoSpike();
  }
  return "Done.";
}


QString PlotSI::setXAC(QString &command) {

  if (_item) {
    double R = fabs(getArg(command).toDouble());
    if (R==0) {
      R = 0.2;
    }
    _item->zoomXMeanCentered(R);
  }
  return "Done.";
}


QString PlotSI::setYAC(QString &command) {
  if (_item) {
    double R = fabs(getArg(command).toDouble());
    if (R==0) {
      R = 0.2;
    }
    _item->zoomYMeanCentered(R);
  }
  return "Done.";
}


QString PlotSI::normalizeXtoY(QString &) {
  if (_item) {
    _item->zoomNormalizeXtoY();
  }
  return "Done.";
}


QString PlotSI::setLogX(QString &command) {

  if (_item) {
    QString arg = getArg(command);
    if (arg.toLower()=="true") {
      _item->zoomLogX(false, false, true);
    } else {
      _item->zoomLogX(false, false, false);
    }
  }
  return "Done.";
}


QString PlotSI::setLogY(QString &command) {
  if (_item) {
    QString arg = getArg(command);
    if (arg.toLower()=="true") {
      _item->zoomLogY(false, false, true);
    } else {
      _item->zoomLogY(false, false, false);
    }
  }
  return "Done.";
}


QString PlotSI::setXAxisReversed(QString &) {
  if (_item) {
    _item->xAxis()->setAxisReversed(true);
  }
  return "Done.";
}


QString PlotSI::setYAxisReversed(QString &) {
  if (_item) {
    _item->yAxis()->setAxisReversed(true);
  }
  return "Done.";
}


QString PlotSI::setXAxisNotReversed(QString &) {
  if (_item) {
    _item->xAxis()->setAxisReversed(false);
  }
  return "Done.";
}


QString PlotSI::setYAxisNotReversed(QString &) {
  if (_item) {
    _item->yAxis()->setAxisReversed(false);
  }
  return "Done.";
}


QString PlotSI::setGlobalFont(QString &command) {
  QStringList vars = getArgs(command);

  if (_item) {
    QFont font = _item->globalFont();
    QString family = vars.at(0);
    qreal size = vars.at(1).toDouble();
    bool bold = ((vars.at(2)=="bold") || (vars.at(2)=="true"));
    bool italic = ((vars.at(3)=="italic") || (vars.at(3)=="true"));
    if (!family.isEmpty()) {
      font.setFamily(family);
    }


    font.setItalic(italic);
    font.setBold(bold);

    _item->setGlobalFont(font);
    if (size>1.0) {
      _item->setGlobalFontScale(size);
    }

  }
  return "Done.";
}


QString PlotSI::setTopLabel(QString &command) {

  if (_item) {
    _item->topLabelDetails()->setText(getArg(command));
    _item->topLabelDetails()->setIsAuto(false);
  }

  return "Done.";
}


QString PlotSI::setBottomLabel(QString &command) {

  if (_item) {
    _item->bottomLabelDetails()->setText(getArg(command));
    _item->bottomLabelDetails()->setIsAuto(false);
  }

  return "Done.";
}


QString PlotSI::setLeftLabel(QString &command) {

  if (_item) {
    _item->leftLabelDetails()->setText(getArg(command));
    _item->leftLabelDetails()->setIsAuto(false);
  }

  return "Done.";
}


QString PlotSI::setRightLabel(QString &command) {

  if (_item) {
    _item->rightLabelDetails()->setText(getArg(command));
    _item->rightLabelDetails()->setIsAuto(false);
  }

  return "Done.";
}

QString PlotSI::setTopLabelAuto(QString &) {

  if (_item) {
    _item->topLabelDetails()->setIsAuto(true);
  }

  return "Done.";
}


QString PlotSI::setBottomLabelAuto(QString &) {

  if (_item) {
    _item->bottomLabelDetails()->setIsAuto(true);
  }

  return "Done.";
}


QString PlotSI::setLeftLabelAuto(QString &) {

  if (_item) {
    _item->leftLabelDetails()->setIsAuto(true);
  }

  return "Done.";
}


QString PlotSI::setRightLabelAuto(QString &) {

  if (_item) {
    _item->rightLabelDetails()->setIsAuto(true);
  }

  return "Done.";
}

QString PlotSI::clearXAxisInterpretation(QString &) {

  if (_item) {
    _item->xAxis()->setAxisInterpret(false);
  }

  return "Done.";
}

QString PlotSI::setXAxisInterpretation(QString &command) {

  if (_item) {
    _item->xAxis()->setAxisInterpret(true);
    QString arg = getArg(command);
    if (arg == "ctime") {
      _item->xAxis()->setAxisInterpretation(AXIS_INTERP_CTIME);
    } else if (arg == "year") {
      _item->xAxis()->setAxisInterpretation(AXIS_INTERP_YEAR);
    } else if (arg == "jd") {
      _item->xAxis()->setAxisInterpretation(AXIS_INTERP_JD);
    } else if (arg == "mjd") {
      _item->xAxis()->setAxisInterpretation(AXIS_INTERP_MJD);
    } else if (arg == "rjd") {
      _item->xAxis()->setAxisInterpretation(AXIS_INTERP_RJD);
    } else if (arg == "ait") {
      _item->xAxis()->setAxisInterpretation(AXIS_INTERP_AIT);
    } else if (arg == "excel") {
      _item->xAxis()->setAxisInterpretation(AXIS_INTERP_EXCEL);
    }
  }

  return "Done.";
}

QString PlotSI::setXAxisDisplay(QString &command) {

  if (_item) {
    QString arg = getArg(command);
    if (arg == "year") {
      _item->xAxis()->setAxisDisplay(AXIS_DISPLAY_YEAR);
    } else if (arg == "qttextdatehhmmss") {
      _item->xAxis()->setAxisDisplay(AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS);
    } else if (arg == "qtlocaldatehhmmss") {
      _item->xAxis()->setAxisDisplay(AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS);
    } else if (arg == "jd") {
      _item->xAxis()->setAxisDisplay(AXIS_DISPLAY_JD);
    } else if (arg == "mjd") {
      _item->xAxis()->setAxisDisplay(AXIS_DISPLAY_MJD);
    } else if (arg == "rjd") {
      _item->xAxis()->setAxisDisplay(AXIS_DISPLAY_RJD);
    } else {
      _item->xAxis()->setAxisDisplay(AXIS_DISPLAY_QTDATETIME_FORMAT);
      _item->xAxis()->setAxisDisplayFormatString(arg);
    }
  }

  return "Done.";
}

}
