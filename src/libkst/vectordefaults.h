/***************************************************************************
                             kstvectordefaults.h
                             -------------------
    begin                : May 28, 2004
    copyright            : (C) 2004 The University of Toronto
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

#ifndef KSTVECTORDEFAULTS_H
#define KSTVECTORDEFAULTS_H

#include <qstring.h>
#include "kst_export.h"

class QSettings;

namespace Kst {
class ObjectStore;

class KST_EXPORT VectorDefaults {
  public:
    VectorDefaults();
    void sync();
    const QString& dataSource() const;
    const QString& wizardXVector() const;
    void setWizardXVector(const QString& vector);
    double f0() const;
    double n() const;
    bool countFromEOF() const;
    bool readToEOF() const;
    bool doSkip() const;
    bool doAve() const;
    int skip() const;

    void readConfig(QSettings *config);
    void writeConfig(QSettings *config);

  private:
    QString _dataSource, _wizardX;
    double _f0;
    double _n;
    bool _doSkip;
    bool _doAve;
    int _skip;
    ObjectStore *_store; // FIXME: this needs to be initialized
};

extern KST_EXPORT VectorDefaults vectorDefaults;
}

#endif
// vim: ts=2 sw=2 et
