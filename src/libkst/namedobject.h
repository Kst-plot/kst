/***************************************************************************
                 namedobject.h: adds naming features to objects...
                             -------------------
    begin                : May 29, 2008
    copyright            : (C) 2008 C. Barth Netterfield
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
// name system: see object names devel doc for intended behavior


#ifndef NAMEDOBJECT_H
#define NAMEDOBJECT_H

#include <QString>
#include <QXmlStreamWriter>

namespace Kst {

// short name index variables
extern int _vnum; // vectors
extern int _pnum; // plugins
extern int _csdnum; // csd
extern int _cnum; // curves
extern int _enum; // equations
extern int _hnum; // histograms
extern int _inum; // images
extern int _psdnum; // psd
extern int _xnum; // scalars
extern int _tnum; // text string
extern int _mnum; // matrix
extern int _plotnum; // plot item
extern int _lnum; // legend
extern int _dnum; // view item (drawable)

extern int max_vnum; // vectors
extern int max_pnum; // plugins
extern int max_csdnum; // csd
extern int max_cnum; // curves
extern int max_enum; // equations
extern int max_hnum; // histograms
extern int max_inum; // images
extern int max_psdnum; // psd
extern int max_xnum; // scalars
extern int max_tnum; // string
extern int max_mnum; // matrix
extern int max_plotnum; // plot item
extern int max_lnum; // legend
extern int max_dnum; // view item

class NamedObject {
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
      DNUM   = 0x2000
    };

    // name system: see object names devel doc
    QString Name() const; // eg GYRO1:V1
    QString descriptiveName() const; // eg GYRO1: automatic or manual
    QString shortName() const; // eg V1: always automatically generated
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
};

}
#endif
