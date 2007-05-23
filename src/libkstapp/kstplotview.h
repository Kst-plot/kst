#ifndef KSTPLOTVIEW_H
#define KSTPLOTVIEW_H

#include <QGraphicsView>

#include "kst_export.h"

class KST_EXPORT KstPlotView : public QGraphicsView
{
  Q_OBJECT
public:
  KstPlotView();
  virtual ~KstPlotView();

private:
};

#endif

// vim: ts=2 sw=2 et
