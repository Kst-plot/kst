/***************************************************************************
                           plugin.h  -  Part of KST
                             -------------------
    begin                : Tue May 06 2003
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

#ifndef _KST_PLUGIN_H
#define _KST_PLUGIN_H

#include <qmap.h>
//Added by qt3to4:
#include <Q3ValueList>

#include "kstsharedptr.h"
#include "kst_export.h"

class KLibrary;

class KST_EXPORT Plugin : public KstShared {
  friend class PluginLoader;
  public:
    virtual ~Plugin();

    class Data;

    const Data& data() const;

    const QString& xmlFile() const;
    const QString& soFile() const;

    int call(const double *const inArrays[], const int inArrayLens[],
        const double inScalars[],
        double *outArrays[], int outArrayLens[],
        double outScalars[]) const;
    int call(const double *const inArrays[], const int inArrayLens[],
        const double inScalars[],
        double *outArrays[], int outArrayLens[],
        double outScalars[],
        const char *inStrings[], char *outStrings[]) const;
    int call(const double *const inArrays[], const int inArrayLens[],
        const double inScalars[],
        double *outArrays[], int outArrayLens[],
        double outScalars[], void **local) const;
    int call(const double *const inArrays[], const int inArrayLens[],
        const double inScalars[],
        double *outArrays[], int outArrayLens[],
        double outScalars[],
        const char *inStrings[], char *outStrings[],
        void **local) const;
    QString parameterName(int index) const;
    bool freeLocalData(void **local) const;
    const char *errorCode(int code) const;

    static const int CallError;

    class Data {
      public:
        Data() : _isFilter(false), _localdata(false), _isFit(false), _isFitWeighted(false) {}

        /**
         *   Clear all values.
         */
        void clear();

        /**
         *   Types use by this class.
         */
        enum PluginState { Unknown, PreAlpha, Alpha, Beta, Release };
        enum ParameterTypes { String, Integer };

        class IOValue {
          public:
            enum ValueType { UnknownType,
              TableType,
              StringType,
              MapType,
              IntegerType,
              FloatType,
              PidType,
              MatrixType
            };
            enum ValueSubType { UnknownSubType,
              AnySubType,
              FloatSubType,
              StringSubType,
              IntegerSubType,
              FloatNonVectorSubType
            };

            QString _name;
            ValueType _type;
            ValueSubType _subType;
            QString _description;
            QString _default;
        };

        // Intro
        bool _isFilter : 1;
        bool _localdata : 1;
        bool _isFit : 1;
        bool _isFitWeighted : 1;
        QString _name;
        QString _readableName;
        QString _author;
        QString _description;
        QString _version;
        QString _filterInputVector;
        QString _filterOutputVector;
        PluginState _state;
        // Unimplemented:  platforms, language, documentation

        // Interface
        Q3ValueList<IOValue> _inputs;
        Q3ValueList<IOValue> _outputs;

        // Parameter list
        QMap<QString, QPair<ParameterTypes, QString> > _parameters;

        // Curve hints
        struct CurveHint {
          CurveHint() {}
          CurveHint(const QString& name, const QString& x, const QString& y)
            : name(name), x(x), y(y) {}
          QString name, x, y;
        };
        Q3ValueList<Data::CurveHint> _hints;
    };


    static void countScalarsVectorsAndStrings(const Q3ValueList<Plugin::Data::IOValue>& table, int& scalars, int& vectors, int& strings, int& numberOfPids);

  protected:
    Plugin();

    Data _data;

    KLibrary *_lib;

    void *_symbol, *_freeSymbol, *_errorSymbol;
    void *_parameterName;

    // Related files
    QString _xmlFile, _soFile;
};


#endif

// vim: ts=2 sw=2 et
