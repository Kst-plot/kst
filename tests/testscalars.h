/*
 *  Copyright 2004, The University of Toronto
 *  Licensed under GPL.
 */
#ifndef SLISTENER_H
#define SLISTENER_H
#include <qobject.h>

class SListener : public QObject {
  Q_OBJECT
  public:
    SListener();
    virtual ~SListener();
    int _trigger;
  public slots:
    void trigger();
};

#endif
// vim: ts=2 sw=2 et
