/***************************************************************************
                                    js.h
                             -------------------
    begin                : Feb 09 2004
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

#ifndef JS_H
#define JS_H

#include <qguardedptr.h>
#include <qsplitter.h>

#include <kstextension.h>
#include <kxmlguiclient.h>

class JSIfaceImpl;

namespace KParts {
  class Part;
}

class KToggleAction;

class KstJS : public KstExtension, public KXMLGUIClient {
  Q_OBJECT
  public:
    KstJS(QObject *parent, const char *name, const QStringList&);
    virtual ~KstJS();

    // To save state
    virtual void load(QDomElement& e);
    virtual void save(QTextStream& ts, const QString& indent = QString::null);
    virtual void processArguments(const QString& args);

    KJSEmbed::KJSEmbedPart *part() const { return _jsPart; }

  public slots:
    void doShow(bool);
    void loadScript();
    void createRegistry();
    void destroyRegistry();
    void showConsole();
    void hideConsole();
    void resetInterpreter();

  private slots:
    void shellExited();
    void restoreUI();
    void doArgs();

  private:
    void createBindings();

  protected:
    friend class LoadScript;
    QStringList _scripts;
    QGuardedPtr<KJSEmbed::KJSEmbedPart> _jsPart;

  private:
    JSIfaceImpl *_iface;
    QGuardedPtr<KParts::Part> _konsolePart;
    QGuardedPtr<QSplitter> _splitter;
    QGuardedPtr<QWidget> _oldCentralWidget;
    KToggleAction *_showAction;
    QGuardedPtr<QObject> _merge;
    QStringList _args;
};


class KstJSUIHelper : public KXMLGUIClient {
  public:
    KstJSUIHelper(const QString& ui, KstJS *js);
    virtual ~KstJSUIHelper();
    void merge();
    KActionCollection *actionCollection() const { return _js->actionCollection(); }
  private:
    KstJS *_js;
    QString _ui;
};


class KstJSUIBuilder : public QObject {
  Q_OBJECT
  public:
    KstJSUIBuilder(const QString& ui, KstJS *js);
    virtual ~KstJSUIBuilder();

  public slots:
    KActionCollection *actionCollection() const;
    void merge();

  private:
    KstJSUIHelper *_helper;
};


class KstUIMerge : public QObject {
  Q_OBJECT
  public:
    KstUIMerge(KstJS *parent, const char *name);
    virtual ~KstUIMerge();

  public slots:
    KstJSUIBuilder *loadGUI(const QString& ui);

  private:
    KstJS *_parent;
};

#endif

// vim: ts=2 sw=2 et
