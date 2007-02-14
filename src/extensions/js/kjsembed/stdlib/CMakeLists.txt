
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  cmdline.js DESTINATION ${DATA_INSTALL_DIR}/kjsembed )



#original Makefile.am contents follow:

## -*- makefile -*-
#
#jslibdir = $(kde_datadir)/kjsembed
#jslib_DATA = cmdline.js
#
