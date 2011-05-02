TOPOUT_REL = ../..
include($$PWD/$$TOPOUT_REL/kst.pri)
QT += gui \
    network \
    svg \
    opengl
TEMPLATE = lib
TARGET = $$kstlib(kst2app)
DESTDIR = $$OUTPUT_DIR/lib
win32:CONFIG += staticlib


!isEmpty(INSTALL_PREFIX) { 
    target.path = $$INSTALL_PREFIX/$$INSTALL_LIBDIR
    INSTALLS += target
}

INCLUDEPATH += tmp \
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$TOPLEVELDIR/src/widgets \
    $$TOPOUTDIR/src/widgets \
    $$TOPOUTDIR/src/libkstapp/tmp

macx {
	CONFIG += lib_bundle
	LIBS += -F$$OUTPUT_DIR/lib
	qtAddLibrary(kst2lib)
	qtAddLibrary(kst2widgets)
	qtAddLibrary(kst2math)
} else {
	LIBS += -L$$OUTPUT_DIR/lib \
		-L$$OUTPUT_DIR/plugin \
		-l$$kstlib(kst2widgets) \
		-l$$kstlib(kst2math) \
		-l$$kstlib(kst2lib)
}


SOURCES += aboutdialog.cpp \
    application.cpp \
    applicationsettings.cpp \
    applicationsettingsdialog.cpp \
    arrowitem.cpp \
    arrowitemdialog.cpp \
    arrowpropertiestab.cpp \
    axis.cpp \
    axistab.cpp \
    basicplugindialog.cpp \
    boxitem.cpp \
    bugreportwizard.cpp \
    builtingraphics.cpp \
    cartesianrenderitem.cpp \
    changedatasampledialog.cpp \
    changefiledialog.cpp \
    childviewoptionstab.cpp \
    choosecolordialog.cpp \
    circleitem.cpp \
    commandlineparser.cpp \
    contenttab.cpp \
    csddialog.cpp \
    curvedialog.cpp \
    databutton.cpp \
    databuttonaction.cpp \
    datadialog.cpp \
    datagui.cpp \
    datamanager.cpp \
    datasourcedialog.cpp \
    datatab.cpp \
    datawizard.cpp \
    debugdialog.cpp \
    debugnotifier.cpp \
    defaultlabelpropertiestab.cpp \
    dialog.cpp \
    dialoglaunchergui.cpp \
    dialogpage.cpp \
    dialogtab.cpp \
    differentiatecurvesdialog.cpp \
    dimensionstab.cpp \
    document.cpp \
    editmultiplewidget.cpp \
    ellipseitem.cpp \
    equationdialog.cpp \
    eventmonitordialog.cpp \
    exportgraphicsdialog.cpp \
    filltab.cpp \
    filterfitdialog.cpp \
    formatgridhelper.cpp \
    generaltab.cpp \
    graphicsfactory.cpp \
    gridtab.cpp \
    histogramdialog.cpp \
    imagedialog.cpp \
    labelcreator.cpp \
    labelitem.cpp \
    labelitemdialog.cpp \
    labelpropertiestab.cpp \
    labelrenderer.cpp \
    labeltab.cpp \
    layoutboxitem.cpp \
    layouttab.cpp \
    legenditem.cpp \
    legenditemdialog.cpp \
    legendtab.cpp \
    lineitem.cpp \
    logdialog.cpp \
    logwidget.cpp \
    mainwindow.cpp \
    markerstab.cpp \
    matrixdialog.cpp \
    matrixmodel.cpp \
    memorywidget.cpp \
    overridelabeltab.cpp \
    pictureitem.cpp \
    plotaxis.cpp \
    plotitem.cpp \
    plotitemdialog.cpp \
    plotitemmanager.cpp \
    plotmarkers.cpp \
    plotrenderitem.cpp \
    powerspectrumdialog.cpp \
	primitivemodel.cpp \
    rangetab.cpp \
    scalardialog.cpp \
    scalarmodel.cpp \
    scene.cpp \
    selectionrect.cpp \
    sessionmodel.cpp \
    sharedaxisboxitem.cpp \
    stringdialog.cpp \
    stringmodel.cpp \
    stroketab.cpp \
    svgitem.cpp \
    tabwidget.cpp \
    vectordialog.cpp \
    vectormodel.cpp \
    viewcommand.cpp \
    view.cpp \
    viewdialog.cpp \
    viewgridlayout.cpp \
    viewitem.cpp \
    viewitemdialog.cpp \
    viewmatrixdialog.cpp \
    viewprimitivedialog.cpp \
    viewvectordialog.cpp \
    pluginmenuitemaction.cpp \
    exportvectorsdialog.cpp
HEADERS += aboutdialog.h \
    application.h \
    applicationsettings.h \
    applicationsettingsdialog.h \
    arrowitem.h \
    arrowitemdialog.h \
    arrowpropertiestab.h \
    axis.h \
    axistab.h \
    basicplugindialog.h \
    boxitem.h \
    bugreportwizard.h \
    builtingraphics.h \
    cartesianrenderitem.h \
    changedatasampledialog.h \
    changefiledialog.h \
    childviewoptionstab.h \
    choosecolordialog.h \
    circleitem.h \
    commandlineparser.h \
    contenttab.h \
    csddialog.h \
    curvedialog.h \
    databutton.h \
    databuttonaction.h \
    datadialog.h \
    datagui.h \
    datamanager.h \
    dataref.h \
    datasourcedialog.h \
    datatab.h \
    datawizard.h \
    debugdialog.h \
    debugnotifier.h \
    defaultlabelpropertiestab.h \
    dialog.h \
    dialoglaunchergui.h \
    dialogpage.h \
    dialogtab.h \
    differentiatecurvesdialog.h \
    dimensionstab.h \
    document.h \
    editmultiplewidget.h \
    ellipseitem.h \
    equationdialog.h \
    eventmonitordialog.h \
    exportgraphicsdialog.h \
    filltab.h \
    filterfitdialog.h \
    formatgridhelper.h \
    generaltab.h \
    graphicsfactory.h \
    gridtab.h \
    histogramdialog.h \
    imagedialog.h \
    labelcreator.h \
    labelitem.h \
    labelitemdialog.h \
    labelpropertiestab.h \
    labelrenderer.h \
    labeltab.h \
    layoutboxitem.h \
    layouttab.h \
    legenditem.h \
    legenditemdialog.h \
    legendtab.h \
    lineitem.h \
    logdialog.cpp\
    logwidget.h \
    mainwindow.h \
    markerstab.h \
    matrixdialog.h \
    matrixmodel.h \
    memorywidget.h \
    overridelabeltab.h \
    pictureitem.h \
    plotaxis.h \
    plotitem.h \
    plotitemdialog.h \
    plotmarkers.h \
    plotitemmanager.h \
    plotrenderitem.h \
    powerspectrumdialog.h \
	primitivemodel.h \
    rangetab.h \
    scalardialog.h \
    scalarmodel.h \
    scene.h \
    selectionrect.h \
    sessionmodel.h \
    sharedaxisboxitem.h \
    stringdialog.h \
    stringmodel.h \
    svgitem.h \
    stroketab.h \
    tabwidget.h \
    vectordialog.h \
    vectormodel.h \
    viewcommand.h \
    view.h \
    viewdialog.h \
    viewgridlayout.h \
    viewitemdialog.h \
    viewitem.h \
    viewmatrixdialog.h \
    viewprimitivedialog.h \
    viewvectordialog.h \
    pluginmenuitemaction.h \
    logdialog.h \
    exportvectorsdialog.h
FORMS += aboutdialog.ui \
    arrowpropertiestab.ui \
    axistab.ui \
    basicplugintab.ui \
    bugreportwizard.ui \
    changedatasampledialog.ui \
    changefiledialog.ui \
    childviewoptionstab.ui \
    choosecolordialog.ui \
    contenttab.ui \
    csdtab.ui \
    curvetab.ui \
    datamanager.ui \
    datawizardpagedatapresentation.ui \
    datawizardpagedatasource.ui \
    datawizardpagefilters.ui \
    datawizardpageplot.ui \
    datawizardpagevectors.ui \
    debugdialog.ui \
    defaultlabelpropertiestab.ui \
    dialog.ui \
    differentiatecurvesdialog.ui \
    dimensionstab.ui \
    editmultiplewidget.ui \
    equationtab.ui \
    eventmonitortab.ui \
    exportgraphicsdialog.ui \
    filltab.ui \
    filterfittab.ui \
    generaltab.ui \
    gridtab.ui \
    histogramtab.ui \
    imagetab.ui \
    labelcreator.ui \
    labeltab.ui \
    labelpropertiestab.ui \
    layouttab.ui \
    legendtab.ui \
    logdialog.ui \
    markerstab.ui \
    matrixtab.ui \
    powerspectrumtab.ui \
    overridelabeltab.ui \
    rangetab.ui \
    scalartab.ui \
    stringtab.ui \
    stroketab.ui \
    vectortab.ui \
    viewmatrixdialog.ui \
    viewprimitivedialog.ui \
    viewvectordialog.ui \
    exportvectorsdialog.ui
RESOURCES += $$TOPLEVELDIR/src/images/images.qrc
