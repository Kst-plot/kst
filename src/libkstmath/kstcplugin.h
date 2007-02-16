/***************************************************************************
                                 kstplugin.h
                             -------------------
    begin                : May 15 2003
    copyright            : (C) 2003 The University of Toronto
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

#ifndef KSTPLUGIN_H
#define KSTPLUGIN_H

#include "kstdataobject.h"
#include "plugin.h"
#include "kst_export.h"
//Added by qt3to4:
#include <Q3ValueList>


/*  Usage notes:
 *   - Output vectors are created internally, but may be renamed.
 *   - Input vectors must be set before setPlugin(), else it will fail.
 */

class KST_EXPORT KstCPlugin : public KstDataObject {
  public:
    KstCPlugin();
    KstCPlugin(const QDomElement &e);
    virtual ~KstCPlugin();

    virtual UpdateType update(int update_counter);

    virtual void save(Q3TextStream &ts, const QString& indent = QString::null);

    virtual bool slaveVectorsUsed() const;
    virtual bool isValid() const;

    virtual bool setPlugin(KstSharedPtr<Plugin> plugin);
    KstSharedPtr<Plugin> plugin() const;

    virtual QString propertyString() const;

    virtual const KstCurveHintList* curveHints() const;

    QString label(int precision) const;
    QString lastError() const;

    virtual Kind kind() const;

    // FIXME: remove this
    void createFitScalars();

    virtual KstDataObjectPtr makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap);

  protected:
    static void countScalarsAndVectors(const Q3ValueList<Plugin::Data::IOValue>& table, unsigned& scalars, unsigned& vectors);
    virtual void showNewDialog();
    virtual void showEditDialog();
    KstSharedPtr<Plugin> _plugin;
    unsigned _inScalarCnt, _inArrayCnt, _inStringCnt, _outScalarCnt;
    unsigned _inPid, _outArrayCnt, _outStringCnt;
    void *_localData;
    QString _lastError;

    int *_inArrayLens, *_outArrayLens;
    double *_inScalars, *_outScalars;
    double **_inVectors, **_outVectors;
    char **_inStrings;
    char **_outStrings;

  private:
    void commonConstructor();
    void allocateParameters();
    void freeParameters();
};

typedef KstSharedPtr<KstCPlugin> KstCPluginPtr;
typedef KstObjectList<KstCPluginPtr> KstCPluginList;

#endif

// vim: ts=2 sw=2 et
