#ifndef KSTPLOTCOMMAND_H
#define KSTPLOTCOMMAND_H

#include <QPointer>
#include <QUndoCommand>

#include "kst_export.h"

class KstPlotView;

class KST_EXPORT KstPlotCommand : public QUndoCommand
{
public:
  KstPlotCommand(KstPlotView *view, const QString &text, QUndoCommand *parent = 0);
  virtual ~KstPlotCommand();

protected:
  QPointer<KstPlotView> _view;
};

#endif

// vim: ts=2 sw=2 et
