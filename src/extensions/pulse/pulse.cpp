/***************************************************************************
                                  pulse.cpp
                             -------------------
    begin                : Sep 25 2005
    copyright            : (C) 2005 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "pulse.h"
#include "kstgvector.h"

#include <kst.h>
#include <kstdatacollection.h>

#include <kaction.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <kparts/part.h>


/* TODO
- have sensible defaults when the dialog is opened
- make vectors show up in the data manager
- make vectors more easily editable (from data manager?)
- more configurable?  perhaps stepsize and starting value?  or just equations?
- random noise generator?  Or just use equations for that?
- memory guard
*/
K_EXPORT_COMPONENT_FACTORY(kstextension_pulse, KGenericFactory<KstPulse>)

KstPulse::KstPulse(QObject *parent, const char *name, const QStringList& l) : KstExtension(parent, name, l), KXMLGUIClient() {
  new KAction(i18n("Pulse Generator"), 0, 0, this, SLOT(doShow()), actionCollection(), "pulse_show");
  setInstance(app()->instance());
  setXMLFile("kstextension_pulse.rc", true);
  app()->guiFactory()->addClient(this);
}


KstPulse::~KstPulse() {
  delete _generator;
  KstGVectorList gvl = kstObjectSubList<KstVector, KstGVector>(KST::vectorList);
  for (KstGVectorList::Iterator i = gvl.begin(); i != gvl.end(); ++i) {
    (*i)->setFrequency(0);
  }
  KstApp *app = dynamic_cast<KstApp*>(this->app());
  if (app && app->guiFactory()) {
    app->guiFactory()->removeClient(this);
  }
}


void KstPulse::doShow() {
  if (!_generator) {
    _generator = new KstPulseGenerator(app(), "Kst Pulse Generator");
  }
  _generator->show();
}


void KstPulse::load(QDomElement& e) {
  Q_UNUSED(e)
}


void KstPulse::save(QTextStream& ts, const QString& indent) {
  Q_UNUSED(ts)
  Q_UNUSED(indent)
}


#include "pulse.moc"
// vim: ts=2 sw=2 et
