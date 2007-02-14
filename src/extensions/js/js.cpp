/***************************************************************************
                                   js.cpp
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

#define private public
#define protected public
#include <kjsembed/kjsembedpart.h>
#undef protected
#undef private

#include <config.h>

#include "js.h"
#include "jsiface_impl.h"

#include <kst.h>

#include <qdeepcopy.h>
#include <qlayout.h>
#include <qsplitter.h>

#include <kaction.h>
#include <kfiledialog.h>
#include <kgenericfactory.h>
#include <kjs/function.h>
#include <kjsembed/jsbuiltin.h>
#include <kjsembed/jsconsolewidget.h>
#include <kjsembed/jsfactory.h>
#include <kjsembed/jsproxy.h>
#include <kjsembed/jssecuritypolicy.h>
#include <kmessagebox.h>
#include <kparts/part.h>

#include "bind_arrow.h"
#include "bind_box.h"
#include "bind_curve.h"
#include "bind_datamatrix.h"
#include "bind_datasource.h"
#include "bind_datavector.h"
#include "bind_debug.h"
#include "bind_equation.h"
#include "bind_ellipse.h"
#include "bind_histogram.h"
#include "bind_jsdataobject.h"
#include "bind_kst.h"
#include "bind_label.h"
#include "bind_legend.h"
#include "bind_line.h"
#include "bind_matrix.h"
#include "bind_picture.h"
#include "bind_plot.h"
#include "bind_plugin.h"
#include "bind_point.h"
#include "bind_powerspectrum.h"
#include "bind_scalar.h"
#include "bind_size.h"
#include "bind_string.h"
#include "bind_vector.h"
#include "bind_window.h"


K_EXPORT_COMPONENT_FACTORY(kstextension_js, KGenericFactory<KstJS>)


class Function : public KJS::ObjectImp {
  public:
    virtual bool implementsCall() const {
      return true;
    }
};


class LoadScript : public Function {
  public:
  LoadScript(KstJS *ext) : Function(), _e(ext) {}
  virtual KJS::Value call(KJS::ExecState* exec, KJS::Object&, const KJS::List& args) {
    if (args.size() != 1) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly one argument.");
      exec->setException(eobj);
      return KJS::Undefined();
    }
    if (!_e) {
      return KJS::Boolean(false);
    }
    QString file = args[0].toString(exec).qstring();
    if (_e->_jsPart->runFile(file)) {
      if (!_e->_scripts.contains(file)) {
        _e->_scripts.append(file);
      }
      return KJS::Boolean(true);
    }
    return KJS::Boolean(false);
  }
  QGuardedPtr<KstJS> _e;
};


KstJS::KstJS(QObject *parent, const char *name, const QStringList& l) : KstExtension(parent, name, l), KXMLGUIClient() {
  KJSEmbed::JSSecurityPolicy::setDefaultPolicy(KJSEmbed::JSSecurityPolicy::CapabilityAll);
  _jsPart = new KJSEmbed::KJSEmbedPart(0L, "javascript", this, "kjsembedpart");
  createBindings();

#ifdef KST_HAVE_READLINE
  _showAction = new KToggleAction(i18n("Show &JavaScript Console"), 0, 0, 0, 0, actionCollection(), "js_console_show");
  connect(_showAction, SIGNAL(toggled(bool)), this, SLOT(doShow(bool)));
#endif
  new KAction(i18n("Load JavaScript..."), 0, 0, this, SLOT(loadScript()), actionCollection(), "js_load");
  new KAction(i18n("Clear JavaScript &Interpreter"), 0, 0, this, SLOT(resetInterpreter()), actionCollection(), "js_reset");
  setInstance(app()->instance());
  setXMLFile("kstextension_js.rc", true);
  app()->guiFactory()->addClient(this);

  _merge = new KstUIMerge(this, "KstUIMerge");
  _jsPart->addObject(_merge, _merge->name());

  createRegistry();
  _iface = new JSIfaceImpl(_jsPart);
  _splitter = 0L;
  _konsolePart = 0L;
}


KstJS::~KstJS() {
  delete _konsolePart;
  _konsolePart = 0L;
  delete _iface;
  _iface = 0L;
  destroyRegistry();
  KstApp *app = dynamic_cast<KstApp*>(this->app());
  if (app && app->guiFactory()) {
    app->guiFactory()->removeClient(this);
  }
}


void KstJS::processArguments(const QString& args) {
  _args.append(args);
  QTimer::singleShot(0, this, SLOT(doArgs()));
}


void KstJS::doArgs() {
  static bool executing = false;
  if (!_jsPart || executing) {
    QTimer::singleShot(0, this, SLOT(doArgs()));
    return;
  }
  executing = true;
  QStringList a = QDeepCopy<QStringList>(_args);
  _args.clear();
  for (QStringList::ConstIterator i = a.begin(); i != a.end(); ++i) {
    _jsPart->execute(*i);
  }
  executing = false;
}


void KstJS::createBindings() {
  _jsPart->factory()->addType("KstJSUIBuilder");
  _jsPart->factory()->addType("KActionCollection");
  if (_merge) {
    _jsPart->addObject(_merge, _merge->name());
  }

  KJS::ExecState *exec = _jsPart->globalExec();
  KJS::Object globalObj = _jsPart->globalObject();

  new KstBindPoint(exec, &globalObj);
  new KstBindSize(exec, &globalObj);
  
  new KstBindVector(exec, &globalObj);
  new KstBindScalar(exec, &globalObj);
  new KstBindString(exec, &globalObj);
  new KstBindDataSource(exec, &globalObj);
  new KstBindDataVector(exec, &globalObj);
  new KstBindMatrix(exec, &globalObj);
  new KstBindDataMatrix(exec, &globalObj);

  new KstBindEquation(exec, &globalObj);
  new KstBindPlugin(exec, &globalObj);
  new KstBindPowerSpectrum(exec, &globalObj);
  new KstBindHistogram(exec, &globalObj);
  //new KstBindJSDataObject(exec, &globalObj);

  new KstBindCurve(exec, &globalObj);

  new KstBindPlot(exec, &globalObj);
  new KstBindLine(exec, &globalObj);
  new KstBindLabel(exec, &globalObj);
  new KstBindLegend(exec, &globalObj);
  new KstBindBox(exec, &globalObj);
  new KstBindEllipse(exec, &globalObj);
  new KstBindPicture(exec, &globalObj);
  new KstBindArrow(exec, &globalObj);

  new KstBindWindow(exec, &globalObj);
  new KstBindDebug(exec, &globalObj);
  new KstBindKst(exec, &globalObj, this);

  globalObj.put(exec, "loadScript", KJS::Object(new LoadScript(this)));

  createRegistry();
}


void KstJS::resetInterpreter() {
  // FIXME: save and restore the script registry
  _jsPart->globalObject().imp()->deleteAllProperties(_jsPart->globalExec());
  while (KJS::Interpreter::collect()) {}
  _jsPart->interpreter()->initGlobalObject();
  delete _jsPart->builtins;
  KJS::Object obj = _jsPart->globalObject();
  _jsPart->createBuiltIn(_jsPart->globalExec(), obj);
  createBindings();
}


static char shellStr[30];

void KstJS::showConsole() {
#ifdef KST_HAVE_READLINE
  _showAction->setText(i18n("Hide &JavaScript Console"));
  if (!_konsolePart) {
    strcpy(shellStr, "SHELL=kstcmd");
    putenv(shellStr);
    KLibFactory *f = KLibLoader::self()->factory("libkonsolepart");
    if (!f) {
      KMessageBox::sorry(app(), i18n("Could not load konsole part.  Please install kdebase."));
      return;
    }
    _splitter = new QSplitter(Qt::Vertical, app());
    KParts::Part *p = dynamic_cast<KParts::Part*>(f->create(_splitter, "kstcmd"));
    if (!p) {
      KMessageBox::sorry(app(), i18n("Konsole part appears to be incompatible.  Please install kdebase correctly."));
      delete _splitter;
      return;
    }

    _oldCentralWidget = app()->centralWidget();
    _oldCentralWidget->reparent(_splitter, QPoint(0, 0));
    _splitter->moveToLast(p->widget());
    app()->setCentralWidget(_splitter);

    connect(p, SIGNAL(processExited()), this, SLOT(shellExited()));
    _konsolePart = p;
  }
  _splitter->show();
  _konsolePart->widget()->show();
#endif
}


void KstJS::shellExited() {
#ifdef KST_HAVE_READLINE
  _showAction->setChecked(false);
  _showAction->setText(i18n("Show &JavaScript Console"));
  _konsolePart = 0L;
  QTimer::singleShot(0, this, SLOT(restoreUI())); // konsole crashes otherwise
#endif
}


void KstJS::restoreUI() {
  if (_oldCentralWidget) {
    _oldCentralWidget->reparent(app(), QPoint(0, 0));
    app()->setCentralWidget(_oldCentralWidget);
  }
  delete _splitter;
  _splitter = 0L;
}


void KstJS::hideConsole() {
#ifdef KST_HAVE_READLINE
  _showAction->setText(i18n("Show &JavaScript Console"));
  if (_konsolePart) {
    _konsolePart->widget()->hide();
  }
#endif
}


void KstJS::doShow(bool show) {
  if (show) {
    showConsole();
  } else {
    hideConsole();
  }
}


void KstJS::loadScript() {
  QString fn = KFileDialog::getOpenFileName("::<kstfiledir>",
      i18n("*.js|JavaScript (*.js)\n*|All Files"),
      app(), i18n("Open Script"));

  if (!fn.isEmpty()) {
    if (_jsPart->runFile(fn)) {
      if (!_scripts.contains(fn)) {
        _scripts.append(fn);
      }
    } else {
      KJS::Completion c = _jsPart->completion();
      if (!c.isNull()) {
        QString err = c.toString(_jsPart->globalExec()).qstring();
        KMessageBox::error(app(), i18n("Error running script %1: %2").arg(fn).arg(err));
      } else {
        KMessageBox::error(app(), i18n("Unknown error running script %1.").arg(fn));
      }
    }
  }
}


void KstJS::load(QDomElement& e) {
  Q_UNUSED(e)
}


void KstJS::save(QTextStream& ts, const QString& indent) {
  Q_UNUSED(ts)
  Q_UNUSED(indent)
}


void KstJS::createRegistry() {
  QString registry =
    "function KstScriptRegistryPrototype() {\n"
    "  this.registry = Array();\n"
    "  this.addScript = function(name, factory) {\n"
    "    this.registry[name] = eval(\"new \" + factory);\n"
    "  };\n"
    "}\n"
    "\n"
    "var KstScriptRegistry = new KstScriptRegistryPrototype();\n"
    "\n";
  _jsPart->execute(registry);
}


void KstJS::destroyRegistry() {
  _jsPart->execute("delete KstScriptRegistry;");
}



/////////////////////////////////////////////////////////////////////////////

KstJSUIBuilder::KstJSUIBuilder(const QString& ui, KstJS *js)
  : QObject(js, ui.latin1()), _helper(new KstJSUIHelper(ui, js)) {
    //new KAction(i18n("LALALALA..."), 0, 0, 0, 0, js->actionCollection(), "kst_scalar_trigger");
}


KstJSUIBuilder::~KstJSUIBuilder() {
  delete _helper;
  _helper = 0L;
}


KActionCollection *KstJSUIBuilder::actionCollection() const {
  return _helper->actionCollection();
}


void KstJSUIBuilder::merge() {
  _helper->merge();
}


KstJSUIHelper::KstJSUIHelper(const QString& ui, KstJS *js)
: KXMLGUIClient(), _js(js), _ui(ui) {
  setInstance(_js->app()->instance());
}


KstJSUIHelper::~KstJSUIHelper() {
}


void KstJSUIHelper::merge() {
  setXMLFile(_ui, true);
  _js->app()->guiFactory()->addClient(this);
}


KstUIMerge::KstUIMerge(KstJS *parent, const char *name)
: QObject(parent, name), _parent(static_cast<KstJS*>(parent)) {
}


KstUIMerge::~KstUIMerge() {
}


KstJSUIBuilder *KstUIMerge::loadGUI(const QString& ui) {
  KstJSUIBuilder *b = new KstJSUIBuilder(ui, _parent);
  return b;
}


#include "js.moc"
// vim: ts=2 sw=2 et
