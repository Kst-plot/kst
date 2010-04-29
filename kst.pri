
TOPOUTDIR=$$OUT_PWD/$$TOPOUT_REL
OUTPUT_DIR=$$TOPOUTDIR/build


INSTALL_PREFIX = $$(INSTALL_PREFIX)
unix {
  !mac {
    isEmpty(INSTALL_PREFIX):INSTALL_PREFIX=/usr
} }

INSTALL_LIBDIR = $$(INSTALL_LIBDIR)
unix {
  !mac {
    isEmpty(INSTALL_LIBDIR):INSTALL_LIBDIR=lib
} }

INSTALL_PREFIX=$$(DESTDIR)$$INSTALL_PREFIX

DEFINES += INSTALL_LIBDIR=\\\"$$INSTALL_LIBDIR\\\"

QT += core xml

CONFIG += silent warn_on

DEBUG_MODE = $$(KST_DEBUG_MODE)
contains(DEBUG_MODE, 1) {
  !win32:CONFIG += debug
  QMAKE_RPATHDIR += $$OUTPUT_DIR/lib $$OUTPUT_DIR/plugin
}



!win32:OBJECTS_DIR = tmp
!win32:MOC_DIR = tmp

TOPLEVELDIR = $$PWD

INCLUDEPATH += $$TOPLEVELDIR

LIBS += -L$$OUTPUT_DIR/lib -L$$OUTPUT_DIR/plugin
#If VERSION is set qmake appends '2' to each dll, but we don't need this numbering.
#VERSION = 2.0.0

# use precompiled headers
PRECOMPILED_HEADER = $$TOPLEVELDIR/pch.h
CONFIG += precompile_header

# similar to qtLibraryTarget from mkspecs\features\qt_functions.p
defineReplace(kstlib) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   mac:!static:contains(QT_CONFIG, qt_framework) {
      QMAKE_FRAMEWORK_BUNDLE_NAME = $$LIBRARY_NAME
      export(QMAKE_FRAMEWORK_BUNDLE_NAME)
   }
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
          else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}
