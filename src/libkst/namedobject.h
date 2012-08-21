/***************************************************************************
                 namedobject.h: adds naming features to objects...
                             -------------------
    begin                : May 29, 2008
    copyright            : (C) 2008 C. Barth Netterfield
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// name system: see object names devel doc for intended behavior


#ifndef NAMEDOBJECT_H
#define NAMEDOBJECT_H

#include <QString>
#include <QXmlStreamWriter>
#include <QFont>
#include "kst_export.h"

namespace Kst {

// short name index variables
KSTCORE_EXPORT extern int _vnum; // vectors
KSTCORE_EXPORT extern int _pnum; // plugins
KSTCORE_EXPORT extern int _csdnum; // csd
KSTCORE_EXPORT extern int _cnum; // curves
KSTCORE_EXPORT extern int _enum; // equations
KSTCORE_EXPORT extern int _hnum; // histograms
KSTCORE_EXPORT extern int _inum; // images
KSTCORE_EXPORT extern int _psdnum; // psd
KSTCORE_EXPORT extern int _xnum; // scalars
KSTCORE_EXPORT extern int _tnum; // text string
KSTCORE_EXPORT extern int _mnum; // matrix
KSTCORE_EXPORT extern int _plotnum; // plot item
KSTCORE_EXPORT extern int _lnum; // legend
KSTCORE_EXPORT extern int _dnum; // view item (drawable)
KSTCORE_EXPORT extern int _dsnum; // datasource

KSTCORE_EXPORT extern int max_vnum; // vectors
KSTCORE_EXPORT extern int max_pnum; // plugins
KSTCORE_EXPORT extern int max_csdnum; // csd
KSTCORE_EXPORT extern int max_cnum; // curves
KSTCORE_EXPORT extern int max_enum; // equations
KSTCORE_EXPORT extern int max_hnum; // histograms
KSTCORE_EXPORT extern int max_inum; // images
KSTCORE_EXPORT extern int max_psdnum; // psd
KSTCORE_EXPORT extern int max_xnum; // scalars
KSTCORE_EXPORT extern int max_tnum; // string
KSTCORE_EXPORT extern int max_mnum; // matrix
KSTCORE_EXPORT extern int max_plotnum; // plot item
KSTCORE_EXPORT extern int max_lnum; // legend
KSTCORE_EXPORT extern int max_dnum; // view item
KSTCORE_EXPORT extern int max_dsnum; // datasource

class KSTCORE_EXPORT NamedObject 
{
public: 
     NamedObject();
     virtual ~NamedObject();

     enum ShortNameIndex {
      VNUM   = 0x0001,
      PNUM   = 0x0002,
      CSDNUM = 0x0004,
      CNUM   = 0x0008,
      ENUM   = 0x0010,
      HNUM   = 0x0020,
      INUM   = 0x0040,
      PSDNUM = 0x0080,
      XNUM   = 0x0100,
      TNUM   = 0x0200,
      MNUM   = 0x0400,
      PLOTNUM= 0x0800,
      LNUM   = 0x1000,
      DNUM   = 0x2000,
      DSNUM  = 0x4000
    };

    // name system: see object names devel doc
    QString Name() const; // eg GYRO1 (V1)
    QString CleanedName() const; // all \_ replaced with _
    QString descriptiveName() const; // eg GYRO1: automatic or manual
    QString shortName() const; // eg V1: always automatically generated
    QString lengthLimitedName(int length = 20) const; // Name, but with descriptiveName truncated
    QString sizeLimitedName(const QFont&font,const int&width) const; // Name, shrunk to fit in width with font
    QString sizeLimitedName(const QWidget *widget) const; // Name, shrunk to fit in widget
    virtual QString descriptionTip() const = 0; // description for tooltips
    void setDescriptiveName(QString new_name); // auto if new_name.isEmpty()
    bool descriptiveNameIsManual() const;
    static void processShortNameIndexAttributes(QXmlStreamAttributes &attrs);

    // Reset all name indexes.  Should only be used by ObjectStore when clearing the store entirely.
    static void resetNameIndex();

  protected:
    virtual QString _automaticDescriptiveName() const= 0;
    virtual void _initializeShortName() = 0;
    QString _manualDescriptiveName;
    QString _shortName;
    virtual void saveNameInfo(QXmlStreamWriter &s, unsigned I = 0xffff);

    // object indices used for saving/resorting shortnames
    int _initial_vnum; // vectors
    int _initial_pnum; // plugins
    int _initial_csdnum; // csd
    int _initial_cnum; // curves
    int _initial_enum; // equations
    int _initial_hnum; // histograms
    int _initial_inum; // images
    int _initial_psdnum; // psd
    int _initial_xnum; // scalars
    int _initial_tnum; // text string
    int _initial_mnum; // matrix

    int _initial_plotnum; // plot item
    int _initial_lnum; // legend
    int _initial_dnum; // view item
    int _initial_dsnum; // datasource
};

KSTCORE_EXPORT bool shortNameLessThan(NamedObject *n1, NamedObject *n2);

}
#endif
