
include_directories( ${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${CMAKE_SOURCE_DIR}/kst/src/widgets ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_BINARY_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

########### next target ###############

set(kstapp_LIB_SRCS 
#    kstdatacollection-gui.cpp
#    dialoglauncher-gui.cpp
#    labelrenderer.cpp
#    kstnumbersequence.cpp
#    kstuinames.cpp
#    kstcurvedifferentiate.cpp
#    kstiface_impl.cpp
#    statuslabel.cpp
#    plotmimesource.cpp
#    kstplotdrag.cpp
#    kstviewobjectimagedrag.cpp
#    kstvectortable.cpp
#    kstmatrixtable.cpp
#    kstfittable.cpp
#    kstlogwidget.cpp
#    kstplotbase.cpp
#    kstviewlabel.cpp
#    kstplotlabel.cpp
#    kst2dplot.cpp
#    kstviewlegend.cpp
#    kstviewbox.cpp
#    kstviewline.cpp
#    kstviewellipse.cpp
#    kstviewpicture.cpp
#    kstviewbezier.cpp
#    kstviewarrow.cpp
#    kstviewwidget.cpp
#    kstviewobjectfactory.cpp
#    kstviewobject.cpp
#    kstplotgroup.cpp
#    kstborderedviewobject.cpp
#    ksttoplevelview.cpp
#    kstgfxmousehandler.cpp
#    kstgfxmousehandlerutils.cpp
#    kstgfxrectanglemousehandler.cpp
#    kstgfxlinemousehandler.cpp
#    kstgfxtextmousehandler.cpp
#    kstgfxellipsemousehandler.cpp
#    kstgfxpicturemousehandler.cpp
#    kstgfxarrowmousehandler.cpp
#    kstgfx2dplotmousehandler.cpp
#    kstmetaplot.cpp
#    kstdrag.cpp
#    kstplugindialog.cpp
#    kstfitdialog.cpp
#    kstfilterdialog.cpp
#    extensionmgr.cpp
#    ksteditviewobjectdialog.cpp
#    kstdatamanager.cpp
#    kstviewmanager.cpp
#    kstvectordialog.cpp
#    kstcsddialog.cpp
#    ksteqdialog.cpp
#    ksthsdialog.cpp
#    kstpsddialog.cpp
#    kstgraphfiledialog.cpp
#    kstimagedialog.cpp
#    kstquickstartdialog.cpp
#    updatethread.cpp
#    emailthread.cpp
#    kstprintoptionspage.cpp
#    kstviewscalarsdialog.cpp
#    kstviewstringsdialog.cpp
#    kstviewvectorsdialog.cpp
#    kstviewmatricesdialog.cpp
#    kstviewfitsdialog.cpp
#    kstchoosecolordialog.cpp
#    kstchangenptsdialog.cpp
#    kstchangefiledialog.cpp
#    ksteventmonitor.cpp
#    kstdebugdialog.cpp
#    kstmatrixdialog.cpp
#    kstmonochromedialog.cpp
#    kstalignment.cpp
#    ksteventmonitorentry.cpp
#    kstcurvedialog.cpp
#    kstbasicdialog.cpp
#    kstdoc.cpp
#    kstviewwindow.cpp
#    ktimezonecombo.cpp
#    kstdebugnotifier.cpp
#    kstdatanotifier.cpp
#    kstscalarlistview.cpp
#    kststringlistview.cpp
#    kst.cpp
   )

kde4_automoc(${kstapp_LIB_SRCS})

kde4_add_ui_files(kstapp_LIB_SRCS 
   editviewobjectdialog4.ui
   vectordialogwidget4.ui
   curvedifferentiate4.ui
   csddialogwidget4.ui
   eqdialogwidget4.ui
   histogramdialogwidget4.ui
   psddialogwidget4.ui
   imagedialogwidget4.ui
   matrixdialogwidget4.ui
   plugindialogwidget4.ui
   basicdialogwidget4.ui
   eventmonitorwidget4.ui
   pluginmanager4.ui
   curvedialogwidget4.ui
   viewlabelwidget4.ui
   viewlegendwidget4.ui
   view2dplotwidget4.ui
   datasourcemetadatadialog4.ui
   extensiondlg4.ui
   kstdatadialog4.ui
   choosecolordialog4.ui
   debugdialog4.ui
   datawizard4.ui
   changefiledialog4.ui
   graphfiledialog4.ui
   viewscalarsdialog4.ui
   viewstringsdialog4.ui
   viewvectorsdialog4.ui
   viewmatricesdialog4.ui
   viewfitsdialog4.ui
   changenptsdialog4.ui
   datamanager4.ui
   kstviewmanager4.ui
   vectorsavedialog4.ui
   kstsettingsdlg4.ui
   quickstartdialog4.ui
   monochromedialog4.ui
   )

#kde4_add_dcop_skels(kstapp_LIB_SRCS MESSAGE(STATUS "DCOP stuff was remove into kde4, convert it to dbus please.kde4_add_dcop_skels macro doesn't exist now. We keep it just to remember to port to dbus")

kde4_add_library(kstapp SHARED ${kstapp_LIB_SRCS})

target_link_libraries(kstapp ${KDE4_KDECORE_LIBS} ${KDE4_KDEPRINT_LIBS} kstwidgets)

set_target_properties(kstapp PROPERTIES VERSION ${KST_LIBKST_VERSION} SOVERSION 1)
install(TARGETS kstapp DESTINATION ${LIB_INSTALL_DIR} )


########### install files ###############




#original Makefile.am contents follow:

#
##AM_CXXFLAGS = -Wold-style-cast -Wredundant-decls -Wunused-macros -Wfloat-equal -Wno-multichar
#
#lib_LTLIBRARIES = libkstapp.la
#
#libkstapp_la_SOURCES = \
#	kstdatacollection-gui.cpp\
#	dialoglauncher-gui.cpp\
#	labelrenderer.cpp\
#	kstnumbersequence.cpp\
#	kstuinames.cpp\
#	editviewobjectdialog.ui \
#	vectordialogwidget.ui \
#	kstcurvedifferentiate.cpp\
#	curvedifferentiate.ui \
#	csddialogwidget.ui \
#	eqdialogwidget.ui \
#	histogramdialogwidget.ui \
#	psddialogwidget.ui \
#	imagedialogwidget.ui \
#	matrixdialogwidget.ui \
#	plugindialogwidget.ui \
#	basicdialogwidget.ui \
#	eventmonitorwidget.ui \
#	pluginmanager.ui \
#	curvedialogwidget.ui \
#	kstiface.skel \
#	kstiface_impl.cpp\
#	statuslabel.cpp\
#	plotmimesource.cpp\
#	kstplotdrag.cpp\
#	kstviewobjectimagedrag.cpp\
#	kstvectortable.cpp\
#	kstmatrixtable.cpp\
#	kstfittable.cpp\
#	kstlogwidget.cpp\
#	kstplotbase.cpp\
#	viewlabelwidget.ui \
#	viewlegendwidget.ui \
#	view2dplotwidget.ui \
#	kstviewlabel.cpp\
#	kstplotlabel.cpp\
#	kst2dplot.cpp\
#	kstviewlegend.cpp\
#	kstviewbox.cpp\
#	kstviewline.cpp\
#	kstviewellipse.cpp\
#	kstviewpicture.cpp\
#	kstviewbezier.cpp\
#	kstviewarrow.cpp\
#	kstviewwidget.cpp\
#	kstviewobjectfactory.cpp\
#	kstviewobject.cpp\
#	kstplotgroup.cpp\
#	kstborderedviewobject.cpp\
#	ksttoplevelview.cpp\
#	kstgfxmousehandler.cpp\
#	kstgfxmousehandlerutils.cpp\
#	kstgfxrectanglemousehandler.cpp\
#	kstgfxlinemousehandler.cpp\
#	kstgfxtextmousehandler.cpp\
#	kstgfxellipsemousehandler.cpp\
#	kstgfxpicturemousehandler.cpp\
#	kstgfxarrowmousehandler.cpp\
#	kstgfx2dplotmousehandler.cpp\
#	kstmetaplot.cpp\
#	kstdrag.cpp\
#	kstplugindialog.cpp\
#	kstfitdialog.cpp\
#	kstfilterdialog.cpp\
#	extensionmgr.cpp\
#	ksteditviewobjectdialog.cpp\
#	datasourcemetadatadialog.ui \
#	kstdatamanager.cpp\
#	kstviewmanager.cpp\
#	kstvectordialog.cpp\
#	kstcsddialog.cpp\
#	ksteqdialog.cpp\
#	ksthsdialog.cpp\
#	kstpsddialog.cpp\
#	kstgraphfiledialog.cpp\
#	kstimagedialog.cpp\
#	kstquickstartdialog.cpp\
#	updatethread.cpp\
#	emailthread.cpp\
#	kstprintoptionspage.cpp\
#	kstviewscalarsdialog.cpp\
#	kstviewstringsdialog.cpp\
#	kstviewvectorsdialog.cpp\
#	kstviewmatricesdialog.cpp\
#	kstviewfitsdialog.cpp\
#	kstchoosecolordialog.cpp\
#	kstchangenptsdialog.cpp\
#	kstchangefiledialog.cpp\
#	ksteventmonitor.cpp\
#	kstdebugdialog.cpp\
#	kstmatrixdialog.cpp\
#	kstmonochromedialog.cpp\
#	kstalignment.cpp\
#	ksteventmonitorentry.cpp\
#	kstcurvedialog.cpp\
#	kstbasicdialog.cpp\
#	kstdoc.cpp\
#	kstviewwindow.cpp\
#	ktimezonecombo.cpp\
#	extensiondlg.ui \
#	kstdatadialog.ui \
#	choosecolordialog.ui \
#	debugdialog.ui \
#	datawizard.ui \
#	changefiledialog.ui \
#	graphfiledialog.ui \
#	viewscalarsdialog.ui \
#	viewstringsdialog.ui \
#	viewvectorsdialog.ui \
#	viewmatricesdialog.ui \
#	viewfitsdialog.ui \
#	changenptsdialog.ui \
#	datamanager.ui \
#	kstviewmanager.ui \
#	vectorsavedialog.ui \
#	kstsettingsdlg.ui \
#	quickstartdialog.ui \
#	monochromedialog.ui \
#	kstdebugnotifier.cpp\
#	kstdatanotifier.cpp\
#	kstscalarlistview.cpp\
#	kststringlistview.cpp\
#	kst.cpp
#
#
#libkstapp_la_LIBADD = $(LIB_KDEPRINT) ../widgets/libkstwidgets.la $(LIB_KMDI)
#libkstapp_la_LDFLAGS = -version-info @KST_LIBKST_VERSION@ $(all_libraries) -no-undefined
#
## set the include path for X, qt and KDE
#INCLUDES=-I$(top_srcdir)/kst/src/extdate -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath -I../libkst -I$(top_srcdir)/kst/src/widgets -I../widgets -I$(top_srcdir) $(KMDI_INCLUDES) $(all_includes)
#
#messages: rc.cpp
#	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp-o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
#	if test -n "$$LIST"; then \
#	 $(XGETTEXT) $$LIST -o $(podir)/kst.pot; \
#	fi
#
#datawizard.lo: datawizard.ui.h datawizard.ui
#debugdialog.lo: debugdialog.ui.h debugdialog.ui
#editviewobjectdialog.lo: editviewobjectdialog.ui
#kst.lo: curvedifferentiate.h
#kst2dplot.lo: editviewobjectdialog.h plugindialogwidget.h matrixdialogwidget.h vectordialogwidget.h
#kstcsddialog_i.lo: csddialogwidget.h
#kstdatadialog.lo: kstdatadialog.ui.h kstdatadialog.ui
#kstdatamanager_i.lo: curvedialogwidget.h histogramdialogwidget.h imagedialogwidget.h
#ksteqdialog_i.lo: eqdialogwidget.h
#ksteventmonitor_i.lo: eventmonitorwidget.h
#kstfilterdialog_i.lo: plugindialogwidget.h
#kstfitdialog_i.lo: plugindialogwidget.h
#kstplugindialog_i.lo: pluginmanager.h plugindialogwidget.h
#kstpsddialog_i.lo: psddialogwidget.h
#kstsettingsdlg.lo: kstsettingsdlg.ui.h kstsettingsdlg.ui
#kstviewbox.lo: editviewobjectdialog.h
#kstviewlegend.lo: editviewobjectdialog.h
#matrixselector.lo: matrixselector.ui.h
#pluginmanager.lo: pluginmanager.ui.h pluginmanager.ui
#vectorsavedialog.lo: vectorsavedialog.ui.h vectorsavedialog.ui
#
#METASOURCES=AUTO
#
