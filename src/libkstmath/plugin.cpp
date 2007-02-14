/***************************************************************************
                          plugin.cpp  -  Part of KST
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


#include "plugin.h"

#include <klibloader.h>
#include <klocale.h>


const int Plugin::CallError = -424242;

Plugin::Plugin() : KstShared() {
  _lib = 0L;
  _symbol = 0L;
  _freeSymbol = 0L;
  _errorSymbol = 0L;
  _parameterName = 0L;
  //kstdDebug() << "Creating Plugin: " << long(this) << endl;
}


Plugin::~Plugin() {
  _symbol = 0L;
  _freeSymbol = 0L;
  _errorSymbol = 0L;
  _parameterName = 0L;

  if (_lib) {
    _lib->unload();  // this deletes it too
    _lib = 0L;
  }

  //kstdDebug() << "Destroying Plugin: " << long(this) << endl;
}


const QString& Plugin::xmlFile() const {
  return _xmlFile;
}


const QString& Plugin::soFile() const {
  return _soFile;
}


int Plugin::call(const double *const inArrays[], const int inArrayLens[],
                 const double inScalars[], double *outArrays[],
                 int outArrayLens[], double outScalars[], void** local) const {
  if (!_symbol) {
    return CallError;
  }

  return ((int(*)(const double *const[], const int[],
          const double[], double *[], int[],
          double[], void**))_symbol)
    (inArrays, inArrayLens, inScalars, outArrays, outArrayLens, outScalars, local);
}


int Plugin::call(const double *const inArrays[], const int inArrayLens[],
                 const double inScalars[], double *outArrays[],
                 int outArrayLens[], double outScalars[]) const {
  if (!_symbol) {
    return CallError;
  }

  return ((int(*)(const double *const[], const int[],
          const double[], double *[], int[],
          double[]))_symbol)
    (inArrays, inArrayLens, inScalars, outArrays, outArrayLens, outScalars);
}


int Plugin::call(const double *const inArrays[], const int inArrayLens[],
                 const double inScalars[], double *outArrays[],
                 int outArrayLens[], double outScalars[],
                 const char *inStrings[], char *outStrings[], void** local) const {
  if (!_symbol) {
    return CallError;
  }

  return ((int(*)(const double *const[], const int[],
          const double[], double *[], int[],
          double[], const char *[], char *[], void**))_symbol)
    (inArrays, inArrayLens, inScalars, outArrays, outArrayLens, outScalars, inStrings, outStrings, local);
}


int Plugin::call(const double *const inArrays[], const int inArrayLens[],
                 const double inScalars[], double *outArrays[],
                 int outArrayLens[], double outScalars[],
                 const char *inStrings[], char *outStrings[]) const {
  if (!_symbol) {
    return CallError;
  }

  return ((int(*)(const double *const[], const int[],
          const double[], double *[], int[],
          double[], const char *[], char *[]))_symbol)
    (inArrays, inArrayLens, inScalars, outArrays, outArrayLens, outScalars, inStrings, outStrings);
}


QString Plugin::parameterName(int idx) const {
  QString parameter;
  char *name = 0L;

  if (_data._isFit && _parameterName) {
    if (((int(*)(int, char**))_parameterName)(idx, &name) && name) {
      parameter = name; // deep copy into QString
      free(name);
    }
  }

  if (parameter.isEmpty()) {
    parameter = i18n("Param%1").arg(idx);
  }
  
  return parameter;
}


void Plugin::countScalarsVectorsAndStrings(const QValueList<Plugin::Data::IOValue>& table, unsigned& scalars, unsigned& vectors, unsigned& strings, unsigned& numberOfPids) {
  scalars = 0;
  vectors = 0;
  strings = 0;
  numberOfPids = 0;

  for (QValueList<Plugin::Data::IOValue>::ConstIterator it = table.begin(); it != table.end(); ++it) {
    switch ((*it)._type) {
      case Plugin::Data::IOValue::StringType:
        ++strings;
        break;
      case Plugin::Data::IOValue::PidType:
        ++numberOfPids;
      case Plugin::Data::IOValue::FloatType:
        ++scalars;
        break;
      case Plugin::Data::IOValue::TableType:
        if ((*it)._subType == Plugin::Data::IOValue::FloatSubType ||
            (*it)._subType == Plugin::Data::IOValue::FloatNonVectorSubType) {
          ++vectors;
        }
        break;
      default:
        break;
    }
  }
}


const Plugin::Data& Plugin::data() const {
  return _data;
}


void Plugin::Data::clear() {
  _isFilter = false;
  _localdata = false;
  _name = QString::null;
  _readableName = QString::null;
  _author = QString::null;
  _description = QString::null;
  _version = QString::null;
  _filterInputVector = QString::null;
  _filterOutputVector = QString::null;
  _state = Unknown;
  _isFit = false;
  _isFitWeighted = false;

  _inputs.clear();
  _outputs.clear();
  _parameters.clear();
  _hints.clear();
}


bool Plugin::freeLocalData(void **local) const {
  if (_freeSymbol) {
    ((void (*)(void**))_freeSymbol)(local);
    return true;
  }
  return false;
}


const char *Plugin::errorCode(int code) const {
  if (_errorSymbol) {
    return ((const char *(*)(int))_errorSymbol)(code);
  }
  return 0L;
}

// vim: ts=2 sw=2 et
