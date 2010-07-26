TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_ascii)
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/ascii/tmp

SOURCES += \
  asciisource.cpp \
  asciisourceconfig.cpp \
  asciiplugin.cpp \
  kst_atof.cpp

HEADERS += \
  asciisource.h \
  asciisourceconfig.h \
  asciiplugin.h \
  namedparameter.h \
  kst_atof.h  \
  ../../libkst/kst_inf.h

FORMS += asciiconfig.ui

win32:DEFINES += KST_USE_KST_ATOF  

