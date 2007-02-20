/***************************************************************************
                      pluginxmlparser.cpp  -  Part of KST
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

#include "pluginxmlparser.h"

#include <kglobal.h>
#include <klocale.h>
#include <qfile.h>
//Added by qt3to4:
#include <Q3ValueList>

PluginXMLParser::PluginXMLParser() {
}


PluginXMLParser::~PluginXMLParser() {
}


int PluginXMLParser::parseFile(const QString& filename) {
  QFile qf(filename);
  int retVal = -1;

  if (qf.open(QIODevice::ReadOnly)) {
    QDomDocument doc(filename);

    if (doc.setContent(&qf)) {
      _pluginData.clear();
      retVal = parseDOM(doc);
    } else {
      retVal = -2;
    }
    qf.close();
  }

  return retVal;
}


const Plugin::Data& PluginXMLParser::data() const {
  return _pluginData;
}


static const QString& QS_string = KGlobal::staticQString("string");
static const QString& QS_int = KGlobal::staticQString("int");
static const QString& QS_name = KGlobal::staticQString("name");
static const QString& QS_readableName = KGlobal::staticQString("readableName");
static const QString& QS_helptext = KGlobal::staticQString("helptext");
static const QString& QS_intro = KGlobal::staticQString("intro");
static const QString& QS_interface = KGlobal::staticQString("interface");
static const QString& QS_paralist = KGlobal::staticQString("paralist");
static const QString& QS_modulename = KGlobal::staticQString("modulename");
static const QString& QS_author = KGlobal::staticQString("author");
static const QString& QS_fit = KGlobal::staticQString("fit");
static const QString& QS_weighted = KGlobal::staticQString("weighted");
static const QString& QS_description = KGlobal::staticQString("description");
static const QString& QS_descr = KGlobal::staticQString("descr");
static const QString& QS_default = KGlobal::staticQString("default");
static const QString& QS_version = KGlobal::staticQString("version");
static const QString& QS_state = KGlobal::staticQString("state");
static const QString& QS_platforms = KGlobal::staticQString("platforms");
static const QString& QS_language = KGlobal::staticQString("language");
static const QString& QS_documentation = KGlobal::staticQString("documentation");
static const QString& QS_text = KGlobal::staticQString("text");
static const QString& QS_major = KGlobal::staticQString("major");
static const QString& QS_minor = KGlobal::staticQString("minor");
static const QString& QS_devstate = KGlobal::staticQString("devstate");
static const QString& QS_prealpha = KGlobal::staticQString("pre-alpha");
static const QString& QS_alpha = KGlobal::staticQString("alpha");
static const QString& QS_beta = KGlobal::staticQString("beta");
static const QString& QS_release = KGlobal::staticQString("release");
static const QString& QS_input = KGlobal::staticQString("input");
static const QString& QS_output = KGlobal::staticQString("output");
static const QString& QS_table = KGlobal::staticQString("table");
static const QString& QS_matrix = KGlobal::staticQString("matrix");
static const QString& QS_map = KGlobal::staticQString("map");
static const QString& QS_type = KGlobal::staticQString("type");
static const QString& QS_float = KGlobal::staticQString("float");
static const QString& QS_floatNonVector = KGlobal::staticQString("floatnonvector");
static const QString& QS_any = KGlobal::staticQString("any");
static const QString& QS_integer = KGlobal::staticQString("integer");
static const QString& QS_filter = KGlobal::staticQString("filter");
static const QString& QS_filter_input = KGlobal::staticQString("input");
static const QString& QS_filter_output = KGlobal::staticQString("output");
static const QString& QS_localdata = KGlobal::staticQString("localdata");
static const QString& QS_pid = KGlobal::staticQString("pid");
static const QString& QS_curvehints = KGlobal::staticQString("curvehints");
static const QString& QS_hint = KGlobal::staticQString("hint");

int PluginXMLParser::parseDOM(const QDomDocument& doc) {
QDomElement topElem = doc.documentElement();

  if (topElem.tagName().toLower() == QString::fromLatin1("module")) {
    QDomNode n = topElem.firstChild();

    while (!n.isNull()) {
      QDomElement e = n.toElement();
      QString tn = e.tagName().toLower();
      int rc = 0;

      if (tn == QS_interface) {
        rc = parseInterface(e);
      } else if (tn == QS_intro) {
        rc = parseIntro(e);
#if 0
      } else if (tn == QS_paralist) {
        rc = parseParalist(e);
#endif
      } else if (tn == QS_curvehints) {
        rc = parseCurveHints(e);
      } else {
        // Unknown tag
      }

      if (rc < 0) {  // error occurred
        return rc;
      }      
      n = n.nextSibling();
    }

    // check that we have a valid filter, and if not then disable it.
    if (_pluginData._isFilter) {
      _pluginData._isFilter = false;

      // if we have no input vector name check to see if we can declare one by default.
      if (_pluginData._filterInputVector.isEmpty()) {
        int num = 0;
        int inputIndex = 0;

        for (uint i = 0; i < _pluginData._inputs.size(); i++) {
          if (_pluginData._inputs[i]._type == Plugin::Data::IOValue::TableType) {
            num++;
            inputIndex = i;
          }
        }
        if (num == 1) {
          _pluginData._filterInputVector = _pluginData._inputs[inputIndex]._name;
        }
      }

      // if we have no output vector name check to see if we can declare one by default.
      if (_pluginData._filterOutputVector.isEmpty()) {
        int num = 0;
        int inputIndex = 0;

        for (uint i = 0; i < _pluginData._outputs.size(); i++) {
          if (_pluginData._outputs[i]._type == Plugin::Data::IOValue::TableType) {
            num++;
            inputIndex = i;
          }
        }
        if (num == 1) {
          _pluginData._filterOutputVector = _pluginData._outputs[inputIndex]._name;
        }
      }

      // check that we have valid vector names for the input and output.
      for (uint i = 0; i < _pluginData._inputs.size(); i++) {
        if (_pluginData._inputs[i]._type == Plugin::Data::IOValue::TableType &&
            _pluginData._inputs[i]._name == _pluginData._filterInputVector) {
          for (uint j = 0; j < _pluginData._outputs.size(); j++) {
            if (_pluginData._outputs[j]._type == Plugin::Data::IOValue::TableType &&
                _pluginData._outputs[j]._name == _pluginData._filterOutputVector) {
              _pluginData._isFilter = true;
              break;
            }
          }
        }
      }
    }
  } else {
    return -3;  // XML parse error - no "module" at the top
  }

return 0;
}


int PluginXMLParser::parseIntro(const QDomElement& element) {
QDomNode n = element.firstChild();

  while (!n.isNull()) {
    int rc = 0;
    QDomElement e = n.toElement();

    if (e.isNull()) {
      n = n.nextSibling();
      continue;
    }

    QString tn = e.tagName().toLower();
    if (tn == QS_modulename) {
      _pluginData._readableName = e.attribute(QS_readableName);
      _pluginData._name = e.attribute(QS_name);
    } else if (tn == QS_localdata) {
      _pluginData._localdata = true;
    } else if (tn == QS_filter) {
      _pluginData._isFilter = true;
      _pluginData._filterInputVector = e.attribute(QS_filter_input);
      _pluginData._filterOutputVector = e.attribute(QS_filter_output);
      QString n = i18n("Filter Output");
      QString x = _pluginData._filterInputVector;
      QString y = _pluginData._filterOutputVector;
      if (!y.isEmpty() && !x.isEmpty()) {
        _pluginData._hints.append(Plugin::Data::CurveHint(n, x, y));
      }
    } else if (tn == QS_author) {
      _pluginData._author = e.attribute(QS_name);
    } else if (tn == QS_description) {
      _pluginData._description = e.attribute(QS_text);
    } else if (tn == QS_fit) {
      _pluginData._isFit = true;
      _pluginData._isFitWeighted = (bool)e.attribute(QS_weighted).toInt();
    } else if (tn == QS_version) {
      _pluginData._version = QString("%1.%2").arg(e.attribute(QS_major))
                                             .arg(e.attribute(QS_minor));
    } else if (tn == QS_state) {
      QString st = e.attribute(QS_devstate).toLower();
      _pluginData._state = Plugin::Data::Unknown;

      if (st == QS_prealpha) {
        _pluginData._state = Plugin::Data::PreAlpha;
      } else if (st == QS_alpha) {
        _pluginData._state = Plugin::Data::Alpha;
      } else if (st == QS_beta) {
        _pluginData._state = Plugin::Data::Beta;
      } else if (st == QS_release) {
        _pluginData._state = Plugin::Data::Release;
      }
    } else if (tn == QS_platforms) {
      // Unimplemented
    } else if (tn == QS_language) {
      // Unimplemented
    } else if (tn == QS_documentation) {
      // Unimplemented
    } else {
      // Unknown node
    }

    if (rc < 0) {
      return rc;
    }

    n = n.nextSibling();
  }

  if (_pluginData._readableName.isEmpty()) {
    _pluginData._readableName = _pluginData._name;
  }

  return 0;
}


int PluginXMLParser::parseInterface(const QDomElement& element) {
QDomNode n = element.firstChild();

  while (!n.isNull()) {
    int rc = 0;
    QDomElement e = n.toElement();

    if (e.isNull()) {
      n = n.nextSibling();
      continue;
    }

    QString tn = e.tagName().toLower();
    if (tn == QS_input) {
      rc = parseIO(e, _pluginData._inputs);
    } else if (tn == QS_output) {
      rc = parseIO(e, _pluginData._outputs);
    } else {
      // Unknown node
    }

    if (rc < 0) {
      return rc;
    }

    n = n.nextSibling();
  }

return 0;
}


int PluginXMLParser::parseIO(const QDomElement& element, Q3ValueList<Plugin::Data::IOValue>& collection) {
QDomNode n = element.firstChild();

  while (!n.isNull()) {
    QDomElement e = n.toElement();

    if (e.isNull()) {
      n = n.nextSibling();
      continue;
    }

    Plugin::Data::IOValue iov;

    QString tn = e.tagName().toLower();
    if (tn == QS_table) {
      iov._type = Plugin::Data::IOValue::TableType;
//    } else if (tn == QS_integer) {
//      iov._type = Plugin::Data::IOValue::IntegerType;
    } else if (tn == QS_matrix) {
      iov._type = Plugin::Data::IOValue::MatrixType;
    } else if (tn == QS_float) {
      iov._type = Plugin::Data::IOValue::FloatType;
    } else if (tn == QS_pid) {
      iov._type = Plugin::Data::IOValue::PidType;
    } else if (tn == QS_string) {
      iov._type = Plugin::Data::IOValue::StringType;
//    } else if (tn == QS_map) {
//      iov._type = Plugin::Data::IOValue::MapType;
    } else {
      // Unknown node
      iov._type = Plugin::Data::IOValue::UnknownType;
      return -1;
    }

    if (iov._type != Plugin::Data::IOValue::UnknownType) {
      iov._name = e.attribute(QS_name);
      iov._description = e.attribute(QS_descr);
      iov._default = e.attribute(QS_default);
      QString subtype = e.attribute(QS_type).toLower();
      if (subtype == QS_float) {
        iov._subType = Plugin::Data::IOValue::FloatSubType;
      } else if (subtype == QS_floatNonVector) {
        iov._subType = Plugin::Data::IOValue::FloatNonVectorSubType;
//      } else if (subtype == QS_any) {
//        iov._subType = Plugin::Data::IOValue::AnySubType;
      } else if (subtype == QS_string) {
        iov._subType = Plugin::Data::IOValue::StringSubType;
        iov._description = e.attribute(QS_helptext);
//      } else if (subtype == QS_integer) {
//        iov._subType = Plugin::Data::IOValue::IntegerSubType;
      } else {
        iov._subType = Plugin::Data::IOValue::UnknownSubType;
      }

      if (iov._type != Plugin::Data::IOValue::TableType ||
          iov._subType != Plugin::Data::IOValue::UnknownSubType) {
        collection.append(iov);
      } else {
        return -1;
      }
    }

    n = n.nextSibling();
  }

return 0;
}


int PluginXMLParser::parseCurveHints(const QDomElement& element) {
  QDomNode n = element.firstChild();

  while (!n.isNull()) {
    int rc = 0;
    QDomElement e = n.toElement();

    if (e.isNull()) {
      n = n.nextSibling();
      continue;
    }

    QString tn = e.tagName().toLower();
    if (tn == QS_hint) {
      QString n = e.attribute(QS_name);
      QString x = e.attribute("x");
      QString y = e.attribute("y");
      if (!n.isEmpty() && !y.isEmpty() && !x.isEmpty()) {
        _pluginData._hints.append(Plugin::Data::CurveHint(n, x, y));
      }
    } else {
      // Unknown node
    }

    if (rc < 0) {
      return rc;
    }

    n = n.nextSibling();
  }

  return 0;
}


#if 0
int PluginXMLParser::parseParalist(const QDomElement& element) {
  QDomNode n = element.firstChild();

  while (!n.isNull()) {
    int rc = 0;
    QDomElement e = n.toElement();

    if (e.isNull()) {
      n = n.nextSibling();
      continue;
    }

    QString tn = e.tagName().toLower();
    if (tn == QS_string) {
      _pluginData._parameters[e.attribute(QS_name)] = qMakePair(Plugin::Data::String, e.attribute(QS_helptext));
    } else if (tn == QS_int) {
      _pluginData._parameters[e.attribute(QS_name)] = qMakePair(Plugin::Data::Integer, e.attribute(QS_helptext));
    } else {
      // Unknown node
    }

    if (rc < 0) {
      return rc;
    }

    n = n.nextSibling();
  }

  return 0;
}
#endif


// vim: ts=2 sw=2 et
