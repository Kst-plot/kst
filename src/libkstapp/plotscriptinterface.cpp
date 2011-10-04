#include "plotscriptinterface.h"
#include "plotitem.h"

namespace Kst {

PlotSI::PlotSI(PlotItem *it) : _layout(new LayoutTabSI), _dim(new DimensionTabSI), _fill(new FillTabSI), _stroke(new StrokeTabSI) {
  _layout->vi=it;
  _dim->item=it;
  _fill->item=it;
  _stroke->item=it;
  _item = it;

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

}

QByteArrayList PlotSI::commands() {
    return _layout->commands()<<_dim->commands()<<_stroke->commands()<<_fill->commands();
}

QString PlotSI::doCommand(QString x) {
  QString command = x.left(x.indexOf('('));

  InterfaceMemberFn fn=_fnMap.value(command,&PlotSI::noSuchFn);

  if(fn!=&PlotSI::noSuchFn) {
    return CALL_MEMBER_FN(*this,fn)(x);
  }

  QString v=_layout->doCommand(x);
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

QByteArray PlotSI::getHandle() {
    return ((QString)("Finished editing "%_dim->item->Name())).toAscii();
}

QStringList PlotSI::getArgs(const QString &command) {
  int i0 = command.indexOf('(')+1;
  int i1 = command.lastIndexOf(')');
  int n = i1-i0;

  QString x = command.mid(i0,n);
  return x.split(',');

}

QString PlotSI::getArg(const QString &command) {
  int i0 = command.indexOf('(')+1;
  int i1 = command.lastIndexOf(')');
  int n = i1-i0;

  QString x = command.mid(i0,n);
  return x;

}

/***************************/
/* commands                */
/***************************/

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


QString PlotSI::setGlobalFont(QString &command) {
  QStringList vars = getArgs(command);

  if (_item) {
    QFont font = _item->globalFont();
    QString family = vars.at(0);
    bool bold = ((vars.at(1)=="bold") || (vars.at(1)=="true"));
    bool italic = ((vars.at(2)=="italic") || (vars.at(2)=="true"));
    if (!family.isEmpty()) {
      font.setFamily(family);
    }
    font.setItalic(italic);
    font.setBold(bold);

    _item->setGlobalFont(font);
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

}
