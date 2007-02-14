
#AM_CXXFLAGS = -Wold-style-cast -Wredundant-decls -Wunused-macros -Wfloat-equal -Wno-multichar

lib_LTLIBRARIES = libkstapp.la

libkstapp_la_SOURCES = \
	kstdatacollection-gui.cpp \
	dialoglauncher-gui.cpp \
	labelrenderer.cpp \
	kstnumbersequence.cpp \
	kstuinames.cpp \
	editviewobjectdialog.ui \
	vectordialogwidget.ui \
	kstcurvedifferentiate_i.cpp \
	curvedifferentiate.ui \
	csddialogwidget.ui \
	eqdialogwidget.ui \
	histogramdialogwidget.ui \
	psddialogwidget.ui \
	imagedialogwidget.ui \
	matrixdialogwidget.ui \
	plugindialogwidget.ui \
	basicdialogwidget.ui \
	eventmonitorwidget.ui \
	pluginmanager.ui \
	curvedialogwidget.ui \
	kstiface.skel \
	kstiface_impl.cpp \
	statuslabel.cpp \
	plotmimesource.cpp \
	kstplotdrag.cpp \
	kstviewobjectimagedrag.cpp \
	kstvectortable.cpp \
	kstmatrixtable.cpp \
	kstfittable.cpp \
	kstlogwidget.cpp \
	kstplotbase.cpp \
	viewlabelwidget.ui \
	viewlegendwidget.ui \
	view2dplotwidget.ui \
	kstviewlabel.cpp \
	kstplotlabel.cpp \
	kst2dplot.cpp \
	kstviewlegend.cpp \
	kstviewbox.cpp \
	kstviewline.cpp \
	kstviewellipse.cpp \
	kstviewpicture.cpp \
	kstviewbezier.cpp \
	kstviewarrow.cpp \
	kstviewwidget.cpp \
	kstviewobjectfactory.cpp \
	kstviewobject.cpp \
	kstplotgroup.cpp \
	kstborderedviewobject.cpp \
	ksttoplevelview.cpp \
	kstgfxmousehandler.cpp \
	kstgfxmousehandlerutils.cpp \
	kstgfxrectanglemousehandler.cpp \
	kstgfxlinemousehandler.cpp \
	kstgfxtextmousehandler.cpp \
	kstgfxellipsemousehandler.cpp \
	kstgfxpicturemousehandler.cpp \
	kstgfxarrowmousehandler.cpp \
	kstgfx2dplotmousehandler.cpp \
	kstmetaplot.cpp \
	kstdrag.cpp \
	kstplugindialog_i.cpp \
	kstfitdialog_i.cpp \
	kstfilterdialog_i.cpp \
	extensionmgr.cpp \
	ksteditviewobjectdialog_i.cpp \
	datasourcemetadatadialog.ui \
	kstdatamanager_i.cpp \
	kstviewmanager_i.cpp \
	kstvectordialog_i.cpp \
	kstcsddialog_i.cpp \
	ksteqdialog_i.cpp \
	ksthsdialog_i.cpp \
	kstpsddialog_i.cpp \
	kstgraphfiledialog_i.cpp \
	kstimagedialog_i.cpp \
	kstquickstartdialog_i.cpp \
	updatethread.cpp \
	emailthread.cpp \
	kstprintoptionspage.cpp \
	kstviewscalarsdialog_i.cpp \
	kstviewstringsdialog_i.cpp \
	kstviewvectorsdialog_i.cpp \
	kstviewmatricesdialog_i.cpp \
	kstviewfitsdialog_i.cpp \
	kstchoosecolordialog_i.cpp \
	kstchangenptsdialog_i.cpp \
	kstchangefiledialog_i.cpp \
	ksteventmonitor_i.cpp \
	kstdebugdialog_i.cpp \
	kstmatrixdialog_i.cpp \
	kstmonochromedialog_i.cpp \
	kstalignment.cpp \
	ksteventmonitorentry.cpp \
	kstcurvedialog_i.cpp \
	kstbasicdialog_i.cpp \
	kstdoc.cpp \
	kstviewwindow.cpp \
	ktimezonecombo.cpp \
	extensiondlg.ui \
	kstdatadialog.ui \
	choosecolordialog.ui \
	debugdialog.ui \
	datawizard.ui \
	changefiledialog.ui \
	graphfiledialog.ui \
	viewscalarsdialog.ui \
	viewstringsdialog.ui \
	viewvectorsdialog.ui \
	viewmatricesdialog.ui \
	viewfitsdialog.ui \
	changenptsdialog.ui \
	datamanager.ui \
	kstviewmanager.ui \
	vectorsavedialog.ui \
	kstsettingsdlg.ui \
	quickstartdialog.ui \
	monochromedialog.ui \
	kstdebugnotifier.cpp \
	kstdatanotifier.cpp \
	kstscalarlistview.cpp \
	kststringlistview.cpp \
	kst.cpp


libkstapp_la_LIBADD = $(LIB_KDEPRINT) ../widgets/libkstwidgets.la $(LIB_KMDI)
libkstapp_la_LDFLAGS = -version-info @KST_LIBKST_VERSION@ $(all_libraries) -no-undefined

# set the include path for X, qt and KDE
INCLUDES=-I$(top_srcdir)/kst/src/extdate -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath -I../libkst -I$(top_srcdir)/kst/src/widgets -I../widgets -I$(top_srcdir) $(KMDI_INCLUDES) $(all_includes)

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kst.pot; \
	fi

datawizard.lo: datawizard.ui.h datawizard.ui
debugdialog.lo: debugdialog.ui.h debugdialog.ui
editviewobjectdialog.lo: editviewobjectdialog.ui
kst.lo: curvedifferentiate.h
kst2dplot.lo: editviewobjectdialog.h plugindialogwidget.h matrixdialogwidget.h vectordialogwidget.h
kstcsddialog_i.lo: csddialogwidget.h
kstdatadialog.lo: kstdatadialog.ui.h kstdatadialog.ui
kstdatamanager_i.lo: curvedialogwidget.h histogramdialogwidget.h imagedialogwidget.h
ksteqdialog_i.lo: eqdialogwidget.h
ksteventmonitor_i.lo: eventmonitorwidget.h
kstfilterdialog_i.lo: plugindialogwidget.h
kstfitdialog_i.lo: plugindialogwidget.h
kstplugindialog_i.lo: pluginmanager.h plugindialogwidget.h
kstpsddialog_i.lo: psddialogwidget.h
kstsettingsdlg.lo: kstsettingsdlg.ui.h kstsettingsdlg.ui
kstviewbox.lo: editviewobjectdialog.h
kstviewlegend.lo: editviewobjectdialog.h
matrixselector.lo: matrixselector.ui.h
pluginmanager.lo: pluginmanager.ui.h pluginmanager.ui
vectorsavedialog.lo: vectorsavedialog.ui.h vectorsavedialog.ui

METASOURCES=AUTO

