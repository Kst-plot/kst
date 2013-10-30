/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2013 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** this class will provide a signal when an update to object lists in
 * dialogs is needed.  For instance, when a new vector is added, or its
 * name has been changed. */

#ifndef UPDATESERVER_H
#define UPDATESERVER_H

#include "kst_export.h"

#include <QObject>

namespace Kst {

/** A signal is requested by a call to requestUpdateSignal.  The signal
 * will be emitted when the _signalSuspendDepthCounter is zero.  If multiple
 * signals are requested while _signalSuspendDepthCounter is non zero, only one
 * will be emitted.  beginSuspendSignals() increments _signalSuspendDepthCounter.
 * endSuspendSignals() decrements it. */
class KSTCORE_EXPORT UpdateServer : public QObject
{
    Q_OBJECT
  public:
    static UpdateServer* self();
    void beginSuspendSignals() {_signalSuspendDepthCounter++;}
    void endSuspendSignals();
    void requestUpdateSignal();

  private:
    UpdateServer();
    ~UpdateServer();

    static UpdateServer* _self;
    static void cleanup();

    int _signalSuspendDepthCounter;
    bool _signalRequested;

  Q_SIGNALS:
    void objectListsChanged();

  public slots:


};

}
#endif // UPDATESERVER_H
