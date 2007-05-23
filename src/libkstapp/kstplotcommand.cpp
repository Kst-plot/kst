#include "kstplotcommand.h"
#include "kstplotview.h"

KstPlotCommand::KstPlotCommand(KstPlotView *view, const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent), _view(view) {
}


KstPlotCommand::~KstPlotCommand() {
}

// vim: ts=2 sw=2 et
