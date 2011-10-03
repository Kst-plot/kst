#include "plotscriptinterface.h"
#include "plotitem.h"

namespace Kst {

PlotSI::PlotSI(PlotItem *it) : _layout(new LayoutTabSI), _dim(new DimensionTabSI), _fill(new FillTabSI), _stroke(new StrokeTabSI) {
  _layout->vi=it;
  _dim->item=it;
  _fill->item=it;
  _stroke->item=it;

}

QByteArrayList PlotSI::commands() {
    return _layout->commands()<<_dim->commands()<<_stroke->commands()<<_fill->commands();
}

QString PlotSI::doCommand(QString x) {
  qDebug() << "Plot SI doing command " << x;
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

}
